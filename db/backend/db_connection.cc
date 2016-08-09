#include "db/backend/db_connection.h"

namespace db {

DBConnection::DBConnection(const ConnectionInfo& info)
  : once_called_(0),
    recyclable_(0) {
  int cache_size = info.Get("@stmt_cache_size", 64);
  if (cache_size > 0) {
    cache_.SetSize(cache_size);
  }
  //TODO
  std::string def_is_prep = info.Get("@use_prepared","off");

  if(def_is_prep == "on")
    default_is_prepared_ = 1;
  else if(def_is_prep == "off")
    default_is_prepared_ = 0;
  else {
    DCHECK(false) << "db:backend:connection: @use_prepared should be either 'on' or 'off'";
  }
}

//TODO
DBConnection::~DBConnection() {
  ClearCache();
}

scoped_ref_ptr<DBStatement> 
DBConnection::GetDirectStatement(const std::string& query) {
  scoped_ref_ptr<DBStatement> result(NewDirectStatement(query));
  return result;
}

bool DBConnection::once_called() const {
  return once_called_;
}

void DBConnection::set_once_called(bool v) {
  once_called_ = v;
}

bool DBConnection::recyclable() const {
  return recyclable_;
}

void DBConnection::set_recyclable(bool v) {
  recyclable_ = v;
}

void DBConnection::ClearCache() {
  cache_.Clear();
}

} // namespace db
