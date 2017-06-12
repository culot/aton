#include <exception>
#include <memory>
#include <algorithm>

#include <glog/logging.h>

#include "signature.h"
#include "textstate.h"

#include "statemgr.h"

namespace aton {

void StateMgr::clear() {
  LOG(INFO) << __func__ << " - Removing [" << states_.size() << "] states";
  states_.clear();
  currentState_ = nullptr;
}

StatePtr StateMgr::registerState(State::Type type, const std::string& input, uint64_t id) {
  switch (type) {
    case State::Type::text:
      LOG(INFO) << __func__ << " - Registering text state with id [" << id << "]";
      return registerTextState(input, id);
      break;
    default:
      LOG(ERROR) << __func__ << " - State type not yet supported";
      throw std::runtime_error("State type not yet supported");
  }
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
  currentState_ = state;
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

}
