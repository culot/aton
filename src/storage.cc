#include <exception>

#include <glog/logging.h>
#include <sqlite3.h>

#include "cfg.h"

#include "storage.h"

namespace aton {

static sqlite3* db_;

Storage::Storage() {
  init();
}

void Storage::init() {
  int rc = sqlite3_open(cfg::DB_FILE.c_str(), &db_);
  if (rc) {
    LOG(ERROR) << __func__ << sqlite3_errmsg(db_);
    throw std::runtime_error("Could not open database");
  } else {
    LOG(INFO) << __func__ << " - Database [" << cfg::DB_FILE << "] successfully opened";
  }
  createSchemaIfMissing();
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

void Storage::createSchemaIfMissing() {
  std::string createSchema =
      "PRAGMA foreign_keys = ON;"
      "CREATE TABLE IF NOT EXISTS textstate ("
      "id INTEGER PRIMARY KEY,"
      "trigger TEXT NOT NULL UNIQUE);"
      "CREATE TABLE IF NOT EXISTS transition ("
      "id INTEGER PRIMARY KEY,"
      "state_from INTEGER NOT NULL,"
      "state_to INTEGER NOT NULL,"
      "weight INTEGER NOT NULL,"
      "UNIQUE (state_from,state_to) ON CONFLICT FAIL,"
      "FOREIGN KEY (state_from) REFERENCES textstate (id),"
      "FOREIGN KEY (state_to) REFERENCES textstate (id))";

  LOG(INFO) << "Creating schema if missing from storage";
  executeStatement(createSchema);
}

void Storage::executeStatement(const std::string& statement) {
  char* errmsg = nullptr;
  int rc = sqlite3_exec(db_, statement.c_str(), nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not execute [" << statement << "] because [" << errmsg << "]";
    sqlite3_free(errmsg);
    throw std::runtime_error("Storage error: statement could not be executed");
  } else {
    LOG(INFO) << __func__ << " - statement successfully executed";
  }
}

}
