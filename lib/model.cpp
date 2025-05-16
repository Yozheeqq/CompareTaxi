#include "model.h"

#include <cmath>
#include <ctime>

#include <userver/logging/log.hpp>

#include <userver/formats/json/value_builder.hpp>
#include <userver/clients/http/client.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/json/serialize.hpp>

namespace taxi_compare {

struct TWeatherInfo {
    double Temp;
    double WindSpeed;
    double RainVolume;
};

namespace {
    ui64 GetMinuteOfHour(time_t timestamp) {
        struct tm *timeinfo = localtime(&timestamp);
        return timeinfo->tm_min;
    }

    ui64 GetHourOfDay(time_t timestamp) {
        struct tm *timeinfo = localtime(&timestamp);
        return timeinfo->tm_hour;
    }

    ui64 GetDayOfWeek(time_t timestamp) {
        struct tm *timeinfo = localtime(&timestamp);
        return timeinfo->tm_wday;
    }

    bool IsNight(time_t timestamp) {
        int hour = GetHourOfDay(timestamp);
        return (hour >= 0 && hour <= 6) || (hour >= 22 && hour <= 23);
    }

    double RoundToDecimalPlaces(double value, ui64 digits = 2) {
        const auto precision = std::pow(10, digits);
        return std::round(value * precision) / precision;
    }

    double HaversineDistance(double srcLat, double srcLon, double dstLat, double dstLon) {
        static const double EARTH_RADIUS_KM = 6371.0;

        double srcLatRad = srcLat * M_PI / 180.0;
        double srcLonRad = srcLon * M_PI / 180.0;
        double dstLatRad = dstLat * M_PI / 180.0;
        double dstLonRad = dstLon * M_PI / 180.0;

        double dlat = dstLatRad - srcLatRad;
        double dlon = dstLonRad - srcLonRad;

        double a = sin(dlat / 2) * sin(dlat / 2) +
                   cos(srcLatRad) * cos(dstLatRad) *
                   sin(dlon / 2) * sin(dlon / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));

        return EARTH_RADIUS_KM * c;
    }

    double DistanceToCenter(double srcLat, double srcLon) {
        static const double centerLat = 55.752871, centerLon = 37.618093;
        return HaversineDistance(srcLat, srcLon, centerLat, centerLon);
    }

    TWeatherInfo GetWeatherInfo(double srcLat, double srcLon, time_t timestamp) {
        const auto& response = userver::GetAsyncRequest(
            "https://api.meteostat.net/v2/point/hourly",
            "lat", srcLat,
            "lon", srcLon,
            "start", timestamp
        );
        return TWeatherInfo{
            .Temp = response.GetArg("temp"),
            .WindSpeed = response.GetArg("wind"),
            .RainVolume = response.GetArg("rnvl"),
        };
    }
}

TModel::TModel(
    const components::ComponentContext& context
)   : HttpClient(context.FindComponent<components::HttpClient>().GetHttpClient())
{ }


// ['distance', 'dstLat', 'dstLon', 'order_duration_sec',
//     'plan_transporting_time_sec', 'srcLat', 'srcLon', 'minute_of_hour',
//     'hour_of_day', 'day_of_week', 'rounded_srcLat', 'rounded_srcLon',
//     'haversine_distance', 'src_to_center', 'dst_to_center', 'is_night']

std::vector<float> TModel::GetInputFeatures(const TTaxiInfo& priceInfo) const {
    const auto weatherInfo = GetWeatherInfo(priceInfo.StartPointY, priceInfo.StartPointX, priceInfo.Timestamp);
    return std::vector<float>{
        static_cast<float>(priceInfo.Distance),
        static_cast<float>(priceInfo.EndPointY),
        static_cast<float>(priceInfo.EndPointX),
        static_cast<float>(priceInfo.EstimateDuration + 300),
        static_cast<float>(priceInfo.EstimateDuration),
        static_cast<float>(priceInfo.StartPointY),
        static_cast<float>(priceInfo.StartPointX),
        static_cast<float>(GetMinuteOfHour(priceInfo.Timestamp)),
        static_cast<float>(GetHourOfDay(priceInfo.Timestamp)),
        static_cast<float>(GetDayOfWeek(priceInfo.Timestamp)),
        static_cast<float>(RoundToDecimalPlaces(priceInfo.EndPointY)),
        static_cast<float>(RoundToDecimalPlaces(priceInfo.EndPointX)),
        static_cast<float>(HaversineDistance(priceInfo.StartPointY, priceInfo.StartPointX, priceInfo.EndPointY, priceInfo.EndPointX)),
        static_cast<float>(DistanceToCenter(priceInfo.StartPointY, priceInfo.StartPointX)),
        static_cast<float>(DistanceToCenter(priceInfo.EndPointY, priceInfo.EndPointX)),
        static_cast<float>(IsNight(priceInfo.Timestamp)),
        static_cast<float>(weatherInfo.Temp),
        static_cast<float>(weatherInfo.WindSpeed),
        static_cast<float>(weatherInfo.RainVolume),
    };
}

formats::json::Value TModel::GetPricePredict(const TTaxiInfo& priceInfo) const {
    formats::json::ValueBuilder jsonBuilder;
    formats::json::ValueBuilder featuresArrayBuilder;
    for (auto i = 1; i <= 12; ++i) {
        auto nextPriceInfo = priceInfo;
        nextPriceInfo.Timestamp += i * 60 * 5;
        auto inputFeatures = GetInputFeatures(nextPriceInfo);
        formats::json::ValueBuilder featureBuilder;
        for (auto feature : inputFeatures) {
            featureBuilder.PushBack(feature);
        }
        featuresArrayBuilder.PushBack(featureBuilder.ExtractValue());
    }
    jsonBuilder["features"] = featuresArrayBuilder.ExtractValue();
    std::string jsonStr = formats::json::ToString(jsonBuilder.ExtractValue());

    auto response = HttpClient
        .CreateRequest()
        .url("http://localhost:5000/predict")
        .post()
        .headers({{"Content-Type", "application/json"}})
        .data(jsonStr)
        .perform()
    ;

    return formats::json::FromString(response->body());
}

} // taxi_compare
