#include <string>
#include <iostream>
#include <unistd.h>

#include <zmq.h>

#include "common.h"


int main (void) {
  std::cout << "Connecting to server... ";
  void* context = zmq_ctx_new();
  void* requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");
  std::cout << "OK" << std::endl;

  common::sendAndReceive(requester, "tic", "");
  common::sendAndReceive(requester, "tac", "");
  common::sendAndReceive(requester, "tic", "tac");
  common::sendAndReceive(requester, "toc", "");
  common::sendAndReceive(requester, "tic", "tac|toc");

  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
