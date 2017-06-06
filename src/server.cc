#include <exception>
#include <sstream>
#include <ctime>
#include <iomanip>

#include <glog/logging.h>
#include <zmq.h>

#include "cfg.h"
#include "statemgr.h"
#include "state.h"

#include "server.h"

namespace aton {

void Server::start() {
  context_ = zmq_ctx_new();
  responder_ = zmq_socket(context_, ZMQ_REP);
  std::string address = "tcp://*:" + std::to_string(port_);
  LOG(INFO) << "Trying to start server on address [" << address << "]";
  int rc = zmq_bind(responder_, address.c_str());
  if (rc != 0) {
    throw std::runtime_error("Could not instantiate zmq server");
  }
  startTime_ = std::chrono::system_clock::now();
  LOG(INFO) << "Server succesfully started";
}

void Server::processInput() {
  LOG(INFO) << "Waiting for input to be processed";
  for (;;) {
    char buf[cfg::MAXINPUTSIZE];
    int nbytes = zmq_recv(responder_, buf, cfg::MAXINPUTSIZE, 0);
    std::string input(buf, nbytes);
    LOG(INFO) << "<<< Received input [" << input << "] length [" << nbytes << "]";
    std::string reply;
    if (isStatusRequest(input)) {
      reply = getStatus();
    } else if (isClearRequest(input)) {
      reply = clear();
    } else {
      reply = handleRequest(input);
    }
    LOG(INFO) << ">>> Sending reply [" << reply << "] length [" << reply.length() << "]";
    zmq_send(responder_, reply.c_str(), reply.length(), 0);
  }
}

std::string Server::handleRequest(const std::string& request) {
  StatePtr currentState = statemgr_.currentTextState();
  StatePtr newState = statemgr_.registerTextState(request);
  transitionmgr_.registerTransition(currentState, newState);

  // Now for the answer, look for the most probable transition
  // starting with newState, and if it exists then return its
  // final state.
  TransitionPtr transition = transitionmgr_.getTransitionFrom(newState);
  if (transition != nullptr) {
    StatePtr mostProbableNextState = transition->to();
    return mostProbableNextState->str();
  } else {
    return "";
  }
}

std::string Server::getStatus() const {
  std::string status("Server UP since ");
  std::stringstream ss;
  auto startTimeT = std::chrono::system_clock::to_time_t(startTime_);
  ss << std::put_time(std::localtime(&startTimeT), "%Y-%m-%d %X");
  status.append(ss.str());
  return status;
}

std::string Server::clear() {
  LOG(INFO) << "Clearing server memory";
  transitionmgr_.clear();
  statemgr_.clear();
  return "Server memory cleared";
}

bool Server::isStatusRequest(const std::string& request) const {
  return request == cfg::REQUEST_STATUS;
}

bool Server::isClearRequest(const std::string& request) const {
  return request == cfg::REQUEST_CLEAR;
}

}
