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
  int size() const {return states_.size();}
  StatePtr registerState(State::Type type, const std::string& input, uint64_t id = 0);
  TransitionPtr registerTransition(uint64_t from, uint64_t to, int weight);
  std::vector<StatePtr> getAllStates() const;
  std::vector<TransitionPtr> getAllTransitions() const {return transitionmgr_.getAllTransitions();}
  void endUnit();
  std::vector<StatePtr> predictNextStates() const;
  std::string formatAsString(const std::vector<StatePtr>& states) const;

 private:
  StatePtr getStateWithId(uint64_t id) const;
  StatePtr registerTextState(const std::string& input, uint64_t id = 0);
  StatePtr registerMultiState(const std::vector<StatePtr>& states, uint64_t id = 0);
  StatePtr buildMultiState(const std::vector<StatePtr>& states, uint64_t id = 0) const;

  TransitionMgr transitionmgr_;
  StatePtr currentState_ {nullptr};
  std::vector<StatePtr> currentUnit_;
  std::map<uint64_t, StatePtr> states_;
};

}
