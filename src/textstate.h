#pragma once

#include <string>
#include <cstdint>

#include "signature.h"
#include "state.h"

namespace aton {

class TextState : public State {
 public:
  TextState(uint64_t id, const std::string& input, const Signature& signature)
      : State(State::Type::text, id), trigger_(input), signature_(signature) {}

  std::string str() const override {return trigger_;}
  uint64_t signature() const override {return signature_.fingerprint();}

 private:
  const Signature signature_;
  const std::string trigger_;
};

}
