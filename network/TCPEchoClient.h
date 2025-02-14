#ifndef TCP_ECHO_CLIENT_H
#define TCP_ECHO_CLIENT_H

#include <string>

class TCPEchoClient {
 public:
  TCPEchoClient();
  ~TCPEchoClient();

  void echo(const std::string& ip, int port, const std::string& msg);

 private:
  int sockfd;
};

#endif
