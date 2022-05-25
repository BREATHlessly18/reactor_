#include "../core/epoller.hh"
#include "../core/server.hh"

const uint16_t g_port = 6666;
const char* g_ip = "0.0.0.0";

int main() {
  Epoller poller;

  Server server(g_port, g_ip, std::addressof(poller));
  server.start();

  poller.loop();

  return {};
}