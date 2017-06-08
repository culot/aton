#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <string>

#include "state.h"


namespace aton {

class StateMgr {
 public:
  void clear();
  StatePtr currentTextState() const {return currentState_;}
  StatePtr registerTextState(const std::string& input);
  std::vector<StatePtr> getAllStates() const;

 private:
  StatePtr currentState_ {nullptr};
  std::map<uint64_t, StatePtr> states_;
};

}
