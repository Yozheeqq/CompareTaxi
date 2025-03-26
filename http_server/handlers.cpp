#include "handlers.h"

#include <userver/fs/blocking/read.hpp>

#include <filesystem>
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

formats::json::Value TGetConfigHandler::HandleRequestJsonThrow(
    [[maybe_unused]] const server::http::HttpRequest& request,
    [[maybe_unused]] const formats::json::Value& requestJson,
    [[maybe_unused]] server::request::RequestContext& context
) const {
    const auto& errorMessage = ValidateJsonRequest<TParserInfo>(requestJson, ERequestType::Get);
    if (errorMessage.empty()) {
        const auto type = requestJson["type"].As<TString>();
        const auto name = requestJson["name"].As<TString>();
        try {
            const auto jsonContent = userver::fs::blocking::ReadFileContents(GetFullConfigPath(type, name));
            return userver::formats::json::FromString(jsonContent);
        } catch (std::exception& e) {
            request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
            return formats::json::MakeObject("Error reading file", e.what());
        }
    } else {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return formats::json::MakeObject("Error while parsing request", errorMessage);
    }
};

TString TGetConfigHandler::GetCurrentDirPath() const {
    return std::filesystem::current_path().string();
}
TString TGetConfigHandler::GetFullConfigPath(const TString& type, const TString& name) const {
    const auto& dirPath = GetCurrentDirPath();
    return (std::filesystem::path(dirPath) / "configs" / "parsers" / type / (name + ".json")).string();
}

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
