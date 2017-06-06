#include <string>
#include <iostream>
#include <unistd.h>
#include <cassert>

#include <zmq.h>

#include "common.h"


static const int MAXREPLYSIZE = 1024;

int main (void) {
  std::cout << "Connecting to server..." << std::endl;
  void* context = zmq_ctx_new();
  void* requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");

  common::clearServerMemory(requester);

  {
    std::cout << "Sending STATUS query... ";
    zmq_send(requester, "STATUS", 6, 0);
    char buf[MAXREPLYSIZE];
    zmq_recv(requester, buf, MAXREPLYSIZE, 0);
    std::size_t up = std::string(buf).find("UP");
    assert(up != std::string::npos);
    std::cout << "OK" << std::endl;
  }

  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
