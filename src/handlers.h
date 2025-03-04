#pragma once

#include <userver/kafka/producer.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value.hpp>

#include "model.h"

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
enum class SendStatus {
    kSuccess,
    kErrorRetryable,
    kErrorNonRetryable,
};
struct RequestMessage {
    std::string topic;
    std::string key;
    std::string payload;
};
class TSetPriceInfoHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-set-price-info";

    TSetPriceInfoHandler(const components::ComponentConfig& config, const components::ComponentContext& context);

    formats::json::Value HandleRequestJsonThrow(
        const server::http::HttpRequest& request,
        const formats::json::Value& request_json,
        server::request::RequestContext& context
    ) const override;

private:
    SendStatus Produce(const RequestMessage& message) const;

private:
    const kafka::Producer& Producer;
};

class TSetUserInfoHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-set-user-info";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &
    ) const override;
};

}
