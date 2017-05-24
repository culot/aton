#include <string>
#include <iostream>
#include <unistd.h>
#include <cassert>

#include <glog/logging.h>

#include "server.h"

using namespace aton;

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);

  Server srv;
  try {
    srv.start();
  } catch (const std::exception& e) {
    LOG(ERROR) << "Aborting due to following error: " << e.what();
  }

  return 0;
}
