#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include "model.h"

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

class TSetPriceInfoHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-set-price-info";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &
    ) const override;
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
