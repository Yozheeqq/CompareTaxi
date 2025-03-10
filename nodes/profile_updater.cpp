#include "profile_updater.h"

#include <userver/kafka/consumer_component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

#include <iostream>

namespace taxi_compare {

TConsumerHandler::TConsumerHandler(const components::ComponentConfig& config, const components::ComponentContext& context)
    : components::ComponentBase{config, context},
    Consumer{context.FindComponent<kafka::ConsumerComponent>("kafka-consumer-profile-updater").GetConsumer()} {
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

        std::cerr << "YOZHEEQ: Message=" << message.GetPayload();
    }
}

} // taxi_compare
