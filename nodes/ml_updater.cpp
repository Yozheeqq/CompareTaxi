#include "ml_updater.h"
#include "common_types.h"

#include <userver/kafka/consumer_component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

#include <iostream>

namespace taxi_compare {

TMlUpdaterHandler::TMlUpdaterHandler(const components::ComponentConfig& config, const components::ComponentContext& context)
    : components::ComponentBase{config, context}
    , pg_cluster_(context.FindComponent<userver::components::Postgres>("ml-info-database").GetCluster())
    , Consumer{context.FindComponent<kafka::ConsumerComponent>("kafka-consumer-ml-updater").GetConsumer()}
{
    Consumer.Start([this](kafka::MessageBatchView messages) {
        Consume(messages);
        Consumer.AsyncCommit();
    });
}

void TMlUpdaterHandler::Consume(kafka::MessageBatchView messages) const {
    for (const auto& message : messages) {
        if (!message.GetTimestamp().has_value()) {
            continue;
        }

        try {
            const auto json = formats::json::FromString(message.GetPayload());

            const auto startPointX = json["start_point_x"].As<double>();
            const auto startPointY = json["start_point_y"].As<double>();
            const auto endPointX = json["end_point_x"].As<double>();
            const auto endPointY = json["end_point_y"].As<double>();
            const auto timestamp = json["timestamp"].As<int64_t>();
            const auto price = json["price"].As<int64_t>();
            const auto distance = json["distance"].As<double>();

            auto result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "INSERT INTO \"ml-info\" (start_point_x, start_point_y, end_point_x, end_point_y, timestamp, price, distance) "
                "VALUES ($1, $2, $3, $4, $5, $6, $7) ",
                startPointX,
                startPointY,
                endPointX,
                endPointY,
                timestamp,
                price,
                distance
            );

            LOG_INFO() << "Inserted record for ml-info, affected_rows=" << result.RowsAffected();

        } catch (const std::exception& ex) {
            LOG_ERROR() << "Failed to process message: " << ex.what()
                       << ", payload: " << message.GetPayload();
        }
    }
}

} // taxi_compare
