#include "kafka_helpers.h"

#include <userver/formats/json/string_builder.hpp>
#include <userver/logging/log.hpp>

SendStatus Produce(const formats::json::Value& requestJson, const kafka::Producer& producer, const std::string& topic) {
    formats::json::StringBuilder sb;
    WriteToStream(requestJson, sb);
    try {
        producer.Send(topic, "", sb.GetString());
        LOG_INFO() << "YOZHEEQ: Message sent successfully\n";
        return SendStatus::kSuccess;
    } catch (const kafka::SendException& ex) {
        return ex.IsRetryable() ? SendStatus::kErrorRetryable : SendStatus::kErrorNonRetryable;
    }
}
