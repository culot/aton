#include <memory>

#include <glog/logging.h>

#include "transitionmgr.h"

namespace aton {

TransitionPtr TransitionMgr::registerTransition(const StatePtr& from, const StatePtr& to) {
  if (!isValidTransition(from, to)) {
    LOG(INFO) << __func__ << " - No transition to be created";
    return nullptr;
  } else {
    TransitionPtr transition = getTransition(from, to);
    if (transition == nullptr) {
      LOG(INFO) << __func__ << " - Transition must be created";
      return createTransition(from, to);
    } else {
      LOG(INFO) << __func__ << " - Transition already exists";
      increaseTransitionWeight(transition);
      return transition;
    }
  }
}

TransitionPtr TransitionMgr::createTransition(const StatePtr& from, const StatePtr& to) {
  LOG(INFO) << __func__ << " - Creating new transition from ["
            << *from << "] to [" << *to << "]";
  TransitionPtr transition = std::make_shared<Transition>(from, to);
  transitions_.emplace(from->signature(), transition);
  return transition;
}

TransitionPtr TransitionMgr::getMostProbableTransitionFrom(const StatePtr& from) const {
  uint64_t fromStateSig = from->signature();
  auto it = transitions_.find(fromStateSig);
  if (it == transitions_.end()) {
    LOG(INFO) << __func__ << " - No existing transition from [" << *from << "]";
    return nullptr;
  } else {
    TransitionPtr transition = it->second;
    LOG(INFO) << __func__ << " - Transition found from [" << *from << "] to [" << *transition->to() << "]";
    return transition;
  }
}

std::vector<TransitionPtr> TransitionMgr::getAllTransitionsFrom(const StatePtr& from) const {
  uint64_t fromStateSig = from->signature();
  auto range = transitions_.equal_range(fromStateSig);
  std::vector<TransitionPtr> transitions;
  for (auto it = range.first; it != range.second; ++it) {
    TransitionPtr transition = it->second;
    LOG(INFO) << __func__ << " - Transition found from [" << *from << "] to [" << *transition->to() << "]";
    transitions.push_back(transition);
  }
  LOG(INFO) << __func__ << " - Found [" << transitions.size() << "] transitions from [" << *from << "]";
  return transitions;
}

std::vector<TransitionPtr> TransitionMgr::getAllTransitions() const {
  std::vector<TransitionPtr> transitions;
  for (const auto i : transitions_) {
    transitions.push_back(i.second);
  }
  return transitions;
}

TransitionPtr TransitionMgr::getTransition(const StatePtr& from, const StatePtr& to) const {
  std::vector<TransitionPtr> transitions = getAllTransitionsFrom(from);
  for (const auto transition : transitions) {
    if (*transition->to() == *to) {
      LOG(INFO) << __func__ << "- Transition found from [" << *from << "] to [" << *to << "]";
      return transition;
    }
  }
  LOG(INFO) << __func__ << "- No exisiting transition from [" << *from << "] to [" << *to << "]";
  return nullptr;
}

void TransitionMgr::increaseTransitionWeight(TransitionPtr& transition) const {
  transition->increaseWeight();
  LOG(INFO) << __func__ << " - Weigth increased (now [" << transition->weight()
            << "]) for transition from [" << *transition->from()
            << "] to [" << *transition->to() << "]";
}

bool TransitionMgr::isValidTransition(const StatePtr& from, const StatePtr& to) const {
  if (from == nullptr || to == nullptr) {
    LOG(INFO) << "Transition misses either FROM or TO state";
    return false;
  }
  return true;
}

void TransitionMgr::clear() {
  LOG(INFO) << __func__ << " - Removing [" << transitions_.size() << "] transitions";
  transitions_.clear();
}

}
