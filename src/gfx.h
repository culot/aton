#pragma once

#include <functional>
#include <string>
#include <fstream>
#include <map>

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

  using PlotHandler = std::function<void(Gfx&, const StateMgr&)>;
  void plotGraphviz(const StateMgr& statemgr);

  const Type type_;
  const std::string ofname_;
  std::ofstream out_;

  const std::map<Gfx::Type, PlotHandler> plotHandlers_ {
    {Gfx::Type::graphviz, &Gfx::plotGraphviz}
  };
};

}
