#pragma once

#include <string.h>
#include <stdexcept>

#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value.hpp>

#include <userver/logging/log.hpp>

using namespace userver;

namespace taxi_compare {

using i64 = long long;
using ui64 = unsigned long long;
using TString = std::string;

enum class EWeatherType {
    Sun,
    Rain,
    Snow,
};

enum class ERequestType {
    Get,
    Post,
};

struct TUserInfo {
    TString PhoneId;
    TString StartAddress;
    TString EndAddress;
    ui64 Timestamp;
};

struct TTaxiInfo {
    double StartPointX;
    double StartPointY;
    double EndPointX;
    double EndPointY;
    ui64 Timestamp;
    double Distance;
    ui64 Price;
};

struct TParserInfo {
    TString Type;
    TString Name;
};

EWeatherType StringToWeatherType(const std::string& weatherStr);

template<typename T>
TString ValidateStruct(const formats::json::Value& json, ERequestType type);

template<typename T>
TString ValidateJsonRequest(const formats::json::Value& requestJson, ERequestType type) {
    try {
        return ValidateStruct<T>(requestJson, type);
    } catch (const std::exception& e) {
        LOG_ERROR() << "YOZHEEQ: Exc: " << e.what();
        return e.what();
    }
}

} // taxi_compare
