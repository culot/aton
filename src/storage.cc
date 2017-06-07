#include <exception>

#include <glog/logging.h>
#include <sqlite3.h>

#include "cfg.h"

#include "storage.h"

namespace aton {

static sqlite3* db_;

Storage::Storage() {
  int rc = sqlite3_open(cfg::DB_FILE.c_str(), &db_);
  if (rc) {
    LOG(ERROR) << __func__ << sqlite3_errmsg(db_);
    throw std::runtime_error("Could not open database");
  } else {
    LOG(INFO) << __func__ << " - Database [" << cfg::DB_FILE << "] successfully opened";
  }
}

Storage::~Storage() {
  close();
}

void Storage::load() {
  LOG(INFO) << __func__ << " - Loading data";
}

void Storage::save() {
  LOG(INFO) << __func__ << " - Saving data";
}

void Storage::close() {
  sqlite3_close(db_);
  LOG(INFO) << __func__ << " - Database [" << cfg::DB_FILE << "] successfully closed";
}

}
