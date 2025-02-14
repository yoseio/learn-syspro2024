#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

class TCPClient {
 public:
  TCPClient() : sock_(-1) {}
  ~TCPClient() { closeSocket(); }

  bool connectToServer(const std::string& serverIP, int serverPort) {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
      perror("socket");
      return false;
    }

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

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
    if (sock_ < 0)
      return -1;
    ssize_t sent = write(sock_, data.c_str(), data.size());
    if (sent < 0) {
      perror("write");
    }
    return sent;
  }

  ssize_t receiveData(std::string& out, size_t maxSize = 4096) {
    if (sock_ < 0)
      return -1;
    std::vector<char> buf(maxSize + 1, 0);
    ssize_t received = read(sock_, buf.data(), maxSize);
    if (received > 0) {
      buf[received] = '\0';
      out.assign(buf.data());
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

class MessageMonitor {
 public:
  MessageMonitor(const std::string& serverIP, int serverPort)
      : serverIP_(serverIP), serverPort_(serverPort) {}

  bool checkMessages(const std::string& myName,
                     const std::string& senderFilter,
                     std::time_t t1,
                     std::time_t t2) {
    TCPClient client;
    if (!client.connectToServer(serverIP_, serverPort_)) {
      return false;
    }

    std::ostringstream oss;
    oss << "1iSAUYKj get-from-time " << myName << " " << senderFilter << " "
        << t1 << " " << t2 << "\n";
    std::string cmd = oss.str();

    if (client.sendData(cmd) < 0) {
      client.closeSocket();
      return false;
    }

    std::string response;
    if (client.receiveData(response, 4096) > 0) {
      std::cout << "[Server response from " << t1 << " to " << t2 << "]:\n";
      std::cout << response << std::endl;
    } else {
      std::cerr << "No response or error.\n";
    }

    client.closeSocket();
    return true;
  }

 private:
  std::string serverIP_;
  int serverPort_;
};

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <server_ip> <port> <myName> <senderFilter>\n";
    return 1;
  }

  std::string serverIP = argv[1];
  int serverPort = std::atoi(argv[2]);
  std::string myName = argv[3];
  std::string senderFilter = argv[4];

  MessageMonitor monitor(serverIP, serverPort);

  std::time_t lastCheckTime = std::time(nullptr);

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::time_t now = std::time(nullptr);

    monitor.checkMessages(myName, senderFilter, lastCheckTime, now);

    lastCheckTime = now;
  }

  return 0;
}
