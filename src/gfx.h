#pragma once

#include <string>
#include <fstream>

#include "statemgr.h"


namespace aton {

class Gfx {
 public:
  enum Type {
    graphviz
  };

  Gfx(Type type, const std::string& outputFileName);
  void plot(const StateMgr& statemgr);

 private:
  const Type type_;
  const std::string ofname_;
  std::ofstream out_;
};

}
