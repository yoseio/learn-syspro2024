#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class TCPClient {
 public:
  TCPClient() : sock_(-1) {}
  ~TCPClient() {
    if (sock_ >= 0) {
      close(sock_);
    }
  }

  bool connectToServer(const std::string& serverIP, int port) {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
      perror("socket");
      return false;
    }

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
      perror("inet_pton");
      closeSocket();
      return false;
    }

    if (connect(sock_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
      perror("connect");
      closeSocket();
      return false;
    }
    return true;
  }

  ssize_t sendData(const std::string& data) {
    if (sock_ < 0) {
      return -1;
    }
    ssize_t result = write(sock_, data.c_str(), data.size());
    if (result < 0) {
      perror("write");
    }
    return result;
  }

  ssize_t receiveData(std::string& out, size_t maxSize = 1024) {
    if (sock_ < 0) {
      return -1;
    }
    std::vector<char> buf(maxSize + 1, 0);
    ssize_t received = read(sock_, buf.data(), maxSize);
    if (received > 0) {
      buf[received] = '\0';
      out = std::string(buf.data());
    } else if (received < 0) {
      perror("read");
    }
    return received;
  }

  void closeSocket() {
    if (sock_ >= 0) {
      close(sock_);
      sock_ = -1;
    }
  }

 private:
  int sock_;
};

class MessageClient {
 public:
  MessageClient(const std::string& serverIP, int port)
      : serverIP_(serverIP), port_(port) {}

  bool sendMessage(const std::string& myName,
                   const std::string& destName,
                   const std::string& message) {
    if (!tcpClient_.connectToServer(serverIP_, port_)) {
      return false;
    }

    std::ostringstream oss;
    oss << "1iSAUYKj send-to " << myName << " " << destName << " \"" << message
        << "\"\n";
    std::string out = oss.str();

    if (tcpClient_.sendData(out) < 0) {
      tcpClient_.closeSocket();
      return false;
    }

    std::string response;
    ssize_t received = tcpClient_.receiveData(response, 1024);
    if (received > 0) {
      std::cout << "Server Response: " << response << std::endl;
    }

    tcpClient_.closeSocket();
    return true;
  }

 private:
  std::string serverIP_;
  int port_;
  TCPClient tcpClient_;
};

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <server_ip> <port> <myName> <destName>\n";
    return 1;
  }

  std::string serverIP = argv[1];
  int serverPort = std::atoi(argv[2]);
  std::string myName = argv[3];
  std::string destName = argv[4];

  MessageClient msgClient(serverIP, serverPort);

  while (true) {
    std::cout << "Enter message (or just '.' to quit): ";
    std::string message;
    std::getline(std::cin, message);

    if (!std::cin.good()) {
      break;
    }

    if (message == ".") {
      std::cout << "Bye.\n";
      break;
    }
    if (message.empty()) {
      continue;
    }

    bool ok = msgClient.sendMessage(myName, destName, message);
    if (!ok) {
      std::cerr << "Failed to send message.\n";
    }
  }

  return 0;
}
