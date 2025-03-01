#include "handlers.h"

#include <userver/kafka/producer_component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

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
) : server::handlers::HttpHandlerJsonBase{config, context},
    Producer{context.FindComponent<kafka::ProducerComponent>().GetProducer()}
{ }

SendStatus TSetPriceInfoHandler::Produce(const RequestMessage& message) const {
    try {
        Producer.Send(message.topic, message.key, message.payload);
        return SendStatus::kSuccess;
    } catch (const kafka::SendException& ex) {
        return ex.IsRetryable() ? SendStatus::kErrorRetryable : SendStatus::kErrorNonRetryable;
    }
}

RequestMessage Parse(const formats::json::Value& doc, formats::parse::To<RequestMessage>) {
    RequestMessage request_message;
    request_message.topic = doc["topic"].As<std::string>();
    request_message.key = doc["key"].As<std::string>();
    request_message.payload = doc["payload"].As<std::string>();

    return request_message;
}

formats::json::Value TSetPriceInfoHandler::HandleRequestJsonThrow(
    const server::http::HttpRequest& request,
    const formats::json::Value& request_json,
    server::request::RequestContext&
) const {
    const auto message = request_json.As<RequestMessage>();
    switch (Produce(message)) {
        case SendStatus::kSuccess:
            return formats::json::MakeObject("message", "Message send successfully");
        case SendStatus::kErrorRetryable:
            request.SetResponseStatus(server::http::HttpStatus::TooManyRequests);
            return formats::json::MakeObject("error", "Retry later");
        case SendStatus::kErrorNonRetryable:
            request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
            return formats::json::MakeObject("error", "Bad request");
    }
    UINVARIANT(false, "Unknown produce status");
}

std::string TSetUserInfoHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest&,
    userver::server::request::RequestContext&
) const {
    return "10";
};

}
