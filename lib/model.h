#pragma once

#include "common_types.h"

#include <cpu_provider_factory.h>
#include <onnxruntime_cxx_api.h>

namespace taxi_compare {

class TModel {
public:

    TModel(const TString& pathToModel);

    ui64 GetPricePredict(const TTaxiInfo& priceInfo) const;

private:

    static Ort::Env& GetEnv() {
        static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNXModel");
        return env;
    }

    std::vector<float> GetInputFeatures(const TTaxiInfo& priceInfo) const;

private:

    mutable Ort::Session Session;
    mutable std::mutex SessionMutex;
    std::vector<std::string> InputNames;
    std::vector<std::string> OutputNames;
};

} // taxi_compare
