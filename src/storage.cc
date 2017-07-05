#include <exception>
#include <cstdint>
#include <vector>
#include <map>

#include <glog/logging.h>
#include <sqlite3.h>

#include "cfg.h"
#include "state.h"
#include "multistate.h"

#include "storage.h"


namespace aton {

struct Storage::Impl {
  void init();
  void close();
  void clearSchema();
  void createSchemaIfMissing();
  void loadStates(StateMgr& statemgr);
  void loadTextStates(StateMgr& statemgr);
  void loadMultiStates(StateMgr& statemgr);
  void loadTransitions(StateMgr& statemgr);
  void saveTextstates(const std::vector<StatePtr>& states);
  void saveMultistates(const std::vector<StatePtr>& states);
  void saveStates(const std::vector<StatePtr>& states);
  void saveTransitions(std::vector<TransitionPtr>& transitions);
  void executeStatement(const std::string& statement);

  sqlite3* db_;
};

Storage::Storage() : impl_{new Impl} {
  impl_->init();
}

Storage::~Storage() {
  impl_->close();
}

void Storage::load(StateMgr& statemgr) {
  LOG(INFO) << __func__ << " - Loading data";

  LOG(INFO) << __func__ << " - Loading states";
  impl_->loadStates(statemgr);

  LOG(INFO) << __func__ << " - Loading transitions";
  impl_->loadTransitions(statemgr);
}

void Storage::save(StateMgr& statemgr) {
  LOG(INFO) << __func__ << " - Saving data";

  impl_->clearSchema();

  std::vector<StatePtr> states = statemgr.getAllStates();
  LOG(INFO) << __func__ << " - Saving [" << states.size() << "] states";
  impl_->saveStates(states);

  std::vector<TransitionPtr> transitions = statemgr.getAllTransitions();
  LOG(INFO) << __func__ << " - Saving [" << transitions.size() << "] transitions";
  impl_->saveTransitions(transitions);

  LOG(INFO) << __func__ << " - Data successfully saved";
}

void Storage::close() {
  impl_->close();
}

void Storage::Impl::init() {
  int rc = sqlite3_open(cfg::DB_FILE.c_str(), &db_);
  if (rc) {
    LOG(ERROR) << __func__ << sqlite3_errmsg(db_);
    throw std::runtime_error("Could not open database");
  } else {
    LOG(INFO) << __func__ << " - Database [" << cfg::DB_FILE << "] successfully opened";
  }
  createSchemaIfMissing();
}

void Storage::Impl::loadStates(StateMgr& statemgr) {
  loadTextStates(statemgr);
  loadMultiStates(statemgr);
  LOG(INFO) << __func__ << " - Successfully loaded [" << statemgr.size() << "] states";
}

void Storage::Impl::loadTextStates(StateMgr& statemgr) {
  sqlite3_stmt* stmtTextstate;
  std::string selectTextstate("SELECT id,trigger FROM textstate");
  if (sqlite3_prepare_v2(db_, selectTextstate.c_str(), -1, &stmtTextstate, nullptr) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not prepare [" << selectTextstate
               << "] because [" << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: statement could not prepare statements");
  }
  int rc;
  while ((rc = sqlite3_step(stmtTextstate)) == SQLITE_ROW) {
    uint64_t id = sqlite3_column_int(stmtTextstate, 0);
    // The retrieved data is plain ASCII, so the cast is safe here
    std::string trigger = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmtTextstate, 1)));
    statemgr.registerState(State::Type::text, trigger, id);
  }
  if (rc != SQLITE_DONE) {
    LOG(ERROR) << __func__ << " - Could not load states because [" << sqlite3_errmsg(db_) << "]";
    sqlite3_finalize(stmtTextstate);
    throw std::runtime_error("Storage error: could not load states");
  }
  sqlite3_finalize(stmtTextstate);
}

