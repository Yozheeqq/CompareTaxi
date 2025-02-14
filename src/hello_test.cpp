#include "hello.hpp"

#include <userver/utest/utest.hpp>

UTEST(SayHelloTo, Basic) {
  EXPECT_EQ(service_template::SayHelloTo("Developer"), "You are awesome, Developer!\n");
  EXPECT_EQ(service_template::SayHelloTo({}), "You are awesome, unknown user!\n");
}
