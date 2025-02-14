#include "TCPEchoClient.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

static const int BUF_SIZE = 1024;

TCPEchoClient::TCPEchoClient() : sockfd(-1) {}

TCPEchoClient::~TCPEchoClient() {
  if (sockfd >= 0) {
    close(sockfd);
  }
}

void TCPEchoClient::echo(const std::string& ip,
                         int port,
                         const std::string& msg) {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    throw std::runtime_error("Failed to create socket");
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
    throw std::runtime_error("Invalid server IP address: " + ip);
  }
  socklen_t addr_len = sizeof(addr);

  if (connect(sockfd, reinterpret_cast<sockaddr*>(&addr), addr_len) < 0) {
    throw std::runtime_error("Failed to connect to server");
  }

  // send
  ssize_t total_sent = 0;
  ssize_t msg_size = static_cast<ssize_t>(msg.size());
  while (total_sent < msg_size) {
    ssize_t remain = msg_size - total_sent;
    ssize_t write_len = write(sockfd, msg.data() + total_sent, remain);
    if (write_len < 0) {
      std::perror("Failed to send message");
      close(sockfd);
      sockfd = -1;
      return;
    }
    total_sent += write_len;
  }

  // recv
  ssize_t total_received = 0;
  std::vector<char> recv_buf;
  recv_buf.resize(msg_size);
  while (total_received < msg_size) {
    ssize_t remain = msg_size - total_received;
    ssize_t chunk_size = (remain < BUF_SIZE) ? remain : BUF_SIZE;
    ssize_t read_len = read(sockfd, &recv_buf[total_received], chunk_size);
    if (read_len < 0) {
      std::perror("Failed to receive message");
      close(sockfd);
      sockfd = -1;
      return;
    }
    if (read_len == 0) {
      break;
    }
    total_received += read_len;
  }

  // check
  std::string received(recv_buf.data(), total_received);
  if (msg == received) {
    std::cout << "send message == recv message" << std::endl;
  } else {
    std::cout << "send message != recv message" << std::endl;
  }

  if (close(sockfd) < 0) {
    std::perror("Failed to close socket");
  }
  sockfd = -1;
}
