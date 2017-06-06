#pragma once

#include <map>

#include "state.h"
#include "transition.h"

namespace aton {

class TransitionMgr {
 public:
  TransitionPtr registerTransition(const StatePtr& from, const StatePtr& to);
  TransitionPtr getTransitionFrom(const StatePtr& from) const;
  void clear();

 private:
  bool isValidTransition(const StatePtr& from, const StatePtr& to) const;
  TransitionPtr createTransition(const StatePtr& from, const StatePtr& to);
  TransitionPtr getTransition(const StatePtr& from, const StatePtr& to) const;
  void increaseTransitionWeight(TransitionPtr& transition) const;

  std::multimap<uint64_t, TransitionPtr> transitions_;
};

}
