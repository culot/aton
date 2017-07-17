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

  struct Options {
    bool plotMultiState {false};
  };

  Gfx(Type type, const std::string& outputFileName);
  void setOptions(const Options& options) {options_ = options;}
  void plot(const StateMgr& statemgr);

 private:

  using PlotHandler = std::function<void(Gfx&, const StateMgr&)>;
  void plotGraphviz(const StateMgr& statemgr);

  const Type type_;
  const std::string ofname_;
  std::ofstream out_;
  Options options_;

  const std::map<Gfx::Type, PlotHandler> plotHandlers_ {
    {Gfx::Type::graphviz, &Gfx::plotGraphviz}
  };
};

}
