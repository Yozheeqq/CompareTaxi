#pragma once

#include <userver/kafka/consumer_scope.hpp>
#include <userver/kafka/producer.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value.hpp>

#include <userver/ydb/component.hpp>

using namespace userver;

namespace taxi_compare {

class TConsumerHandler final : public components::ComponentBase {
public:
    static constexpr std::string_view kName{"consumer-handler"};

    TConsumerHandler(const components::ComponentConfig& config, const components::ComponentContext& context);

private:
    void Consume(kafka::MessageBatchView messages) const;
    ydb::TableClient& Ydb() const { return *ydb_client_; }

private:
    std::shared_ptr<ydb::TableClient> ydb_client_;
    // Subscriptions must be the last fields! Add new fields above this comment.
    kafka::ConsumerScope Consumer;
};

} // taxi_compare
