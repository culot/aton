#include <exception>

#include <glog/logging.h>
#include <sqlite3.h>

#include "cfg.h"
#include "server.h"

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

  std::vector<StatePtr> states = Server::instance().getAllStates();
  LOG(INFO) << __func__ << " - Saving [" << states.size() << "] states";
  saveStates(states);

  std::vector<TransitionPtr> transitions = Server::instance().getAllTransitions();
  LOG(INFO) << __func__ << " - Saving [" << transitions.size() << "] transitions";
  saveTransitions(transitions);

  LOG(INFO) << __func__ << " - Data successfully saved";
}

void Storage::close() {
  sqlite3_close(db_);
  LOG(INFO) << __func__ << " - Database [" << cfg::DB_FILE << "] successfully closed";
}

void Storage::createSchemaIfMissing() {
  std::string createSchema =
      "PRAGMA foreign_keys = ON;"
      //
      // state
      //
      "CREATE TABLE IF NOT EXISTS state ("
      "id INTEGER PRIMARY KEY,"
      "type INTEGER NOT NULL);"
      //
      // textstate
      //
      "CREATE TABLE IF NOT EXISTS textstate ("
      "id INTEGER NOT NULL UNIQUE,"
      "trigger TEXT NOT NULL UNIQUE,"
      "FOREIGN KEY (id) REFERENCES state (id));"
      //
      // transition
      //
      "CREATE TABLE IF NOT EXISTS transition ("
      "id INTEGER PRIMARY KEY,"
      "state_from INTEGER NOT NULL,"
      "state_to INTEGER NOT NULL,"
      "weight INTEGER NOT NULL,"
      "UNIQUE (state_from,state_to) ON CONFLICT FAIL,"
      "FOREIGN KEY (state_from) REFERENCES textstate (id),"
      "FOREIGN KEY (state_to) REFERENCES textstate (id));";

  LOG(INFO) << "Creating schema if missing from storage";
  executeStatement(createSchema);
}

void Storage::saveStates(const std::vector<StatePtr>& states) {
  sqlite3_stmt* stmtState;
  std::string insertState("INSERT INTO state (type) VALUES (?)");

  sqlite3_stmt* stmtTextstate;
  std::string insertTextstate("INSERT INTO textstate (id,trigger) VALUES (?,?)");


  if ((sqlite3_prepare_v2(db_, insertState.c_str(), -1, &stmtState, 0) != SQLITE_OK)
      || (sqlite3_prepare_v2(db_, insertTextstate.c_str(), -1, &stmtTextstate, 0) != SQLITE_OK)) {
    LOG(ERROR) << __func__ << " - Could not prepare statements because ["
               << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: could not prepare statements");
  }

  for (const auto state : states) {
    sqlite3_bind_int(stmtState, 1, state->type());
    if (sqlite3_step(stmtState) != SQLITE_DONE) {
      LOG(ERROR) << __func__ << " - Error while inserting State [" << state->str()
                 << "] because [" << sqlite3_errmsg(db_) << "]";
      throw std::runtime_error("Storage error: could not save states");
    }

    sqlite3_bind_int(stmtTextstate, 1, sqlite3_last_insert_rowid(db_));
    sqlite3_bind_text(stmtTextstate, 2, state->str().c_str(), state->str().size(), SQLITE_STATIC);
    if (sqlite3_step(stmtTextstate) != SQLITE_DONE) {
      LOG(ERROR) << __func__ << " - Error while inserting State [" << state->str()
                 << "] because [" << sqlite3_errmsg(db_) << "]";
      throw std::runtime_error("Storage error: could not save states");
    }

    sqlite3_reset(stmtState);
    sqlite3_reset(stmtTextstate);
  }
  sqlite3_finalize(stmtState);
  sqlite3_finalize(stmtTextstate);
}

void Storage::saveTransitions(const std::vector<TransitionPtr>& transitions) {

}

void Storage::executeStatement(const std::string& statement) {
  char* errmsg = nullptr;
  if (sqlite3_exec(db_, statement.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not execute [" << statement << "] because [" << errmsg << "]";
    sqlite3_free(errmsg);
    throw std::runtime_error("Storage error: statement could not be executed");
  } else {
    LOG(INFO) << __func__ << " - statement successfully executed";
  }
}

}
