#include "handlers.h"

#include <iostream>

namespace taxi_compare {

std::string TGetPricePredictHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&
) const {
    const auto priceInfo = TPriceInfo {
        std::stod(request.GetArg("start_point_x")),
        std::stod(request.GetArg("start_point_y")),
        std::stod(request.GetArg("end_point_x")),
        std::stod(request.GetArg("end_point_y")),
        static_cast<ui64>(std::stoi(request.GetArg("timestamp"))),
        StringToWeatherType(request.GetArg("weather")),
        std::stod(request.GetArg("distance"))
    };
    return std::to_string(TModel::GetPricePredict(priceInfo));
};

std::string TGetUserInfoHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest&,
    userver::server::request::RequestContext&
) const {
    return "10";
};

std::string TGetConfigHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest&,
    userver::server::request::RequestContext&
) const {
    return "10";
};

TSetPriceInfoHandler::TSetPriceInfoHandler(
    const components::ComponentConfig& config,
    const components::ComponentContext& context
) : ISetHandler(config, context, "price-info")
{ }

TSetUserInfoHandler::TSetUserInfoHandler(
    const components::ComponentConfig& config,
    const components::ComponentContext& context
) : ISetHandler(config, context, "user-info")
{ }

}
