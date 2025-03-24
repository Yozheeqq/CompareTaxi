#pragma once

#include "kafka_helpers.h"
#include "model.h"

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

#include <userver/kafka/producer_component.hpp>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>


using namespace userver;

namespace taxi_compare {

class TGetPricePredictHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-price-predict";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &
    ) const override;
};

class TGetUserInfoHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-user-info";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &
    ) const override;
};

class TGetConfigHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-config";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &
    ) const override;
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
        const auto& errorMessage = ValidateJsonRequest<TStructType>(requestJson, ERequestType::Post);
        if (errorMessage.empty()) {
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
        return formats::json::MakeObject("error", errorMessage);
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
