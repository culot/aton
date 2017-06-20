#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <string>

#include "transitionmgr.h"
#include "transition.h"
#include "state.h"


namespace aton {

class StateMgr {
 public:
  void clear();
  StatePtr registerState(State::Type type, const std::string& input, uint64_t id = 0);
  TransitionPtr registerTransition(uint64_t from, uint64_t to, int weight);
  std::vector<StatePtr> getAllStates() const;
  std::vector<TransitionPtr> getAllTransitions() const {return transitionmgr_.getAllTransitions();}
  std::string getAllTransitionsAsStringFrom(const StatePtr& state) const {return transitionmgr_.getAllTransitionsAsStringFrom(state);}
  void endUnit();

 private:
  StatePtr getStateWithId(uint64_t id) const;
  StatePtr registerTextState(const std::string& input, uint64_t id = 0);
  StatePtr registerMultiState(const std::vector<StatePtr>& states, uint64_t id = 0);

  TransitionMgr transitionmgr_;
  StatePtr currentState_ {nullptr};
  std::vector<StatePtr> currentUnit_;
  std::map<uint64_t, StatePtr> states_;
};

}
