#include "UDPEchoServer.h"

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

UDPEchoServer::UDPEchoServer(int port) : sockfd(-1) {
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd < 0) {
    throw std::runtime_error("Failed to create a socket");
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  socklen_t addr_len = sizeof(addr);

  if (bind(sockfd, reinterpret_cast<sockaddr*>(&addr), addr_len) < 0) {
    close(sockfd);
    throw std::runtime_error("Failed to bind the socket");
  }
}

UDPEchoServer::~UDPEchoServer() {
  if (sockfd >= 0) {
    close(sockfd);
  }
}

void UDPEchoServer::echo() {
  sockaddr_in addr{};
  socklen_t addr_len = sizeof(addr);

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

  // log
  char ip[INET_ADDRSTRLEN] = {0};
  if (inet_ntop(AF_INET, &(addr.sin_addr), ip, sizeof(ip))) {
    std::cout << "From: " << ip << ":" << ntohs(addr.sin_port) << std::endl;
  }
  std::cout << "Data: " << received << "\n" << std::endl;

  // send
  size_t total_sent = 0;
  while (total_sent < received.size()) {
    const size_t chunk_size =
        std::min(BUF_SIZE - 1, received.size() - total_sent);
    std::string chunk = received.substr(total_sent, chunk_size);

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
    char null_terminator = '\0';
    const ssize_t send_len =
        sendto(sockfd, &null_terminator, 1, 0,
               reinterpret_cast<sockaddr*>(&addr), addr_len);
    if (send_len < 0) {
      std::perror("Failed to send a null terminator");
      return;
    }
  }
}
