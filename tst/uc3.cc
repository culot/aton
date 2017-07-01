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

  common::clearServerMemory(requester);

  common::sendAndReceive(requester, "b", "");
  common::sendAndReceive(requester, "a", "");
  common::sendAndReceive(requester, "l", "");
  common::sendJuncture(requester);
  common::sendAndReceive(requester, "t", "");
  common::sendAndReceive(requester, "a", "l");
  common::sendAndReceive(requester, "c", "");
  common::sendJuncture(requester);
  common::sendAndReceive(requester, "b", "a");
  common::sendAndReceive(requester, "a", "l");
  common::sendJuncture(requester);
  common::sendAndReceive(requester, "t", "a");
  common::sendAndReceive(requester, "a", "c");

  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
