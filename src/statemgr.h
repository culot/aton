#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "state.h"


namespace aton {

class StateMgr {
 public:
  StatePtr registerTextState(const std::string& input);

 private:
  std::map<uint64_t, StatePtr> states_;
};

}
