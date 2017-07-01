#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace aton {

class Signature {
 public:
  explicit Signature(const std::string& input);
  explicit Signature(const std::vector<std::string>& inputs);

  uint64_t fingerprint() const {return fingerprint_;}

 private:
  // Inspired by boost::hash_combine
  template <class T>
  inline void hashCombine(uint64_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  uint64_t fingerprint_ {0};
};

}
