#pragma once

#include <mutex>
#include <string>

namespace aton {

class Storage {
 public:
  static Storage& instance() {static Storage instance_; return instance_;}

  void load();
  void save();
  void close();

 private:
  Storage();
  ~Storage();
  Storage(const Storage&) = delete;
  void operator=(const Storage&) = delete;

  void init();
  void createSchemaIfMissing();
  void executeStatement(const std::string& statement);

  std::mutex mutex_;
};

}
