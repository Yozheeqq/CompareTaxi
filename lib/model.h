#pragma once

#include "common_types.h"

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

#include <userver/clients/http/component.hpp>

#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value.hpp>

using namespace userver;

namespace taxi_compare {

class TModel {
public:

    TModel(const userver::components::ComponentContext& context);

    formats::json::Value GetPricePredict(const TTaxiInfo& priceInfo) const;

private:
    std::vector<float> GetInputFeatures(const TTaxiInfo& priceInfo) const;

private:

    userver::clients::http::Client& HttpClient;
};

} // taxi_compare
