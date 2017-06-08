#pragma once

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

namespace aton {

class State {
 public:
  enum Type {
    unknown,
    text
  };

  virtual ~State() {}
  virtual std::string str() const = 0;
  virtual uint64_t signature() const = 0;
  int type() const {return type_;}

  virtual bool operator==(const State& rhs) const = 0;

  friend std::ostream& operator<<(std::ostream& out, const State& s) {
    s.print(out);
    return out;
  }

 protected:
  explicit State(Type type) : type_(type) {}

 private:
  virtual void print(std::ostream& out) const = 0;

  const Type type_;
};

using StatePtr = std::shared_ptr<State>;

}
