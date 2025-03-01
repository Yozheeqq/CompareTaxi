#pragma once

#include <string.h>
#include <stdexcept>

namespace taxi_compare {

using i64 = long long;
using ui64 = unsigned long long;

enum class EWeatherType {
    Sun,
    Rain,
    Snow,
};

EWeatherType StringToWeatherType(const std::string& weatherStr);

} // taxi_compare
