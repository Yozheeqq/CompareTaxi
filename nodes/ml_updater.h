#pragma once

#include <userver/kafka/consumer_scope.hpp>
#include <userver/kafka/producer.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

using namespace userver;

namespace taxi_compare {

class TMlUpdaterHandler final : public components::ComponentBase {
public:
    static constexpr std::string_view kName{"ml-updater-handler"};

    TMlUpdaterHandler(const components::ComponentConfig& config, const components::ComponentContext& context);

private:
    void Consume(kafka::MessageBatchView messages) const;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    // Subscriptions must be the last fields! Add new fields above this comment.
    kafka::ConsumerScope Consumer;
};

} // taxi_compare
