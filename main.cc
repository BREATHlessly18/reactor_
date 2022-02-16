#include <tuple>

#include "assert.h"
#include "server.hh"
#include "timer.hh"
#include "utils/json.hh"

auto netInfo(nlohmann::json const &json) {
  return std::tuple{static_cast<uint16_t>(json.at("port")),
                    static_cast<std::string>(json.at("ip"))};
}

void hello() { ::printf("end\n"); }

auto main() -> int {
  nlohmann::json json{};
  jackson::parse("../config.json", json);
  assert(!json.empty());

  auto [port, ip] = netInfo(json);

  Epoller eventloop{};
  Server server{port, ip.data(), std::addressof(eventloop)};
  server.start();

  Timer timer(5, hello, std::addressof(eventloop));
  timer.go();

  eventloop.runAfter(20, [&] { eventloop.quit(); });

  eventloop.loop();

  return 0;
}