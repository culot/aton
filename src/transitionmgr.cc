#include <memory>

#include <glog/logging.h>

#include "transitionmgr.h"

namespace aton {

TransitionPtr TransitionMgr::registerTransition(const StatePtr& from, const StatePtr& to) {
  uint64_t fromStateSig = from->signature();
  auto it = transitions_.find(fromStateSig);
  TransitionPtr transition;
  if (it == transitions_.end()) {
    LOG(INFO) << __func__ << " - Creating new transition from ["
              << *from << "] to [" << *to << "]";
    transition = std::make_shared<Transition>(from, to);
    transitions_[fromStateSig] = transition;
  } else {
    transition = it->second;
    transition->increaseWeight();
    LOG(INFO) << __func__ << " - Weigth increased (now [" << transition->weight()
              << "]) for transition from [" << *from << "] to [" << *to << "]";
  }
  return transition;
}

}
