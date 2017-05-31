#pragma once

#include <memory>

#include "state.h"

namespace aton {

class Transition {
 public:
  Transition(const StatePtr& fromState, const StatePtr& toState)
      : fromState_(fromState), toState_(toState), weight_(1) {}

  int weight() const {return weight_;}
  void increaseWeight() {++weight_;}

 private:
  const StatePtr fromState_;
  const StatePtr toState_;
  int weight_;
};

using TransitionPtr = std::shared_ptr<Transition>;

}
