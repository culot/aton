#pragma once

#include <string>
#include <chrono>

#include "statemgr.h"

namespace aton {

class Server {
 public:
  explicit Server(int port = 5555) : port_(port) {}
  void start();
  void processInput();

 private:
  std::string handleRequest(const std::string& request);
  std::string getStatus() const;
  bool isStatusRequest(const std::string& request) const;

  int port_;
  void* context_;
  void* responder_;
  StateMgr statemgr_;
  std::chrono::system_clock::time_point startTime_;
};

}
