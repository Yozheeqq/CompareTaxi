#include "profile_updater.h"
#include "common_types.h"

#include <userver/kafka/consumer_component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

#include <userver/ydb/table.hpp>

#include <iostream>

namespace taxi_compare {

TProfileUpdaterHandler::TProfileUpdaterHandler(const components::ComponentConfig& config, const components::ComponentContext& context)
    : components::ComponentBase{config, context}
    , pg_cluster_(context.FindComponent<userver::components::Postgres>("user-info-database").GetCluster())
    , Consumer{context.FindComponent<kafka::ConsumerComponent>("kafka-consumer-profile-updater").GetConsumer()}
{
    Consumer.Start([this](kafka::MessageBatchView messages) {
        Consume(messages);
        Consumer.AsyncCommit();
    });
}

void TProfileUpdaterHandler::Consume(kafka::MessageBatchView messages) const {
    for (const auto& message : messages) {
        if (!message.GetTimestamp().has_value()) {
            continue;
        }

        try {
            const auto json = formats::json::FromString(message.GetPayload());

            const auto phone_id = json["phone_id"].As<std::string>();
            const auto phoneIdHash = static_cast<std::int64_t>(std::hash<std::string>{}(phone_id));
            const auto timestamp = json["timestamp"].As<int64_t>();
            const auto start_address = json["start_address"].As<std::string>();
            const auto end_address = json["end_address"].As<std::string>();

            auto result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "INSERT INTO \"user-info\" (phone_id, timestamp, start_address, end_address) "
                "VALUES ($1, $2, $3, $4) "
                "ON CONFLICT (phone_id, timestamp) DO NOTHING",
                phoneIdHash,
                timestamp,
                start_address,
                end_address
            );

            LOG_INFO() << "Inserted record for phone_id=" << phone_id
                      << ", affected_rows=" << result.RowsAffected();

        } catch (const std::exception& ex) {
            LOG_ERROR() << "Failed to process message: " << ex.what()
                       << ", payload: " << message.GetPayload();
        }
    }
}

} // taxi_compare
