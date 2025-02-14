#include <cstdlib>
#include <iostream>
#include "UDPEchoServer.h"

int main(int argc, char* argv[]) {
  int port = 12345;
  if (argc == 2) {
    port = std::atoi(argv[1]);
  }

  std::cout << "UDP Echo Server (port: " << port << ")\n" << std::endl;

  UDPEchoServer server(port);

  while (true) {
    server.echo();
  }

  return 0;
}
