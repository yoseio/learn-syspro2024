#include <chrono>
#include <iostream>
#include "TCPEchoClient.h"

int main() {
  std::cout << "TCP Echo Client\n" << std::endl;
  TCPEchoClient client;

  std::string ip;
  int port = 0;

  std::cout << "Input server IP: ";
  if (!std::getline(std::cin, ip) || ip.empty()) {
    std::perror("Invalid IP input.");
    return 1;
  }

  std::cout << "Input server port: ";
  if (!(std::cin >> port)) {
    std::perror("Invalid port input.");
    return 1;
  }

  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cout << std::endl;

  while (true) {
    std::string msg;
    std::cout << "Input message (q/quit/exit to quit): ";
    if (!std::getline(std::cin, msg)) {
      std::perror("Input error");
      break;
    }

    if (msg == "q" || msg == "quit" || msg == "exit") {
      std::cout << "Exiting..." << std::endl;
      break;
    }

    const auto start = std::chrono::steady_clock::now();
    client.echo(ip, port, msg);
    const auto end = std::chrono::steady_clock::now();

    const auto rtt_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    std::cout << "RTT: " << rtt_us << " [us]\n" << std::endl;
  }
}
