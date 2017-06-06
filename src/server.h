#pragma once

#include <string>
#include <chrono>

#include "statemgr.h"
#include "transitionmgr.h"

namespace aton {

class Server {
 public:
  explicit Server(int port = 5555) : port_(port) {}
  void start();
  void processInput();

 private:
  std::string handleRequest(const std::string& request);
  std::string formatReply(const std::vector<TransitionPtr>& transitions) const;
  std::string getStatus() const;
  bool isStatusRequest(const std::string& request) const;
  bool isClearRequest(const std::string& request) const;
  std::string clear();

  int port_;
  void* context_;
  void* responder_;
  StateMgr statemgr_;
  TransitionMgr transitionmgr_;
  std::chrono::system_clock::time_point startTime_;
};

}
