#include <exception>
#include <string>

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
  LOG(INFO) << "Server succesfully started";
}

void Server::processInput() {
  LOG(INFO) << "Waiting for input to be processed";
  for (;;) {
    char buf[cfg::MAXINPUTSIZE];
    int nbytes = zmq_recv(responder_, buf, cfg::MAXINPUTSIZE, 0);
    if (nbytes == -1) {
      LOG(ERROR) << "Internal error when reading message - Skipping...";
      continue;
    } else if (nbytes > cfg::MAXINPUTSIZE) {
      LOG(ERROR) << "Input exceeds maximum length [" << cfg::MAXINPUTSIZE << "] - Skipping...";
      continue;
    } else {
      LOG(INFO) << "Received input [" << buf << "]";
    }
  }
}

}
