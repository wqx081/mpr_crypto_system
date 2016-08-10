#include "db/backend/db_connection.h"

namespace db {

DBConnection::DBConnection(const ConnectionInfo& info)
  : connection_pool_(nullptr),
    once_called_(0),
    recyclable_(0) {
  int cache_size = info.Get("@stmt_cache_size", 64);
  if (cache_size > 0) {
    cache_.SetSize(cache_size);
  }

  std::string def_is_prep = info.Get("@use_prepared","on");
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
}

// static
void DBConnection::Dispose(DBConnection* self) {
  if (!self) {
    return;
  }
  scoped_ref_ptr<ConnectionPool> pool = self->connection_pool_;
  self->connection_pool_ = nullptr;
  if (pool && self->recyclable()) {
    pool->Put(self);
  } else {
    self->ClearCache();
    delete self;
    //TODO
    // dirver
  }
}

scoped_ref_ptr<ConnectionPool>
DBConnection::connection_pool() {
  return connection_pool_;
}

void DBConnection::set_connection_pool(scoped_ref_ptr<ConnectionPool> pool) {
  connection_pool_ = pool;
}

scoped_ref_ptr<DBStatement>
DBConnection::Prepare(const std::string& query) {
  if (default_is_prepared_) {
    return GetPreparedStatement(query);
  } else {
    return GetDirectStatement(query);
  }
}

scoped_ref_ptr<DBStatement> 
DBConnection::GetDirectStatement(const std::string& query) {
  scoped_ref_ptr<DBStatement> result(NewDirectStatement(query));
  return result;
}

scoped_ref_ptr<DBStatement>
DBConnection::GetPreparedStatement(const std::string& query) {
  scoped_ref_ptr<DBStatement> result;
  if (!cache_.IsActive()) {
    result = NewPreparedStatement(query);
    return result;
  }
  result = cache_.Fetch(query);
  if (!result) {
    result = NewPreparedStatement(query);
  }
  result->Cache(&cache_);
  return result;
}

scoped_ref_ptr<DBStatement>
DBConnection::GetPreparedUncahcedStatement(const std::string& query) {
  return NewPreparedStatement(query);
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
