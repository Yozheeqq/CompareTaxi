#include "model.h"

#include <cmath>
#include <ctime>

#include <userver/logging/log.hpp>

namespace taxi_compare {

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
}

TModel::TModel(
    const TString& pathToModel
) : Session(GetEnv(), pathToModel.c_str(), Ort::SessionOptions{})
{
    LOG_INFO() << "Initializing model";
    Ort::AllocatorWithDefaultOptions allocator;

    const auto numInputs = Session.GetInputCount();
    InputNames.reserve(numInputs);
    for (size_t i = 0; i < numInputs; ++i) {
        auto name = Session.GetInputNameAllocated(i, allocator);
        InputNames.push_back(name.get());
    }

    auto numOutputs = Session.GetOutputCount();
    OutputNames.reserve(numOutputs);
    for (size_t i = 0; i < numOutputs; ++i) {
        auto name = Session.GetOutputNameAllocated(i, allocator);
        OutputNames.push_back(name.get());
    }

    LOG_INFO() << "Model loaded with " << numInputs << " inputs and " << numOutputs << " outputs";
}


// ['distance', 'dstLat', 'dstLon', 'order_duration_sec',
//     'plan_transporting_time_sec', 'srcLat', 'srcLon', 'minute_of_hour',
//     'hour_of_day', 'day_of_week', 'rounded_srcLat', 'rounded_srcLon',
//     'haversine_distance', 'src_to_center', 'dst_to_center', 'is_night']

std::vector<float> TModel::GetInputFeatures(const TTaxiInfo& priceInfo) const {
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
        static_cast<float>(IsNight(priceInfo.Timestamp))
    };
}

ui64 TModel::GetPricePredict(const TTaxiInfo& priceInfo) const {
    std::lock_guard<std::mutex> lock(SessionMutex);

    auto inputFeatures = GetInputFeatures(priceInfo);
    std::vector<int64_t> inputShape = {1, static_cast<int64_t>(inputFeatures.size())};

    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memory_info,
        inputFeatures.data(),
        inputFeatures.size(),
        inputShape.data(),
        inputShape.size()
    );

    std::vector<const char*> inputNamesC;
    inputNamesC.reserve(InputNames.size());
    for (const auto& name : InputNames) {
        inputNamesC.push_back(name.c_str());
    }

    std::vector<const char*> outputNamesC;
    outputNamesC.reserve(OutputNames.size());
    for (const auto& name : OutputNames) {
        outputNamesC.push_back(name.c_str());
    }

    auto outputTensors = Session.Run(
        Ort::RunOptions{nullptr},
        inputNamesC.data(),
        &inputTensor,
        inputNamesC.size(),
        outputNamesC.data(),
        outputNamesC.size()
    );

    float* outputData = outputTensors.front().GetTensorMutableData<float>();

    return static_cast<ui64>(std::round(outputData[0]));
}

} // taxi_compare
