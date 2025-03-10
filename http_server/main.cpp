#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/kafka/producer_component.hpp>

#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

#include "handlers.h"

int main(int argc, char* argv[]) {
  auto component_list =
    userver::components::MinimalServerComponentList()
      .Append<userver::server::handlers::Ping>()
      .Append<userver::components::HttpClient>()
      .Append<userver::components::Secdist>()
      .Append<components::DefaultSecdistProvider>()
      .Append<userver::clients::dns::Component>()
      .Append<userver::server::handlers::TestsControl>()
      .Append<taxi_compare::TGetPricePredictHandler>()
      .Append<taxi_compare::TGetUserInfoHandler>()
      .Append<taxi_compare::TGetConfigHandler>()
      .Append<taxi_compare::TSetPriceInfoHandler>()
      .Append<taxi_compare::TSetUserInfoHandler>()
      .Append<kafka::ProducerComponent>("kafka-producer")
    ;

  return userver::utils::DaemonMain(argc, argv, component_list);
}
