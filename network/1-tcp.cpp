#include <cstdlib>
#include <iostream>
#include "TCPEchoServer.h"

int main(int argc, char* argv[]) {
  int port = 12345;
  if (argc == 2) {
    port = std::atoi(argv[1]);
  }

  std::cout << "TCP Echo Server (port: " << port << ")\n" << std::endl;

  TCPEchoServer server(port);

  while (true) {
    server.echo();
  }

  return 0;
}
