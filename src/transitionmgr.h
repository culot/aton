#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "state.h"
#include "transition.h"


namespace aton {

class TransitionMgr {
 public:
  TransitionPtr registerTransition(const StatePtr& from, const StatePtr& to, int weight = 1);
  bool hasTransitions(const StatePtr& from) const;
  std::vector<TransitionPtr> getAllTransitionsFrom(const StatePtr& from) const;
  uint64_t getNumberOfTransitionsFrom(const StatePtr& from) const;
  std::vector<TransitionPtr> getAllTransitions() const;
  void clear();

 private:
  bool isValidTransition(const StatePtr& from, const StatePtr& to) const;
  TransitionPtr createTransition(const StatePtr& from, const StatePtr& to, int weight);
  TransitionPtr getTransition(const StatePtr& from, const StatePtr& to) const;
  void increaseTransitionWeight(TransitionPtr& transition) const;

  std::multimap<uint64_t, TransitionPtr> transitions_;
};

}
