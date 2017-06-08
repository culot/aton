#include <string>
#include <iostream>
#include <unistd.h>
#include <cassert>
#include <csignal>

#include <glog/logging.h>

#include "storage.h"
#include "server.h"

using namespace aton;

void sigHandler(int signal) {
  LOG(INFO) << "Received signal [" << signal << "]";
  Storage::instance().save();
  Storage::instance().close();
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);

  std::signal(SIGINT, sigHandler);

  try {
    Storage::instance().load();
  } catch (const std::exception& e) {
    LOG(ERROR) << "Aborting due to following error: " << e.what();
    return 1;
  }

  try {
    Server::instance().start();
  } catch (const std::exception& e) {
    LOG(ERROR) << "Aborting due to following error: " << e.what();
    return 1;
  }

  Server::instance().processInput();

  return 0;
}
