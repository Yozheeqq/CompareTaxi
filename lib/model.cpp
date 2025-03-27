#include "model.h"

#include <userver/logging/log.hpp>

namespace taxi_compare {

TModel::TModel(
    const TString& pathToModel
) : Session(GetEnv(), pathToModel.c_str(), Ort::SessionOptions{})
{
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

std::vector<float> TModel::GetInputFeatures([[maybe_unused]] const TTaxiInfo& priceInfo) const {
    std::vector<float> result;
    for (int i = 0; i < 18; i++) {
        result.push_back(0.1f);
    }
    return result;
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
