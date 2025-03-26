#include "profile_updater.h"
#include "common_types.h"

#include <userver/kafka/consumer_component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

#include <userver/ydb/table.hpp>

#include <iostream>

namespace taxi_compare {

TConsumerHandler::TConsumerHandler(const components::ComponentConfig& config, const components::ComponentContext& context)
    : components::ComponentBase{config, context}
    , ydb_client_(context.FindComponent<ydb::YdbComponent>().GetTableClient("compare-taxi"))
    , Consumer{context.FindComponent<kafka::ConsumerComponent>("kafka-consumer-profile-updater").GetConsumer()}
{
    Consumer.Start([this](kafka::MessageBatchView messages) {
        Consume(messages);
        Consumer.AsyncCommit();
    });
}

void TConsumerHandler::Consume(kafka::MessageBatchView messages) const {
    for (const auto& message : messages) {
        if (!message.GetTimestamp().has_value()) {
            continue;
        }

        const auto json = formats::json::FromString(message.GetPayload());

        static const ydb::Query kUpsertQuery{
            R"(
                --!syntax_v1
                DECLARE $phone_id AS Uint64;
                DECLARE $timestamp AS Uint64;
                DECLARE $start_address AS String;
                DECLARE $end_address AS String;

                UPSERT INTO test/test-table (phone_id, timestamp, start_address, end_address)
                VALUES ($phone_id, $timestamp, $start_address, $end_address);
            )",
            ydb::Query::Name{"upsert-row"},
        };

        const auto phoneId = json["phone_id"].As<std::string>();
        const auto phoneIdHash = static_cast<std::uint64_t>(std::hash<std::string>{}(phoneId));

        auto response = Ydb().ExecuteDataQuery(
            kUpsertQuery,  //
            "$phone_id",
            phoneIdHash,  //
            "$timestamp",
            json["timestamp"].As<std::uint64_t>(),  //
            "$start_address",
            json["start_address"].As<std::string>(),  //
            "$end_address",
            json["end_address"].As<std::string>()  //
        );

        if (response.GetCursorCount()) {
            std::cerr << "YOZHEEQ: Unexpected response data";
        }

        std::cerr << "YOZHEEQ: Message=" << message.GetPayload();
    }
}

} // taxi_compare
