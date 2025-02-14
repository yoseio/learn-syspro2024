#include "UDPEchoClient.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
// #include <thread>
#include <vector>

namespace {
constexpr size_t BUF_SIZE = 1024;
}

UDPEchoClient::UDPEchoClient() : sockfd(-1) {
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    throw std::runtime_error("Failed to create UDP socket");
  }
}

UDPEchoClient::~UDPEchoClient() {
  if (sockfd >= 0) {
    close(sockfd);
  }
}

void UDPEchoClient::echo(const std::string& ip,
                         int port,
                         const std::string& msg) {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
    throw std::runtime_error("Invalid server IP address: " + ip);
  }
  socklen_t addr_len = sizeof(addr);

  // send
  {
    size_t total_sent = 0;
    while (total_sent < msg.size()) {
      const size_t chunk_size = std::min(BUF_SIZE - 1, msg.size() - total_sent);
      std::string chunk = msg.substr(total_sent, chunk_size);

      const ssize_t send_len =
          sendto(sockfd, chunk.data(), chunk.size(), 0,
                 reinterpret_cast<sockaddr*>(&addr), addr_len);
      // std::this_thread::sleep_for(std::chrono::milliseconds(1));
      if (send_len < 0) {
        std::perror("Failed to send a message");
        return;
      }

      total_sent += chunk_size;
    }
    {
      char terminator = '\0';
      const ssize_t send_len =
          sendto(sockfd, &terminator, 1, 0, reinterpret_cast<sockaddr*>(&addr),
                 addr_len);
      if (send_len < 0) {
        std::perror("Failed to send null terminator");
        return;
      }
    }
  }

  // recv
  std::string received;
  while (true) {
    std::vector<char> buf(BUF_SIZE, 0);
    const ssize_t recv_len =
        recvfrom(sockfd, buf.data(), buf.size(), 0,
                 reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (recv_len < 0) {
      std::perror("Failed to receive a message");
      return;
    }
    if (recv_len == 0) {
      break;
    }

    bool terminator = false;
    for (ssize_t i = 0; i < recv_len; ++i) {
      if (buf[i] == '\0') {
        received.append(buf.data(), i);
        terminator = true;
        break;
      }
    }

    if (!terminator) {
      received.append(buf.data(), recv_len);
    } else {
      break;
    }
  }

  // check
  if (msg == received) {
    std::cout << "send message == recv message" << std::endl;
  } else {
    std::cout << "send message != recv message" << std::endl;
  }
}
