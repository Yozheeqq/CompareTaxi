#include "handlers.h"

#include <userver/fs/blocking/read.hpp>

#include <filesystem>
#include <iostream>

namespace taxi_compare {

TGetPricePredictHandler::TGetPricePredictHandler(
    const components::ComponentConfig& config,
    const components::ComponentContext& context
) : server::handlers::HttpHandlerJsonBase{config, context}
  , Model(context)
{ }

formats::json::Value TGetPricePredictHandler::HandleRequestJsonThrow(
    [[maybe_unused]] const server::http::HttpRequest& request,
    [[maybe_unused]] const formats::json::Value& requestJson,
    [[maybe_unused]] server::request::RequestContext& context
) const {
    const auto& taxiInfo = ValidateJsonRequest<TTaxiInfo>(requestJson, ERequestType::Get);
    if (taxiInfo.has_value()) {
        // const auto predict = Model.GetPricePredict(taxiInfo.value());
        return Model.GetPricePredict(taxiInfo.value());
    } else {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return formats::json::MakeObject("Error", "Error while parsing request");
    }
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
    const auto& parserInfo = ValidateJsonRequest<TParserInfo>(requestJson, ERequestType::Get);
    if (parserInfo.has_value()) {
        try {
            const auto jsonContent = userver::fs::blocking::ReadFileContents(GetFullConfigPath(parserInfo->Type, parserInfo->Name));
            return userver::formats::json::FromString(jsonContent);
        } catch (std::exception& e) {
            request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
            return formats::json::MakeObject("Error", e.what());
        }
    } else {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return formats::json::MakeObject("Error", "Error while parsing request");
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
