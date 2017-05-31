#pragma once

#include <cstdint>
#include <memory>
#include <ostream>

namespace aton {

class State {
 public:
  virtual ~State() {}
  virtual uint64_t signature() = 0;

  friend std::ostream& operator<<(std::ostream& out, const State& s) {
    s.print(out);
    return out;
  }

 private:
  virtual void print(std::ostream& out) const = 0;
};

using StatePtr = std::shared_ptr<State>;

}
