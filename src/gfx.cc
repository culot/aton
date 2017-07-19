#include <exception>
#include <vector>
#include <cmath>

#include <glog/logging.h>

#include "state.h"
#include "transition.h"

#include "gfx.h"


namespace aton {

Gfx::Gfx(Type type, const std::string& outputFileName)
    : type_(type), ofname_(outputFileName), out_(ofname_, std::ios::out) {
  if (!out_.is_open()) {
    LOG(ERROR) << __func__ << " - Could not open [" << ofname_ << "]";
    throw std::runtime_error("Could not initialize Gfx class");
  } else {
    LOG(INFO) << __func__ << " - File [" << ofname_ << "] opened to store plot";
  }
}

void Gfx::plot(const StateMgr& statemgr) {
  auto it = plotHandlers_.find(type_);
  if (it != plotHandlers_.end()) {
    (it->second)(*this, statemgr);
  } else {
    LOG(ERROR) << __func__ << " - Unsupported plot format [" << type_ << "]";
  }
}

void Gfx::plotGraphviz(const StateMgr& statemgr) {
  LOG(INFO) << __func__ << " - Starting Graphviz plot generation";

  out_ << "digraph \"aton\" {" << std::endl;
  out_ << std::endl;
  out_ << "  // Set general graph options" << std::endl;

  if (options_.plotWithPics) {
    out_ << "  graph [mindist=0, minlen=0, ratio=\"auto\", imagepath=\"full/path/to/images\"]" << std::endl;
  } else {
    out_ << "  graph [mindist=0, minlen=0, ratio=\"auto\", imagepath=\"full/path/to/images\"]" << std::endl;
  }

  out_ << std::endl;
  out_ << "  // Set the default properties for nodes and edges between nodes" << std::endl;

  if (options_.plotWithPics) {
    out_ << "  node [shape=ellipse, color=gray, fontname=\"fixed\", fontsize=9, label=\"\", fixedsize=true, width=1.125, height=1.125]" << std::endl;
  } else {
    out_ << "  node [shape=ellipse, color=gray, fontname=\"fixed\", fontsize=9, fixedsize=true, width=1.125, height=1.125]" << std::endl;
  }

  out_ << "  edge [color=gray, style=tapered, dir=forward, arrowhead=none]" << std::endl;
  out_ << std::endl;

  std::vector<StatePtr> states = statemgr.getAllStates();
  for (const auto& state : states) {
    if (state->type() == State::Type::multi && !options_.plotMultiState) {
      continue;
    }

    if (options_.plotWithPics) {
      out_ << "  " << state->str()
           << " [image=\"S"  << state->str() << ".jpg\"]" << std::endl;
    } else {
      out_ << "  " << state->str()
           <<" [label=\"" << state->str() << "\"]" << std::endl;
    }
  }
  out_ << std::endl;

  for (const auto& state : states) {
    if (state->type() == State::Type::multi && !options_.plotMultiState) {
      continue;
    }

    std::vector<StateMgr::Prediction> predictions = statemgr.predictNextStates(state);
    for (const auto& prediction : predictions) {
      out_ << "  " << state->str() << " -> " << prediction.state->str();
      out_ << " [penwidth=" << std::lround(10 * prediction.probability) << "]" << std::endl;
    }
  }

  out_ << std::endl;
  out_ << "}" << std::endl;
}

}
