#pragma once

#include "signature.h"
#include "statemgr.h"

#include "state.h"

namespace aton {

class TextState : public State {
 public:
  TextState(const std::string& input, const Signature& signature)
      : trigger_(input), signature_(signature) {}

  uint64_t signature() override {return signature_.fingerprint();}

 private:
  const std::string trigger_;
  const Signature signature_;
};

}
