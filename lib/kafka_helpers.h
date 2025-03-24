#pragma once

#include "common_types.h"

#include <userver/kafka/consumer_scope.hpp>
#include <userver/kafka/producer.hpp>

enum class SendStatus {
    kSuccess,
    kErrorRetryable,
    kErrorNonRetryable,
};

SendStatus Produce(const formats::json::Value& requestJson, const kafka::Producer& producer, const std::string& topic);
