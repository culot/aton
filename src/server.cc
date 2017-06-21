#include <exception>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <vector>

#include <glog/logging.h>
#include <zmq.h>

#include "server.h"


namespace aton {

void Server::start() {
  try {
    storage_.load(statemgr_);
  } catch (const std::exception& e) {
    LOG(ERROR) << "Could not retrieve data from storage due to: " << e.what();
    throw std::runtime_error("Could not load from storage");
  }
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

void Server::terminate() {
  storage_.save(statemgr_);
  storage_.close();
}

void Server::processInput() {
  LOG(INFO) << "Waiting for input to be processed";
  for (;;) {
    char buf[cfg::MAXINPUTSIZE];
    int nbytes = zmq_recv(responder_, buf, cfg::MAXINPUTSIZE, 0);
    std::string input(buf, nbytes);
    LOG(INFO) << "<<< Received input [" << input << "] length [" << nbytes << "]";
    std::string reply;

    auto it = inputHandlers_.find(input);
    if (it != inputHandlers_.end()) {
      reply = (it->second)(*this);
    } else {
      reply = handleRequest(input);
    }

    LOG(INFO) << ">>> Sending reply [" << reply << "] length [" << reply.length() << "]";
    zmq_send(responder_, reply.c_str(), reply.length(), 0);
  }
}

std::string Server::handleRequest(const std::string& request) {
  statemgr_.registerState(State::Type::text, request);
  std::vector<StatePtr> predictions = statemgr_.predictNextStates();
  return statemgr_.formatAsString(predictions);
}

std::string Server::getStatus() {
  std::string status("Server UP since ");
  std::stringstream ss;
  auto startTimeT = std::chrono::system_clock::to_time_t(startTime_);
  ss << std::put_time(std::localtime(&startTimeT), "%Y-%m-%d %X");
  status.append(ss.str());
  return status;
}

std::string Server::clear() {
  LOG(INFO) << "Clearing server memory";
  statemgr_.clear();
  return "Server memory cleared";
}

std::string Server::handleJuncture() {
  LOG(INFO) << "Juncture received, ending current unit";
  statemgr_.endUnit();
  return cfg::REQUEST_JUNCTURE;
}

}
