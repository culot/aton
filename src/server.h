#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <map>
#include <functional>

#include "cfg.h"
#include "storage.h"
#include "statemgr.h"
#include "state.h"


namespace aton {

class Server {
 public:
  static Server& instance() {static Server instance_; return instance_;}

  void start();
  void processInput();
  void terminate();

 private:
  Server() {}
  ~Server() {}
  Server(const Server&) = delete;
  void operator=(const Server&) = delete;

  // All member functions responsible for handling generic
  // requests should have the following signature:
  using InputHandler = std::function<std::string(Server&)>;

  std::string getStatus();
  std::string clear();
  std::string handleJuncture();

  std::string handleRequest(const std::string& request);

  int port_ {cfg::SERVER_PORT};
  void* context_;
  void* responder_;
  Storage storage_;
  StateMgr statemgr_;
  std::chrono::system_clock::time_point startTime_;

  const std::map<std::string, InputHandler> inputHandlers_ {
    {cfg::REQUEST_STATUS, &Server::getStatus},
    {cfg::REQUEST_CLEAR, &Server::clear},
    {cfg::REQUEST_JUNCTURE, &Server::handleJuncture}
  };
};

}
