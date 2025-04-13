#pragma once

#include "kafka_helpers.h"
#include "model.h"

// #include <userver/components/component_config.hpp>
// #include <userver/components/component_context.hpp>

#include <userver/kafka/producer_component.hpp>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>


using namespace userver;

namespace taxi_compare {

class TGetPricePredictHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-get-price-predict";

    TGetPricePredictHandler(
        const components::ComponentConfig& config,
        const components::ComponentContext& context
    );

    formats::json::Value HandleRequestJsonThrow(
        const server::http::HttpRequest& request,
        const formats::json::Value& requestJson,
        server::request::RequestContext& context
    ) const override;

private:
    TModel Model;
};

class TGetUserInfoHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-get-user-info";

    TGetUserInfoHandler(
        const components::ComponentConfig& config,
        const components::ComponentContext& context
    ) : server::handlers::HttpHandlerJsonBase{config, context}
      , pg_cluster_(context.FindComponent<userver::components::Postgres>("user-info-database").GetCluster())
    { }

    formats::json::Value HandleRequestJsonThrow(
        const server::http::HttpRequest& request,
        const formats::json::Value& requestJson,
        server::request::RequestContext& context
    ) const override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

class TGetConfigHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-get-config";

    TGetConfigHandler(
        const components::ComponentConfig& config,
        const components::ComponentContext& context
    ) : server::handlers::HttpHandlerJsonBase{config, context}
    { }

    formats::json::Value HandleRequestJsonThrow(
        const server::http::HttpRequest& request,
        const formats::json::Value& requestJson,
        server::request::RequestContext& context
    ) const override;

    TString GetCurrentDirPath() const;
    TString GetFullConfigPath(const TString& type, const TString& name) const;
    TString GetTypeDirPath(const TString& type) const;
    std::vector<TString> GetFilesInDirectory(const TString& dirPath) const;
};

template <typename TStructType>
class ISetHandler : public userver::server::handlers::HttpHandlerJsonBase {
public:
    ISetHandler(
        const components::ComponentConfig& config,
        const components::ComponentContext& context,
        const std::string& topicName
    ) : server::handlers::HttpHandlerJsonBase{config, context},
        TopicName(topicName),
        Producer{context.FindComponent<kafka::ProducerComponent>().GetProducer()}
    { }

    formats::json::Value HandleRequestJsonThrow(
        const server::http::HttpRequest& request,
        const formats::json::Value& requestJson,
        [[maybe_unused]] server::request::RequestContext& context
    ) const override {
        const auto& requestInfo = ValidateJsonRequest<TStructType>(requestJson, ERequestType::Post);
        if (requestInfo.has_value()) {
            switch (Produce(requestJson, Producer, TopicName)) {
                case SendStatus::kSuccess:
                    return formats::json::MakeObject("message", "Message send successfully");
                case SendStatus::kErrorRetryable:
                    request.SetResponseStatus(server::http::HttpStatus::TooManyRequests);
                    return formats::json::MakeObject("error", "Retry later");
                case SendStatus::kErrorNonRetryable:
                    request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
                    return formats::json::MakeObject("error", "Bad request");
            }
        }
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return formats::json::MakeObject("Error", "Error while parsing request");
    }

protected:
    TString TopicName;
    const kafka::Producer& Producer;
};

class TSetPriceInfoHandler final : public ISetHandler<TTaxiInfo> {
public:
    static constexpr std::string_view kName = "handler-set-price-info";

    TSetPriceInfoHandler(const components::ComponentConfig& config, const components::ComponentContext& context);
};

class TSetUserInfoHandler final : public ISetHandler<TUserInfo> {
public:
    static constexpr std::string_view kName = "handler-set-user-info";

    TSetUserInfoHandler(const components::ComponentConfig& config, const components::ComponentContext& context);
};

}
