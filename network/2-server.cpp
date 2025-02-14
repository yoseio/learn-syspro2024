#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Message {
  std::string sender;
  std::string receiver;
  std::string content;
  std::time_t timestamp;
};

class MessageStore {
 private:
  std::vector<Message> messages_;

 public:
  void addMessage(const std::string& sender,
                  const std::string& receiver,
                  const std::string& content,
                  std::time_t timestamp) {
    Message m;
    m.sender = sender;
    m.receiver = receiver;
    m.content = content;
    m.timestamp = timestamp;
    messages_.push_back(m);
  }

  std::vector<Message> getMessages(const std::string& receiver,
                                   const std::string& sender) {
    std::vector<Message> result;
    for (auto& m : messages_) {
      bool matchReceiver = (m.receiver == receiver);
      bool matchSender = (sender == "all" || m.sender == sender);
      if (matchReceiver && matchSender) {
        result.push_back(m);
      }
    }
    return result;
  }

  std::vector<Message> getMessagesByTimeRange(const std::string& receiver,
                                              const std::string& sender,
                                              std::time_t t1,
                                              std::time_t t2) {
    std::vector<Message> result;
    for (auto& m : messages_) {
      bool matchReceiver = (m.receiver == receiver);
      bool matchSender = (sender == "all" || m.sender == sender);
      bool inRange = (m.timestamp >= t1 && m.timestamp <= t2);
      if (matchReceiver && matchSender && inRange) {
        result.push_back(m);
      }
    }
    return result;
  }

  const Message* getLatestMessage(const std::string& receiver,
                                  const std::string& sender) {
    std::time_t latestTime = 0;
    const Message* latestMsg = nullptr;
    for (auto& m : messages_) {
      bool matchReceiver = (m.receiver == receiver);
      bool matchSender = (sender == "all" || m.sender == sender);
      if (matchReceiver && matchSender) {
        if (m.timestamp > latestTime) {
          latestTime = m.timestamp;
          latestMsg = &m;
        }
      }
    }
    return latestMsg;
  }
};

class CommandHandler {
 private:
  MessageStore& store_;

 public:
  explicit CommandHandler(MessageStore& store) : store_(store) {}

  std::string handleCommand(const std::string& input) {
    std::vector<std::string> tokens = parseMessage(input);

    if (tokens.size() < 2 || tokens[0] != "1iSAUYKj") {
      return "ERROR: Invalid signature.\n";
    }

    std::string command = tokens[1];

    if (command == "send-to") {
      return handleSendTo(tokens);
    } else if (command == "get-from") {
      return handleGetFrom(tokens);
    } else if (command == "get-from-time") {
      return handleGetFromTime(tokens);
    } else {
      return "ERROR: Unknown command.\n";
    }
  }

 private:
  std::string handleSendTo(const std::vector<std::string>& tokens) {
    if (tokens.size() < 5) {
      return "ERROR: Not enough arguments for send-to.\n";
    }
    std::string sender = tokens[2];
    std::string receiver = tokens[3];
    std::string content = tokens[4];
    std::time_t now = std::time(nullptr);

    store_.addMessage(sender, receiver, content, now);
    return "OK: Message stored.\n";
  }

  std::string handleGetFrom(const std::vector<std::string>& tokens) {
    if (tokens.size() < 4) {
      return "ERROR: Not enough arguments for get-from.\n";
    }
    std::string receiver = tokens[2];
    std::string sender = tokens[3];

    std::vector<Message> results = store_.getMessages(receiver, sender);
    if (results.empty()) {
      return "(no messages)\n";
    }

    std::ostringstream oss;
    for (auto& m : results) {
      oss << "[" << m.sender << " @ " << m.timestamp << "]> " << m.content
          << "\n";
    }
    return oss.str();
  }

  std::string handleGetFromTime(const std::vector<std::string>& tokens) {
    if (tokens.size() < 6) {
      return "ERROR: Not enough arguments for get-from-time.\n";
    }
    std::string receiver = tokens[2];
    std::string sender = tokens[3];
    std::time_t t1 = std::atol(tokens[4].c_str());
    std::time_t t2 = std::atol(tokens[5].c_str());

    if (t2 < t1) {
      const Message* latest = store_.getLatestMessage(receiver, sender);
      if (latest) {
        std::ostringstream oss;
        oss << "[Latest: " << latest->timestamp << " from " << latest->sender
            << "]> " << latest->content << "\n";
        return oss.str();
      } else {
        return "(no messages)\n";
      }
    }

    std::vector<Message> results =
        store_.getMessagesByTimeRange(receiver, sender, t1, t2);
    if (results.empty()) {
      return "(no messages in time range)\n";
    }
    std::ostringstream oss;
    for (auto& m : results) {
      oss << "[" << m.sender << " @ " << m.timestamp << "]> " << m.content
          << "\n";
    }
    return oss.str();
  }

 public:
  static std::vector<std::string> parseMessage(const std::string& input) {
    std::vector<std::string> tokens;
    bool inQuotes = false;
    std::ostringstream currentToken;

    for (size_t i = 0; i < input.size(); ++i) {
      char c = input[i];
      if (c == '"') {
        inQuotes = !inQuotes;
      } else if (std::isspace(static_cast<unsigned char>(c)) && !inQuotes) {
        if (!currentToken.str().empty()) {
          tokens.push_back(currentToken.str());
          currentToken.str("");
          currentToken.clear();
        }
      } else {
        currentToken << c;
      }
    }
    if (!currentToken.str().empty()) {
      tokens.push_back(currentToken.str());
    }

    return tokens;
  }
};

class TCPServer {
 private:
  int port_;
  int serverSock_;
  bool running_;
  MessageStore& store_;
  CommandHandler commandHandler_;

 public:
  TCPServer(int port, MessageStore& store)
      : port_(port),
        serverSock_(-1),
        running_(true),
        store_(store),
        commandHandler_(store) {}

  bool start() {
    serverSock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock_ < 0) {
      perror("socket");
      return false;
    }

    int opt = 1;
    setsockopt(serverSock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      perror("bind");
      close(serverSock_);
      return false;
    }

    if (listen(serverSock_, 5) < 0) {
      perror("listen");
      close(serverSock_);
      return false;
    }

    std::cout << "Server listening on port " << port_ << "..." << std::endl;
    return true;
  }

  void run() {
    if (serverSock_ < 0) {
      std::cerr << "Server socket is not initialized.\n";
      return;
    }

    while (running_) {
      struct sockaddr_in clientAddr;
      socklen_t clientLen = sizeof(clientAddr);
      int clientSock =
          accept(serverSock_, (struct sockaddr*)&clientAddr, &clientLen);
      if (clientSock < 0) {
        perror("accept");
        continue;
      }

      char buffer[4096];
      std::memset(buffer, 0, sizeof(buffer));
      ssize_t received = read(clientSock, buffer, sizeof(buffer) - 1);
      if (received <= 0) {
        close(clientSock);
        continue;
      }

      std::string recvStr(buffer);
      std::string response = commandHandler_.handleCommand(recvStr);

      write(clientSock, response.c_str(), response.size());

      close(clientSock);
    }
  }

  void stop() {
    running_ = false;
    if (serverSock_ >= 0) {
      close(serverSock_);
    }
  }
};

int main(int argc, char* argv[]) {
  int port = 12345;
  if (argc >= 2) {
    port = std::atoi(argv[1]);
  }

  MessageStore store;

  TCPServer server(port, store);

  if (!server.start()) {
    return 1;
  }

  server.run();

  return 0;
}
