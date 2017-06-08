#include <memory>

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

StatePtr StateMgr::registerTextState(const std::string& input) {
  Signature signature(input);
  auto it = states_.find(signature.fingerprint());
  StatePtr state;
  if (it == states_.end()) {
    LOG(INFO) << __func__ << " - Creating new state with input ["
              << input << "] and fingerprint [" << signature.fingerprint() << "]";
    state = std::make_shared<TextState>(input, signature);
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

}
