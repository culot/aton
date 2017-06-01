#pragma once

#include <map>

#include "state.h"
#include "transition.h"

namespace aton {

class TransitionMgr {
 public:
  TransitionPtr registerTransition(const StatePtr& from, const StatePtr& to);

 private:
  bool isValidTransition(const StatePtr& from, const StatePtr& to) const;
  TransitionPtr getTransition(const StatePtr& from, const StatePtr& to);
  TransitionPtr createTransition(const StatePtr& from, const StatePtr& to);
  void increaseTransitionWeight(TransitionPtr& transition) const;

  std::multimap<uint64_t, TransitionPtr> transitions_;
};

}
