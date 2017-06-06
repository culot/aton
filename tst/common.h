#pragma once

#include <string>

namespace common {

static const int MAXREPLYSIZE = 1024;

void sendAndReceive(void* tgt, const std::string& in, const std::string& out);

}
