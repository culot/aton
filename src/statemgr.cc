#include <exception>
#include <memory>
#include <algorithm>

#include <glog/logging.h>

#include "signature.h"
#include "textstate.h"
#include "multistate.h"

#include "statemgr.h"

namespace aton {

void StateMgr::clear() {
  transitionmgr_.clear();
  LOG(INFO) << __func__ << " - Removing [" << states_.size() << "] states";
  states_.clear();
  currentUnit_.clear();
  currentState_ = nullptr;
}

StatePtr StateMgr::registerState(State::Type type, const std::string& input, uint64_t id) {
  StatePtr newState;
  switch (type) {
    case State::Type::text:
      LOG(INFO) << __func__ << " - Registering text state with id [" << id << "]";
      newState = registerTextState(input, id);
      break;
    default:
      LOG(ERROR) << __func__ << " - State type not yet supported";
      throw std::runtime_error("State type not yet supported");
  }
  transitionmgr_.registerTransition(currentState_, newState);
  currentState_ = newState;
  currentUnit_.push_back(currentState_);
  return currentState_;
}

// This method is used when loading data from the transition database table.
// The states of the transition are represented by the uint64_t identifiers,
// so we must retrieve the corresponding states before actually creating the
// transition. This implies a dependency when loading from the database:
// states must necessarily be loaded before transitions.
TransitionPtr StateMgr::registerTransition(uint64_t from, uint64_t to, int weight) {
  StatePtr stateFrom = getStateWithId(from);
  StatePtr stateTo = getStateWithId(to);
  LOG(INFO) << __func__ << " - Registering transition from ["
            << *stateFrom << "] to [" << *stateTo << "] with weight [" << weight << "]";
  return transitionmgr_.registerTransition(stateFrom, stateTo, weight);
}

StatePtr StateMgr::registerTextState(const std::string& input, uint64_t id) {
  Signature signature(input);
  auto it = states_.find(signature.fingerprint());
  StatePtr state;
  if (it == states_.end()) {
    LOG(INFO) << __func__ << " - Creating new state with id [" << id << "] and input ["
              << input << "] and fingerprint [" << signature.fingerprint() << "]";
    state = std::make_shared<TextState>(id, input, signature);
    states_[signature.fingerprint()] = state;
  } else {
    state = it->second;
  }
  return state;
}

StatePtr StateMgr::buildMultiState(const std::vector<StatePtr>& states, uint64_t id) const {
  std::vector<std::string> traits;
  for (const auto state : states) {
    traits.push_back(state->str());
  }
  Signature signature(traits);
  return std::make_shared<MultiState>(id, states, signature);
}

StatePtr StateMgr::registerMultiState(const std::vector<StatePtr>& states, uint64_t id) {
  StatePtr state = buildMultiState(states, id);
  auto it = states_.find(state->signature());
  if (it == states_.end()) {
    LOG(INFO) << __func__ << " - Registering new state with id [" << id
              << "] and fingerprint [" << state->signature() << "]";
    states_[state->signature()] = state;
  }
  return state;
}

std::vector<StatePtr> StateMgr::getAllStates() const {
  std::vector<StatePtr> states;
  for (const auto i : states_) {
    states.push_back(i.second);
  }
  return states;
}

StatePtr StateMgr::getStateWithId(uint64_t id) const {
  auto it = std::find_if(states_.begin(),
                         states_.end(),
                         [=](const std::pair<uint64_t, StatePtr>& s) {return s.second->id() == id;});
  if (it != states_.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

void StateMgr::endUnit() {
  currentState_ = currentUnit_.back();
  currentUnit_.pop_back();
  StatePtr multiState = registerMultiState(currentUnit_);
  transitionmgr_.registerTransition(multiState, currentState_);
  currentUnit_.clear();
}

// To predict next possible state, first look for a
// possible transition from the current unit, and if
// nothing is found, then look for a possible transition
// from the current state. This means that as an analogy
// with spoken language, priority is given to the word
// (ie unit) over the current letter (ie current state).
std::vector<StatePtr> StateMgr::predictNextStates() const {
  StatePtr currentUnitState = buildMultiState(currentUnit_);
  std::vector<StatePtr> states = transitionmgr_.predictAllStatesFrom(currentUnitState);
  if (states.empty()) {
    states = transitionmgr_.predictAllStatesFrom(currentState_);
  }
  return states;
}

std::string StateMgr::formatAsString(const std::vector<StatePtr>& states) const {
  static const std::string separator("|");
  std::string str;
  for (const auto state : states) {
    str.append(state->str());
    str.append(separator);
  }
  // Remove extraneous last separator
  str.pop_back();
  return str;
}

}
