#pragma once

#include <mutex>
#include <string>

#include "statemgr.h"
#include "transition.h"
#include "state.h"


namespace aton {

class Storage {
 public:
  Storage();
  ~Storage();

  void load(StateMgr& statemgr);
  void save(StateMgr& statemgr);
  void close();

 private:
  void init();
  void clearSchema();
  void createSchemaIfMissing();
  void loadStates(StateMgr& statemgr);
  void loadTransitions(StateMgr& statemgr);
  void saveStates(std::vector<StatePtr>& states);
  void saveTransitions(std::vector<TransitionPtr>& transitions);
  void executeStatement(const std::string& statement);

  std::mutex mutex_;
};

}
