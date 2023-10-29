#include <server.hpp>

int main() {
  Server server(9999);

  server.initialize();
  server.loop();

  return 0;
}
