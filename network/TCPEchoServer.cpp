#include "TCPEchoServer.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace {
constexpr int BACKLOG = 5;
constexpr int BUF_SIZE = 1024;
}  // namespace

TCPEchoServer::TCPEchoServer(int port) : sockfd(-1) {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

  if (listen(sockfd, BACKLOG) < 0) {
    close(sockfd);
    throw std::runtime_error("Failed to listen on the socket");
  }
}

TCPEchoServer::~TCPEchoServer() {
  if (sockfd >= 0) {
    close(sockfd);
  }
}

void TCPEchoServer::echo() {
  sockaddr_in addr{};
  socklen_t addr_len = sizeof(addr);

  int fd = accept(sockfd, reinterpret_cast<sockaddr*>(&addr), &addr_len);
  if (fd < 0) {
    std::perror("Failed to accept a connection");
    return;
  }

  // log
  char ip[INET_ADDRSTRLEN] = {0};
  if (inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip))) {
    std::cout << "From: " << ip << ":" << ntohs(addr.sin_port) << std::endl;
  }
  std::cout << "Data: ";

  while (true) {
    char buf[BUF_SIZE];
    std::memset(buf, 0, sizeof(buf));

    // recv
    const ssize_t read_len = read(fd, buf, BUF_SIZE - 1);
    if (read_len < 0) {
      std::perror("Failed to receive a message");
      break;
    }
    if (read_len == 0) {
      break;
    }

    // log
    std::cout << buf;

    // send
    const ssize_t write_len = write(fd, buf, read_len);
    if (write_len < 0) {
      std::perror("Failed to send a message");
      break;
    }
  }

  // log
  std::cout << "\n" << std::endl;

  close(fd);
}
