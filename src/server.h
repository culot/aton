#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <map>
#include <functional>

#include "cfg.h"
#include "statemgr.h"
#include "state.h"
#include "transitionmgr.h"
#include "transition.h"

namespace aton {

class Server {
 public:
  static Server& instance() {static Server instance_; return instance_;}

  void start();
  void processInput();
  std::vector<StatePtr> getAllStates() const {return statemgr_.getAllStates();}
  std::vector<TransitionPtr> getAllTransitions() const {return transitionmgr_.getAllTransitions();}
  StatePtr registerState(State::Type type, const std::string& trigger, uint64_t id = 0) {return statemgr_.registerState(type, trigger, id);}
  TransitionPtr registerTransition(uint64_t from, uint64_t to, int weight);

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
  std::string formatReply(const std::vector<TransitionPtr>& transitions) const;

  int port_ {cfg::SERVER_PORT};
  void* context_;
  void* responder_;
  StateMgr statemgr_;
  TransitionMgr transitionmgr_;
  std::chrono::system_clock::time_point startTime_;

  const std::map<std::string, InputHandler> inputHandlers_ {
    {cfg::REQUEST_STATUS, &Server::getStatus},
    {cfg::REQUEST_CLEAR, &Server::clear},
    {cfg::REQUEST_JUNCTURE, &Server::handleJuncture}
  };
};

}
