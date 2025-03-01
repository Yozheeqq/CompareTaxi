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

} // taxi_compare
