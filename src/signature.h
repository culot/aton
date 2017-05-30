#pragma once

#include <cstdint>
#include <string>

namespace aton {

class Signature {
 public:
  explicit Signature(const std::string& input);

  uint64_t fingerprint() const {return fingerprint_;}

 private:
  uint64_t fingerprint_;
};

}
