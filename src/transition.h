#pragma once

#include <memory>

#include "state.h"

namespace aton {

class Transition {
 public:
  Transition(const StatePtr& fromState, const StatePtr& toState, int weight = 1)
      : fromState_(fromState), toState_(toState), weight_(weight) {}

  const StatePtr from() const {return fromState_;}
  const StatePtr to() const {return toState_;}
  int weight() const {return weight_;}
  void increaseWeight() {++weight_;}

 private:
  const StatePtr fromState_;
  const StatePtr toState_;
  int weight_;
};

using TransitionPtr = std::shared_ptr<Transition>;

}
