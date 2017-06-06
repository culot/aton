#include <iostream>
#include <cassert>
#include <zmq.h>

#include "common.h"

namespace common {

void sendAndReceive(void* tgt, const std::string& in, const std::string& out) {
  std::cout << "Sending '" << in << "' ... ";
  zmq_send(tgt, in.c_str(), in.length(), 0);
  char buf[MAXREPLYSIZE];
  zmq_recv(tgt, buf, MAXREPLYSIZE, 0);
  std::string answer(buf);
  std::cout << "Expected '" << out << "', received '" << answer << "' ... ";
  if (out.empty()) {
    assert(answer.length() == 0);
  } else {
    assert(answer == out);
  }
  std::cout << "OK" << std::endl;
}

}
