#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "state.h"


namespace aton {

class StateMgr {
 public:
  StatePtr currentTextState() const {return currentState_;}
  StatePtr registerTextState(const std::string& input);

 private:
  StatePtr currentState_ {nullptr};
  std::map<uint64_t, StatePtr> states_;
};

}
