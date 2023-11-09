#include <server.hpp>
#include <protocol.hpp>

int main() {
  auto protocol_creator = std::make_unique<HttpProtocolCreator>();
  auto http_protocol = protocol_creator->create(HttpStandard::HTTP_0_9);

  Server server(9999, std::move(http_protocol));

  server.initialize();
  server.loop();

  return 0;
}
