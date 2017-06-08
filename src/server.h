#pragma once

#include <string>
#include <chrono>

#include "cfg.h"
#include "statemgr.h"
#include "transitionmgr.h"

namespace aton {

class Server {
 public:
  static Server& instance() {static Server instance_; return instance_;}

  void start();
  void processInput();

 private:
  Server() {}
  ~Server() {}
  Server(const Server&) = delete;
  void operator=(const Server&) = delete;

  std::string handleRequest(const std::string& request);
  std::string formatReply(const std::vector<TransitionPtr>& transitions) const;
  std::string getStatus() const;
  bool isStatusRequest(const std::string& request) const;
  bool isClearRequest(const std::string& request) const;
  std::string clear();

  int port_ {cfg::SERVER_PORT};
  void* context_;
  void* responder_;
  StateMgr statemgr_;
  TransitionMgr transitionmgr_;
  std::chrono::system_clock::time_point startTime_;
};

}
