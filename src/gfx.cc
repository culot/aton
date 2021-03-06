#include <exception>
#include <vector>
#include <cmath>
#include <sstream>

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

/*
 * The Graphviz plot uses the NFM-Indus Script font,
 * which can be downloaded for free from the following url:
 *
 *  https://www.mohenjodaroonline.net/index.php/indus-script/corpus-by-asko-parpola/42-indus-script
 */
void Gfx::plotGraphviz(const StateMgr& statemgr) {
  LOG(INFO) << __func__ << " - Starting Graphviz plot generation";

  out_ << "digraph \"aton\" {" << std::endl;
  out_ << std::endl;
  out_ << "  // Set general graph options" << std::endl;
  out_ << "  graph [mindist=0, minlen=0, ratio=\"auto\"]" << std::endl;
  out_ << std::endl;
  out_ << "  // Set the default properties for nodes and edges between nodes" << std::endl;
  out_ << "  node [shape=ellipse, color=gray, fontname=\"NFM-Indus Script\", fontsize=48, fixedsize=true, width=1.125, height=1.125]" << std::endl;
  out_ << "  edge [color=gray, style=tapered, dir=forward, arrowhead=none]" << std::endl;
  out_ << std::endl;

  std::vector<StatePtr> states = statemgr.getAllStates();
  for (const auto& state : states) {
    if (state->type() == State::Type::multi && !options_.plotMultiState) {
      continue;
    }

    // The state->str() string contains an hexadecinal value,
    // representing the corresponding character (UTF8 code)
    // from the NFM-Indus font. Graphviz knows how to draw
    // UTF8 fonts, as long as it receives the equivalent
    // decimal value, hence the following conversion.
    //
    // Note: The NFM Indus font belongs to UTF8's Basic
    // Multilingual Plane of the Private Use Area range,
    // and starts with U+E.
    int utf8dec;
    std::istringstream("E" + state->str()) >> std::hex >> utf8dec;

    // The 'x' in front of the node name is here to prevent
    // a bug (?) in Grpahviz which seems to dislike node names
    // that do not start with a letter.
    out_ << "  x" << state->str() << " [label=\"&#" << utf8dec << ";\"]" << std::endl;
  }
  out_ << std::endl;

  for (const auto& state : states) {
    if (state->type() == State::Type::multi && !options_.plotMultiState) {
      continue;
    }

    std::vector<StateMgr::Prediction> predictions = statemgr.predictNextStates(state);
    for (const auto& prediction : predictions) {
      out_ << "  x" << state->str() << " -> x" << prediction.state->str();
      out_ << " [penwidth=" << std::lround(10 * prediction.probability) << "]" << std::endl;
    }
  }

  out_ << std::endl;
  out_ << "}" << std::endl;
}

}
