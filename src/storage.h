#pragma once

#include <string>

#include "statemgr.h"


namespace aton {

class Storage {
 public:
  Storage();
  ~Storage();

  void load(StateMgr& statemgr);
  void save(StateMgr& statemgr);
  void close();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  Storage(const Storage&) = delete;
  void operator=(const Storage&) = delete;
};

}
