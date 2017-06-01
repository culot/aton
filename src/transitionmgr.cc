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

TransitionPtr TransitionMgr::getTransition(const StatePtr& from, const StatePtr& to) {
  uint64_t fromStateSig = from->signature();
  // For now, just extract the first item of the multimap (use equal_range to get all items)
  auto it = transitions_.find(fromStateSig);
  if (it == transitions_.end()) {
    LOG(INFO) << __func__ << "Transition from [" << *from << "] to [" << *to
              << "] does not exist";
    return nullptr;
  } else {
    TransitionPtr transition = it->second;
    return transition;
  }
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

}
