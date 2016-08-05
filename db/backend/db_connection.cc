#include "db/backend/db_connection.h"
#include <list>

namespace db {

struct DBConnection::Data {
  using ConnectionDataType = std::list<ConnectionData *>;  
  ConnectionDataType connection_data;
  ~Data() {
    for (ConnectionDataType::iterator it = connection_data.begin();
         it != connection_data.end();
	 ++it) {
      delete *it;
    }
  }
};

DBStatementPtr DBConnection::Prepare(const std::string& query) {
  if (default_is_prepared_) {
    return GetPreparedStatement(query);
  } else {
    return GetStatement(query);
  }
}

DBStatementPtr DBConnection::GetStatement(const std::string& query) {
  DBStatementPtr statement = NewStatement(query);
  return statement;
}

DBStatementPtr DBConnection::GetPreparedStatement(const std::string& query) {

  DBStatementPtr statement;
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

DBStatementPtr DBConnection::GetPreparedUncachedStatement(const std::string& query) {
  DBStatementPtr statement = PrepareStatement(query);
  return statement;
}

DBConnection::DBConnection(const ConnectionInfo& info)
    : data_(new DBConnection::Data),
      connection_pool_(nullptr),
      once_called_(0),
      recyclable_(1) {
  int cache_size = info.Get("@stmt_cache_size", 64);      
  if (cache_size > 0) {
    cache_.SetSize(cache_size);
  }
  std::string prep_switch = info.Get("use_prepared", "on");
  if (prep_switch == "on") {
    default_is_prepared_ = 1;
  } else if (prep_switch == "off") {
    default_is_prepared_ = 0;
  } else {
    throw DBException("db::DBConnection: @use_prepared should be either "
		    "'on' or 'off'");
  }
}

DBConnection::~DBConnection() {}

bool DBConnection::OnceCalled() const {
  return once_called_;
}

void DBConnection::SetOnceCalled(bool v) {
  once_called_ = v;
}

ConnectionData* DBConnection::GetConnectionData(const std::type_info& type) const {
  for (Data::ConnectionDataType::const_iterator it = data_->connection_data.begin();
       it != data_->connection_data.end();
       ++it) {
    if (typeid(**it) == type) {
      return *it;
    }
  }
  return nullptr;
}

ConnectionData* DBConnection::ReleaseConnectionData( const std::type_info& type) {
  for (Data::ConnectionDataType::iterator it = data_->connection_data.begin();
       it != data_->connection_data.end();
       ++it) {
    if (typeid(**it) == type) {
      ConnectionData* data = *it;
      data_->connection_data.erase(it);
      return data;
    }
  }
  return nullptr;
}

void DBConnection::ResetConnectionData(const std::type_info& type,
		                       ConnectionData* data) {
  std::unique_ptr<ConnectionData> t(data);
  if (data && typeid(*data) != type) {
    throw DBException(std::string("db::ConnectionData::Inconsistent pointer type")
		    + typeid(*data).name()
		    + " and std::type_info reference: "
		    + type.name());

    for (Data::ConnectionDataType::iterator it = data_->connection_data.begin();
         it != data_->connection_data.end();
	 ++it) {
      if (typeid(**it) == type) {
        delete *it;
	if (data) {
	  *it = t.release();
	} else {
	  data_->connection_data.erase(it);
	}
	return;
      }
    }
  }

  if (data) {
    data_->connection_data.push_back(nullptr);
    data_->connection_data.back() = t.release();
  }
}

ConnectionPoolPtr DBConnection::GetConnectionPool() {
  return connection_pool_;
}

void DBConnection::SetConnectionPool(ConnectionPoolPtr pool) {
  connection_pool_ = pool;
}

void DBConnection::SetDriver(DBLoadableDriverPtr driver) {
  db_driver_ = driver;
}

void DBConnection::ClearCache() {
  cache_.Clear();
}

void DBConnection::SetRecyclable(bool v) {
  recyclable_ = v;
}

bool DBConnection::Recyclable() {
  return recyclable_;
}

// static
void DBConnection::Dispose(DBConnection* connection) {
  if (!connection) {
    return;
  }

  ConnectionPoolPtr pool = connection->connection_pool_;
  connection->connection_pool_ = nullptr;
  if (pool && connection->Recyclable()) {
    pool->Put(connection);
  } else {
    connection->ClearCache();
    DBLoadableDriverPtr driver = connection->db_driver_;
    delete connection;
    driver.Reset();
  }
}

} // namespace db
