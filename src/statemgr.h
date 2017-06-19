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
  StatePtr registerState(State::Type type, const std::string& input, uint64_t id = 0);
  StatePtr getLastState() const {return lastState_;}
  std::vector<StatePtr> getAllStates() const;
  StatePtr getStateWithId(uint64_t id) const;
  void endUnit();

 private:
  StatePtr registerTextState(const std::string& input, uint64_t id = 0);
  StatePtr registerMultiState(const std::vector<StatePtr>& states, uint64_t id = 0);

  StatePtr lastState_ {nullptr};
  std::vector<StatePtr> currentUnit_;
  std::map<uint64_t, StatePtr> states_;
};

}
