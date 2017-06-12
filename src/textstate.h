#pragma once

#include <ostream>
#include <string>
#include <cstdint>

#include "signature.h"
#include "statemgr.h"

#include "state.h"

namespace aton {

class TextState : public State {
 public:
  TextState(uint64_t id, const std::string& input, const Signature& signature)
      : State(State::Type::text, id), trigger_(input), signature_(signature) {}

  std::string str() const override {return trigger_;}
  uint64_t signature() const override {return signature_.fingerprint();}

  bool operator==(const State& rhs) const override {
    return signature() == rhs.signature();
  }

 private:
  void print(std::ostream& out) const override {
    out << str();
  }

  const std::string trigger_;
  const Signature signature_;
};

}
