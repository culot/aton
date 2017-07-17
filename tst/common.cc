#include <iostream>
#include <cassert>
#include <zmq.h>

#include "common.h"

namespace common {

void clearServerMemory(void* tgt) {
  static const std::string clear("CLEAR");
  std::cout << "Clearing server memory ... ";
  zmq_send(tgt, clear.c_str(), clear.length(), 0);
  char buf[MAXREPLYSIZE];
  zmq_recv(tgt, buf, MAXREPLYSIZE, 0);
  std::string answer(buf);
  std::cout << answer << std::endl;
}

std::string send(void* tgt, const std::string& in) {
  std::cout << "Sending '" << in << "' ... ";
  zmq_send(tgt, in.c_str(), in.length(), 0);
  char buf[MAXREPLYSIZE];
  bzero(buf, MAXREPLYSIZE);
  zmq_recv(tgt, buf, MAXREPLYSIZE, 0);
  std::string answer(buf);
  std::cout << "received '" << answer << "'" << std::endl;
  return answer;
}

void sendAndReceive(void* tgt, const std::string& in, const std::string& out) {
  std::string answer = send(tgt, in);
  std::cout << "Expected '" << out << "', received '" << answer << "' ... ";
  if (out.empty()) {
    assert(answer.length() == 0);
  } else {
    assert(answer == out);
  }
  std::cout << "OK" << std::endl;
}

void sendJuncture(void* tgt) {
  static const std::string juncture("JUNCTURE");
  std::cout << "Sending juncture ... ";
  zmq_send(tgt, juncture.c_str(), juncture.length(), 0);
  char buf[MAXREPLYSIZE];
  zmq_recv(tgt, buf, MAXREPLYSIZE, 0);
  std::string answer(buf);
  std::cout << answer << std::endl;
}

}
