#pragma once

#include <map>

#include "state.h"
#include "transition.h"

namespace aton {

class TransitionMgr {
 public:
  TransitionPtr registerTransition(const StatePtr& from, const StatePtr& to);

 private:
  std::map<uint64_t, TransitionPtr> transitions_;
};

}
