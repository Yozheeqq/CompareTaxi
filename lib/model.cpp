#include "model.h"

namespace taxi_compare {

ui64 TModel::GetPricePredict(const TPriceInfo& priceInfo) {
    auto xDiff = priceInfo.StartPointX - priceInfo.EndPointX;
    auto yDiff = priceInfo.StartPointY - priceInfo.EndPointY;
    return (xDiff * xDiff + yDiff * yDiff) * priceInfo.Distance;
}

} // taxi_compare
