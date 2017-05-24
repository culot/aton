#pragma once

namespace aton {

class Server {
 public:
  explicit Server(int port = 5555) : port_(port) {}
  void start();
  void processInput();

 private:
  int port_;
  void* context_;
  void* responder_;
};

}
