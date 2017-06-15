#pragma once

#include <string>
#include <cstdint>
#include <algorithm>

#include "signature.h"
#include "state.h"

namespace aton {

class MultiState : public State {
 public:
  MultiState(uint64_t id, const std::vector<StatePtr>& states, const Signature& signature)
      : State(State::Type::multi, id), states_(states), signature_(signature) {}

  std::string str() const override {
    std::string str;
    std::for_each(states_.begin(),
                  states_.end(),
                  [&](const StatePtr& state){str.append(state->str());});
    return str;
  }
  uint64_t signature() const override {return signature_.fingerprint();}

 private:
  const Signature signature_;
  const std::vector<StatePtr> states_;
};

}
