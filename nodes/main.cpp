#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/kafka/producer_component.hpp>
#include <userver/kafka/consumer_component.hpp>

#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

#include "profile_updater.h"

int main(int argc, char* argv[]) {
  auto component_list =
    userver::components::MinimalServerComponentList()
      .Append<userver::server::handlers::Ping>()
      .Append<userver::components::HttpClient>()
      .Append<userver::components::Secdist>()
      .Append<components::DefaultSecdistProvider>()
      .Append<userver::clients::dns::Component>()
      .Append<userver::server::handlers::TestsControl>()
      .Append<kafka::ConsumerComponent>("kafka-consumer-profile-updater")
      .Append<taxi_compare::TConsumerHandler>()
    ;

  return userver::utils::DaemonMain(argc, argv, component_list);
}
