#pragma once

#include <mutex>
#include <string>

#include "state.h"
#include "transition.h"

namespace aton {

class Storage {
 public:
  static Storage& instance() {static Storage instance_; return instance_;}

  void load();
  void save();
  void close();

 private:
  Storage();
  ~Storage();
  Storage(const Storage&) = delete;
  void operator=(const Storage&) = delete;

  void init();
  void createSchemaIfMissing();
  void saveStates(const std::vector<StatePtr>& states);
  void saveTransitions(const std::vector<TransitionPtr>& transitions);
  void executeStatement(const std::string& statement);

  std::mutex mutex_;
};

}
