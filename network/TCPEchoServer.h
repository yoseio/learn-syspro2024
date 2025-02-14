#ifndef TCP_ECHO_SERVER_H
#define TCP_ECHO_SERVER_H

class TCPEchoServer {
 public:
  TCPEchoServer(int port);
  ~TCPEchoServer();

  void echo();

 private:
  int sockfd;
};

#endif
