#include <exception>

#include <glog/logging.h>

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
  out_ << "  graph [mindist=0, minlen=0, size=\"9,9\",ratio=\"auto\",orientation=portrait]" << std::endl;
  out_ << std::endl;
  out_ << "  // Set the default properties for nodes and edges between nodes" << std::endl;
  out_ << "  node [shape=ellipse, color=gray, fontname=\"fixed\", fontsize=9]" << std::endl;
  out_ << "  edge [color=gray, arrowhead=open, weight=2]" << std::endl;
  out_ << std::endl;
  out_ << "}" << std::endl;
}

}
