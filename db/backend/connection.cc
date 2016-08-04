#include "db/simple_ref_counted.h"

#include "db/backend/statement.h"
#include "db/backend/connection.h"
#include "db/connection_data.h"
#include "db/exception.h"
#include "db/connection_pool.h"

#include <list>

namespace db {
namespace backend {

struct Connection::Data {
  typedef std::list<ConnectionData *> ConnectionSpecificType;
  ConnectionSpecificType connection_specific;
  ~Data() {
    for (ConnectionSpecificType::iterator it = connection_specific.begin();
         it != connection_specific.end();
	 ++it) {
      delete *it;
    }
  }
};

SimpleRefPtr<Statement>
Connection::Prepare(const std::string& query) {
  if (default_is_prepared_) {
    return GetPreparedStatement(query);
  } else {
    return GetStatement(query);
  }
}

SimpleRefPtr<Statement>
Connection::GetStatement(const std::string& query) {
  SimpleRefPtr<Statement> statement = NewStatement(query);
  return statement;
}

SimpleRefPtr<Statement>
Connection::GetPreparedStatement(const std::string& query) {
  SimpleRefPtr<Statement> statement;
  if (!cache_.IsActive()) {
    statement = PrepareStatement(query);
    return statement;
  }
  statement = cache_.Fetch(query);
  if (!statement) {
    statement = PrepareStatement(query);
  }
  statement->SetCache(&cache_);
  return statement;
}

SimpleRefPtr<Statement>
Connection::GetPreparedUncachedStatement(const std::string& query) {
  SimpleRefPtr<Statement> statement = PrepareStatement(query);
  return statement;
}

Connection::Connection(const ConnectionInfo& connection_info) :
    data_(new Connection::Data),
    pool_(nullptr),
    once_called_(0),
    recyclable_(1) {
  int cache_size = connection_info.Get("@stmt_cache_size", 64);  
  if (cache_size > 0) {
    cache_.SetSize(cache_size);
  }
  std::string def_is_prep = connection_info.Get("@use_prepared", "on");
  if (def_is_prep == "on") {
    default_is_prepared_ = 1;
  } else if (def_is_prep == "off") {
    default_is_prepared_ = 0;
  } else {
    throw DBException("db::backend::Connection @use_prepared should"
		    " be either 'on' or 'off'");
  }

}

Connection::~Connection() {}

bool Connection::OnceCalled() const {
  return once_called_;
}

void Connection::SetOnceCalled(bool v) {
  once_called_ = v;
}

ConnectionData* Connection::GetConnectionData(const std::type_info& type) 
    const {
  for (Data::ConnectionSpecificType::const_iterator it = data_->connection_specific.begin();
       it != data_->connection_specific.end();
       ++it) {
    if (typeid(*it) == type) { // **it TODO
      return *it;
    }
  }
  return nullptr;
}

ConnectionData* Connection::ReleaseConnectionData(const std::type_info& type) {
  for (Data::ConnectionSpecificType::const_iterator it = 
       data_->connection_specific.begin();
       it != data_->connection_specific.end();
       ++it) {
    if (typeid(*it) == type) { // **it TODO
      return *it; 
    }
  }
  return nullptr;
}

void Connection::ResetConnectionData(const std::type_info& type,
    ConnectionData* ptr) {
  std::unique_ptr<ConnectionData> tmp(ptr);
  if (ptr && typeid(*ptr) != type) {
    throw DBException(std::string("db::ConnectionData::Inconsistent pointer "
			    "type") + typeid(*ptr).name()
		                    + " and std::type_info reference: "
				    + type.name() );
  }

  for (Data::ConnectionSpecificType::iterator it =
       data_->connection_specific.begin();
       it != data_->connection_specific.end();
       ++it) {
    if (typeid(*it) == type) {
      delete *it;
      if (ptr) {
        *it = tmp.release();  
      } else {
        data_->connection_specific.erase(it);
      }
      return;
    }
  }
  if (ptr) {
    data_->connection_specific.push_back(nullptr);
    data_->connection_specific.back() = tmp.release();
  }
}

SimpleRefPtr<ConnectionPool>
Connection::GetPool() {
  return pool_;
}

void Connection::SetPool(SimpleRefPtr<ConnectionPool> pool) {
  pool_ = pool;
}

void Connection::SetDriver(SimpleRefPtr<LoadableDriver> driver) {
  driver_ = driver;
}

void Connection::ClearCache() {
  cache_.Clear();
}

void Connection::SetRecyclable(bool v) {
  recyclable_ = v;
}

bool Connection::Recyclable() {
  return recyclable_;
}

void Connection::Dispose(Connection* connection) {
  if (!connection) {
    return;
  }

  SimpleRefPtr<ConnectionPool> pool = connection->pool_;
  connection->pool_ = nullptr;
  if (pool && connection->Recyclable()) {
    pool->Put(connection);   
  } else {
    connection->ClearCache();
    SimpleRefPtr<LoadableDriver> driver = connection->driver_;
    delete connection;
    driver.Reset();
  }
}

} // namespace backend

} // namespace db
