#include <string>
#include <iostream>
#include <unistd.h>
#include <cassert>

#include <zmq.h>

static const int MAXREPLYSIZE = 1024;

int main (void) {
  std::cout << "Connecting to server... ";
  void* context = zmq_ctx_new();
  void* requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");
  std::cout << "OK" << std::endl;

  {
    std::cout << "Sending 'tic' ... ";
    zmq_send(requester, "tic", 3, 0);
    char buf[MAXREPLYSIZE];
    zmq_recv(requester, buf, MAXREPLYSIZE, 0);
    std::string answer(buf);
    std::cout << "Received '" << answer << "' ... ";
    assert(answer.length() == 0);
    std::cout << "OK" << std::endl;
  }
  {
    std::cout << "Sending 'tac' ... ";
    zmq_send(requester, "tac", 3, 0);
    char buf[MAXREPLYSIZE];
    zmq_recv(requester, buf, MAXREPLYSIZE, 0);
    std::string answer(buf);
    std::cout << "Received '" << answer << "' ... ";
    assert(answer.length() == 0);
    std::cout << "OK" << std::endl;
  }
  {
    std::cout << "Sending 'tic' again ... ";
    zmq_send(requester, "tic", 3, 0);
    char buf[MAXREPLYSIZE];
    zmq_recv(requester, buf, MAXREPLYSIZE, 0);
    std::string answer(buf);
    std::cout << "Received '" << answer << "' ... ";
    assert(answer == "tac");
    std::cout << "OK" << std::endl;
  }

  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
