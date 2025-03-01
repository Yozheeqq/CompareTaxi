#pragma once

#include "common_types.h"

namespace taxi_compare {

struct TPriceInfo {
    double StartPointX;
    double StartPointY;
    double EndPointX;
    double EndPointY;
    ui64 Timestamp;
    EWeatherType Weather;
    double Distance;
};

class TModel {
public:

    static ui64 GetPricePredict(const TPriceInfo& priceInfo);
};

} // taxi_compare
