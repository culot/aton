#include "cfg.h"

namespace aton {
namespace cfg {

const int SERVER_PORT = 5555;
const int MAXINPUTSIZE = 1024;

const std::string REQUEST_STATUS("STATUS");
const std::string REQUEST_CLEAR("CLEAR");
const std::string REQUEST_JUNCTURE("JUNCTURE");
const std::string DB_FILE("/var/tmp/aton.db");
const std::string PLOT_FILE_GRAPHVIZ("/var/tmp/aton-graphviz.dot");
const std::string PLOT_FILE_GRAPHVIZ_PICS("/var/tmp/aton-graphviz-pics.dot");

}
}