void Storage::Impl::loadMultiStates(StateMgr& statemgr) {
  sqlite3_stmt* stmtMultistate;
  std::string selectMultistate("SELECT id,state FROM multistate order by id");
  if (sqlite3_prepare_v2(db_, selectMultistate.c_str(), -1, &stmtMultistate, nullptr) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not prepare [" << selectMultistate
               << "] because [" << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: statement could not prepare statements");
  }
  int rc;
  std::map<uint64_t, std::vector<uint64_t>> states;
  while ((rc = sqlite3_step(stmtMultistate)) == SQLITE_ROW) {
    uint64_t id = sqlite3_column_int(stmtMultistate, 0);
    uint64_t stateId = sqlite3_column_int(stmtMultistate, 1);
    states[id].push_back(stateId);
  }
  if (rc != SQLITE_DONE) {
    LOG(ERROR) << __func__ << " - Could not load states because [" << sqlite3_errmsg(db_) << "]";
    sqlite3_finalize(stmtMultistate);
    throw std::runtime_error("Storage error: could not load states");
  }
  sqlite3_finalize(stmtMultistate);

  for (const auto& kv : states) {
    statemgr.registerMultiState(kv.second, kv.first);
  }
}

void Storage::Impl::loadTransitions(StateMgr& statemgr) {
  sqlite3_stmt* stmtTransition;
  std::string selectTransition("SELECT state_from,state_to,weight FROM transition");
  if (sqlite3_prepare_v2(db_, selectTransition.c_str(), -1, &stmtTransition, nullptr) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not prepare [" << selectTransition
               << "] because [" << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: statement could not prepare statements");
  }
  int rc;
  while ((rc = sqlite3_step(stmtTransition)) == SQLITE_ROW) {
    uint64_t idFrom = sqlite3_column_int(stmtTransition, 0);
    uint64_t idTo = sqlite3_column_int(stmtTransition, 1);
    int weight = sqlite3_column_int(stmtTransition, 2);
    statemgr.registerTransition(idFrom, idTo, weight);
  }
  if (rc != SQLITE_DONE) {
    LOG(ERROR) << __func__ << " - Could not load transitions because [" << sqlite3_errmsg(db_) << "]";
    sqlite3_finalize(stmtTransition);
    throw std::runtime_error("Storage error: could not load transitions");
  }
  sqlite3_finalize(stmtTransition);
}

void Storage::Impl::close() {
  sqlite3_close(db_);
  LOG(INFO) << __func__ << " - Database [" << cfg::DB_FILE << "] successfully closed";
}

void Storage::Impl::clearSchema() {
  std::string clearSchema(
      "DELETE from transition;"
      "DELETE from textstate;"
      "DELETE from multistate;"
      "DELETE from state;");
  executeStatement(clearSchema);
  LOG(INFO) << "Schema was cleared";
}

void Storage::Impl::createSchemaIfMissing() {
  std::string createSchema(
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
      // multistate
      //
      "CREATE TABLE IF NOT EXISTS multistate ("
      "id INTEGER NOT NULL,"
      "state INTEGER NOT NULL,"
      "FOREIGN KEY (id) REFERENCES state (id),"
      "FOREIGN KEY (state) REFERENCES state (id));"
      //
      // transition
      //
      "CREATE TABLE IF NOT EXISTS transition ("
      "id INTEGER PRIMARY KEY,"
      "state_from INTEGER NOT NULL,"
      "state_to INTEGER NOT NULL,"
      "weight INTEGER NOT NULL,"
      "UNIQUE (state_from,state_to) ON CONFLICT FAIL,"
      "FOREIGN KEY (state_from) REFERENCES state (id),"
      "FOREIGN KEY (state_to) REFERENCES state (id));");

  LOG(INFO) << "Creating schema if missing from storage";
  executeStatement(createSchema);
}

void Storage::Impl::saveTextstates(const std::vector<StatePtr>& states) {
  sqlite3_stmt* stmtTextstate;
  std::string insertTextstate("INSERT INTO textstate (id,trigger) VALUES (?,?)");
  auto doInsertTextstate = [&](const StatePtr& state) {
    sqlite3_bind_int(stmtTextstate, 1, state->id());
    sqlite3_bind_text(stmtTextstate, 2, state->str().c_str(), state->str().size(), SQLITE_STATIC);
    if (sqlite3_step(stmtTextstate) != SQLITE_DONE) {
      LOG(ERROR) << __func__ << " - Error while inserting State [" << state->str()
                 << "] because [" << sqlite3_errmsg(db_) << "]";
      throw std::runtime_error("Storage error: could not save states");
    }
    sqlite3_reset(stmtTextstate);
  };

  if (sqlite3_prepare_v2(db_, insertTextstate.c_str(), -1, &stmtTextstate, 0) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not prepare statements because ["
               << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: could not prepare statements");
  }

  for (const auto& state : states) {
    if (state->type() == State::Type::text) {
      doInsertTextstate(state);
    }
  }
  sqlite3_finalize(stmtTextstate);
}

void Storage::Impl::saveMultistates(const std::vector<StatePtr>& states) {
  sqlite3_stmt* stmtMultistate;
  std::string insertMultistate("INSERT INTO multistate (id,state) VALUES (?,?)");
  auto doInsertMultistate = [&](const StatePtr& state) {
    const MultiState& multistate = dynamic_cast<MultiState&>(*state);
    const std::vector<StatePtr> states = multistate.components();
    for (const auto& state : states) {
      sqlite3_bind_int(stmtMultistate, 1, multistate.id());
      sqlite3_bind_int(stmtMultistate, 2, state->id());
      if (sqlite3_step(stmtMultistate) != SQLITE_DONE) {
        LOG(ERROR) << __func__ << " - Error while inserting State [" << multistate.str()
                   << "] because [" << sqlite3_errmsg(db_) << "]";
        throw std::runtime_error("Storage error: could not save states");
      }
      sqlite3_reset(stmtMultistate);
    }
  };

  if (sqlite3_prepare_v2(db_, insertMultistate.c_str(), -1, &stmtMultistate, 0) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not prepare statements because ["
               << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: could not prepare statements");
  }

  for (const auto& state : states) {
    if (state->type() == State::Type::multi) {
      doInsertMultistate(state);
    }
  }
  sqlite3_finalize(stmtMultistate);
}

// Tables are saved in this order:
//   1. state
//   2. textstate
//   3. multistate
//
// This is due to the presence of multistates which are made of
// several textstates, hence states of type text must ALL be
// persisted before states of type multi.
//
// Note: we loop 3 times over all states, which is not optimal,
// but persistence only happens at server shutdown when performance
// is not a concern.
void Storage::Impl::saveStates(const std::vector<StatePtr>& states) {
  sqlite3_stmt* stmtState;
  std::string insertState("INSERT INTO state (type) VALUES (?)");
  auto doInsertState = [&](const StatePtr& state) {
    sqlite3_bind_int(stmtState, 1, state->type());
    if (sqlite3_step(stmtState) != SQLITE_DONE) {
      LOG(ERROR) << __func__ << " - Error while inserting State [" << state->str()
                 << "] because [" << sqlite3_errmsg(db_) << "]";
      throw std::runtime_error("Storage error: could not save states");
    }
    sqlite3_reset(stmtState);
  };

  if (sqlite3_prepare_v2(db_, insertState.c_str(), -1, &stmtState, 0) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not prepare statements because ["
               << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: could not prepare statements");
  }

  for (const auto& state : states) {
    doInsertState(state);
    state->setId(sqlite3_last_insert_rowid(db_));
  }
  sqlite3_finalize(stmtState);

  saveTextstates(states);
  saveMultistates(states);
}

void Storage::Impl::saveTransitions(std::vector<TransitionPtr>& transitions) {
  sqlite3_stmt* stmtTransition;
  std::string insertTransition("INSERT INTO transition (state_from,state_to,weight) VALUES (?,?,?)");

  if (sqlite3_prepare_v2(db_, insertTransition.c_str(), -1, &stmtTransition, 0) != SQLITE_OK) {
    LOG(ERROR) << __func__ << " - Could not prepare statements because ["
               << sqlite3_errmsg(db_) << "]";
    throw std::runtime_error("Storage error: could not prepare statements");
  }

  for (const auto& transition : transitions) {
    sqlite3_bind_int(stmtTransition, 1, transition->from()->id());
    sqlite3_bind_int(stmtTransition, 2, transition->to()->id());
    sqlite3_bind_int(stmtTransition, 3, transition->weight());
    if (sqlite3_step(stmtTransition) != SQLITE_DONE) {
      LOG(ERROR) << __func__ << " - Error while inserting transition because ["
                 << sqlite3_errmsg(db_) << "]";
      throw std::runtime_error("Storage error: could not save transitions");
    }
    sqlite3_reset(stmtTransition);
  }
  sqlite3_finalize(stmtTransition);
}

void Storage::Impl::executeStatement(const std::string& statement) {
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
