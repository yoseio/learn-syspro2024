#ifndef UDP_ECHO_CLIENT_H
#define UDP_ECHO_CLIENT_H

#include <string>

class UDPEchoClient {
 public:
  UDPEchoClient();
  ~UDPEchoClient();

  void echo(const std::string& ip, int port, const std::string& msg);

 private:
  int sockfd;
};

#endif
