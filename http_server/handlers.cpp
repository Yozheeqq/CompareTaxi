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

formats::json::Value TGetUserInfoHandler::HandleRequestJsonThrow(
    [[maybe_unused]] const server::http::HttpRequest& request,
    const formats::json::Value& requestJson,
    [[maybe_unused]] server::request::RequestContext& context
) const {
    const auto phoneId = requestJson["phone_id"].As<TString>();
    const auto phoneIdHash = static_cast<std::int64_t>(std::hash<std::string>{}(phoneId));
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM \"user-info\" "
        "WHERE phone_id = $1 ",
        phoneIdHash
    );

    formats::json::ValueBuilder jsonResult;

    for (const auto& row : result) {
        formats::json::ValueBuilder item;
        item["start_address"] = row["start_address"].As<std::string>();
        item["end_address"] = row["end_address"].As<std::string>();
        jsonResult.PushBack(item.ExtractValue());
    }
    return jsonResult.ExtractValue();
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
) : ISetHandler(config, context, "ml-info")
{ }

TSetUserInfoHandler::TSetUserInfoHandler(
    const components::ComponentConfig& config,
    const components::ComponentContext& context
) : ISetHandler(config, context, "user-info")
{ }

}
