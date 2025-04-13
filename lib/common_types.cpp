#include "common_types.h"

namespace taxi_compare {

EWeatherType StringToWeatherType(const std::string& weatherStr) {
    if (weatherStr == "sun") {
        return EWeatherType::Sun;
    } else if (weatherStr == "rain") {
        return EWeatherType::Rain;
    } else if (weatherStr == "snow") {
        return EWeatherType::Snow;
    } else {
        throw std::runtime_error("Invalid weather type " + weatherStr);
    }
}

template<>
TUserInfo ValidateStruct<TUserInfo>(const formats::json::Value& json, ERequestType type) {
    TUserInfo userInfo;
    userInfo.PhoneId = json["phone_id"].As<TString>();
    if (type == ERequestType::Post) {
        userInfo.StartAddress = json["start_address"].As<TString>();
        userInfo.EndAddress = json["end_address"].As<TString>();
        userInfo.Timestamp = json["timestamp"].As<ui64>();
    }
    return userInfo;
}

template<>
TTaxiInfo ValidateStruct<TTaxiInfo>(const formats::json::Value& json, ERequestType type) {
    [[maybe_unused]] TTaxiInfo taxiInfo;
    taxiInfo.StartPointX = json["start_point_x"].As<double>();
    taxiInfo.StartPointY = json["start_point_y"].As<double>();
    taxiInfo.EndPointX = json["end_point_x"].As<double>();
    taxiInfo.EndPointY = json["end_point_y"].As<double>();
    taxiInfo.Timestamp = json["timestamp"].As<ui64>();
    taxiInfo.Distance = json["distance"].As<double>();
    taxiInfo.EstimateDuration = json["duration"].As<ui64>();
    if (type == ERequestType::Post) {
        taxiInfo.Price = json["price"].As<ui64>();
    }
    return taxiInfo;
}

template<>
TParserInfo ValidateStruct<TParserInfo>(const formats::json::Value& json, [[maybe_unused]] ERequestType type) {
    TParserInfo parserInfo;
    parserInfo.Type = json["type"].As<TString>();
    parserInfo.Name = json["name"].As<TString>();
    return parserInfo;
}

} // taxi_compare
