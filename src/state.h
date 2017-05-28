#pragma once

#include <cstdint>

namespace aton {

template <typename T>
class State {
 public:
  explicit State(const T& trigger);
  uint64_t fingerprint() const {return fingerprint_;}

 private:
  T trigger_;
  uint64_t fingerprint_;
};

}
