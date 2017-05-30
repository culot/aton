#pragma once

#include <cstdint>
#include <memory>

namespace aton {

class State {
 public:
  virtual ~State() {}
  virtual uint64_t signature() = 0;
};

using StatePtr = std::shared_ptr<State>;

}
