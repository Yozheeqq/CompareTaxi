#include "handlers.h"

#include <userver/fs/blocking/read.hpp>

#include <filesystem>
#include <iostream>

namespace taxi_compare {

TGetPricePredictHandler::TGetPricePredictHandler(
    const components::ComponentConfig& config,
    const components::ComponentContext& context
) : server::handlers::HttpHandlerJsonBase{config, context}
  , Model("models/model.onnx")
{ }

formats::json::Value TGetPricePredictHandler::HandleRequestJsonThrow(
    [[maybe_unused]] const server::http::HttpRequest& request,
    [[maybe_unused]] const formats::json::Value& requestJson,
    [[maybe_unused]] server::request::RequestContext& context
) const {
    const auto predict = Model.GetPricePredict(TTaxiInfo{});
    return formats::json::MakeObject("Predict price", predict);
};

std::string TGetUserInfoHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest&,
    userver::server::request::RequestContext&
) const {
    return "10";
};

formats::json::Value TGetConfigHandler::HandleRequestJsonThrow(
    const server::http::HttpRequest& request,
    const formats::json::Value& requestJson,
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
