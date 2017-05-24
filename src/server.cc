#include <exception>
#include <string>

#include <glog/logging.h>
#include <zmq.h>

#include "server.h"

namespace aton {

void Server::start() {
  context_ = zmq_ctx_new();
  responder_ = zmq_socket(context_, ZMQ_REP);
  std::string address = "tcp://*:" + std::to_string(port_);
  LOG(INFO) << "Trying to start server on address [" << address << "]";
  int rc = zmq_bind(responder_, address.c_str());
  if (rc != 0) {
    throw std::runtime_error("Could not instanciate zmq server");
  }
  LOG(INFO) << "Server succesfully started";
}

}
