#ifndef UDP_ECHO_SERVER_H
#define UDP_ECHO_SERVER_H

class UDPEchoServer {
 public:
  UDPEchoServer(int port);
  ~UDPEchoServer();

  void echo();

 private:
  int sockfd;
};

#endif
