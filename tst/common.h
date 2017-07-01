#pragma once

#include <string>

namespace common {

static const int MAXREPLYSIZE = 1024;

void clearServerMemory(void* tgt);
void sendAndReceive(void* tgt, const std::string& in, const std::string& out);
void sendJuncture(void* tgt);

}
