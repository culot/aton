#include <glog/logging.h>

#include "signature.h"

namespace aton {

Signature::Signature(const std::string& input) {
  fingerprint_ = std::hash<std::string>{}(input);
  LOG(INFO) << "Fingerprint [" << fingerprint_ << "] computed for input [" << input << "]";
}

}
