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
  if (!stateFrom || !stateTo) {
    LOG(ERROR) << __func__ << " - Invalid state id, could not register transition";
    throw std::runtime_error("Unknown state when registering transition");
  }
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

StatePtr StateMgr::registerMultiState(const std::vector<uint64_t>& stateIds, uint64_t id) {
  std::vector<StatePtr> states;
  for (const auto stateId : stateIds) {
    states.push_back(getStateWithId(stateId));
  }
  return registerMultiState(states, id);
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
  if (currentUnit_.size() > 1) {
    StatePtr multiState = registerMultiState(currentUnit_);
    transitionmgr_.registerTransition(multiState, currentState_);
  }
  currentUnit_.clear();
  currentState_ = nullptr;
}

std::vector<StateMgr::Prediction> StateMgr::predictNextStates() const {
  StatePtr currentState;

  // To predict next possible state, first look for a
  // possible transition from the current unit, and if
  // nothing is found, then look for a possible transition
  // from the current state. This means that as an analogy
  // with spoken language, priority is given to the word
  // (ie unit) over the current letter (ie current state).
  StatePtr currentUnitState = buildMultiState(currentUnit_);
  if (transitionmgr_.hasTransitions(currentUnitState)) {
    currentState = currentUnitState;
  } else {
    currentState = currentState_;
  }

  std::vector<TransitionPtr> transitions = transitionmgr_.getAllTransitionsFrom(currentState);
  uint64_t transno = transitionmgr_.getNumberOfTransitionsFrom(currentState);

  std::vector<StateMgr::Prediction> predictions;
  for (const auto& transition : transitions) {
    StateMgr::Prediction prediction;
    prediction.state = transition->to();
    prediction.probability = static_cast<float>(transition->weight()) / static_cast<float>(transno);
    predictions.push_back(prediction);
  }

  return predictions;
}

std::string StateMgr::formatAsString(const std::vector<StateMgr::Prediction>& predictions) const {
  static const std::string separator("|");
  static const std::string probabilitySeparator(":");
  std::string str;
  for (const auto prediction : predictions) {
    str.append(prediction.state->str());
    str.append(probabilitySeparator);
    str.append(std::to_string(prediction.probability));
    str.append(separator);
  }
  // Remove extraneous last separator
  str.pop_back();
  return str;
}

}
