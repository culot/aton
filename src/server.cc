#include <exception>
#include <sstream>
#include <ctime>
#include <iomanip>

#include <glog/logging.h>
#include <zmq.h>

#include "cfg.h"

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
    LOG(INFO) << "Received input [" << input << "] length [" << nbytes << "]";
    std::string reply;
    if (isStatusRequest(input)) {
      reply = getStatus();
    } else {
      reply = handleRequest(buf);
    }
    zmq_send(responder_, reply.c_str(), reply.length(), 0);
  }
}

std::string Server::handleRequest(const std::string& request) const {
  static const std::string notYet("NOT YET IMPLEMENTED");
  return notYet;
}

std::string Server::getStatus() const {
  std::string status("Server UP since ");
  std::stringstream ss;
  auto startTimeT = std::chrono::system_clock::to_time_t(startTime_);
  ss << std::put_time(std::localtime(&startTimeT), "%Y-%m-%d %X");
  status.append(ss.str());
  return status;
}

bool Server::isStatusRequest(const std::string& request) const {
  return request == cfg::REQUEST_STATUS;
}

}
