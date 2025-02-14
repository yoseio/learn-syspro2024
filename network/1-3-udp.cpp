#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include "UDPEchoClient.h"

std::string generate_random_string(size_t length) {
  const std::string chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);

  std::string random_string;
  for (size_t i = 0; i < length; ++i) {
    random_string += chars[dist(gen)];
  }

  return random_string;
}

int main() {
  std::cout << "UDP Echo Client (Random)\n" << std::endl;
  UDPEchoClient client;

  std::ofstream ofs("1-3-udp.csv");
  if (!ofs) {
    perror("Failed to open file.");
    return 1;
  }
  ofs << "i,rtt_us" << std::endl;

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

  for (int i = 0; i <= 20; i++) {
    for (int j = 0; j < 100; j++) {
      std::string msg = generate_random_string(pow(2, i));

      const auto start = std::chrono::steady_clock::now();
      client.echo(ip, port, msg);
      const auto end = std::chrono::steady_clock::now();

      const auto rtt_us =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start)
              .count();
      std::cout << "i: " << i << " / j: " << j << std::endl;
      std::cout << "RTT: " << rtt_us << " [us]\n" << std::endl;
      ofs << i << "," << rtt_us << std::endl;
    }
  }
}
