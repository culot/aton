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
  StatePtr registerState(State::Type type, const std::string& input, uint64_t id = 0);
  std::vector<StatePtr> getAllStates() const;
  StatePtr getStateWithId(uint64_t id) const;

 private:
  StatePtr registerTextState(const std::string& input, uint64_t id = 0);

  StatePtr currentState_ {nullptr};
  using states_c = std::map<uint64_t, StatePtr>;
  states_c states_;
};

}
