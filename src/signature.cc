#include <glog/logging.h>

#include "signature.h"

namespace aton {

Signature::Signature(const std::string& input) {
  fingerprint_ = std::hash<std::string>{}(input);
  LOG(INFO) << __func__ << " - Fingerprint [" << fingerprint_
            << "] computed for input [" << input << "]";
}

Signature::Signature(const std::vector<std::string>& inputs) {
  std::for_each(inputs.begin(),
                inputs.end(),
                [&](const std::string& input) {hashCombine<std::string>(fingerprint_, input);});
  LOG(INFO) << __func__ << " - Fingerprint [" << fingerprint_ << "] computed";
}

}
