#include "db/common/connection_pool.h"
#include "db/backend/connector_interface.h"

#include <stdlib.h>

namespace db {

scoped_ref_ptr<ConnectionPool> 
ConnectionPool::Create(const ConnectionInfo& info) {
  scoped_ref_ptr<ConnectionPool> result(new ConnectionPool(info));
  return result;
}

scoped_ref_ptr<ConnectionPool>
ConnectionPool::Create(const std::string& str) {
  ConnectionInfo info(str);
  scoped_ref_ptr<ConnectionPool> result(new ConnectionPool(info));
  return result;
}

ConnectionPool::ConnectionPool(const ConnectionInfo& info)
    : limit_(0),
      connection_info_(info),
      size_(0) {
  limit_ = info.Get("@pool_size", 16);
  life_time_ = base::TimeDelta::FromSeconds(info.Get("@pool_max_idle", 600));
}

ConnectionPool::~ConnectionPool() {}

scoped_ref_ptr<DBConnection> ConnectionPool::Open() {
  std::unique_ptr<db::ConnectorInterface> connector;
  if (limit_ == 0) {
    db::NewConnector(connection_info_, &connector);
    return connector->Connect();
  }

  db::NewConnector(connection_info_, &connector);
  scoped_ref_ptr<DBConnection> connection = get();
  if (!connection) {
    connection = connector->Connect();
  }
  connection->set_connection_pool(this);
  return connection;
}

scoped_ref_ptr<DBConnection> ConnectionPool::get() {
  if (limit_ == 0) {
    return nullptr;
  }
  scoped_ref_ptr<DBConnection> connection = nullptr;
  ConnectionPoolType garbage;
  base::Time now = base::Time::Now();
  {
    threading::Guard l(lock_);
    ConnectionPoolType::iterator it = connection_pool_.begin();
    ConnectionPoolType::iterator tmp;
    while (it != connection_pool_.end()) {
      if (it->last_used + life_time_ < now) {
        tmp = it;
	it++;
	garbage.splice(garbage.begin(),
                       connection_pool_,
		       tmp);
	size_--;
      } else {
        break;
      }
    }
    if (!connection_pool_.empty()) {
      connection = connection_pool_.back().db_connection;
      connection_pool_.pop_back();
      size_--;
    }
  }
  return connection;
}

void ConnectionPool::Put(DBConnection* connection_in) {
  std::unique_ptr<DBConnection> connection(connection_in);
  if (limit_ == 0) {
    return;
  }
  ConnectionPoolType garbage;
  base::Time now = base::Time::Now();
  {
    threading::Guard l(lock_);

    if (connection.get()) {
      connection_pool_.push_back(Entry());
      connection_pool_.back().last_used = now;
      connection_pool_.back().db_connection = connection.release();
      size_++;
    }

    ConnectionPoolType::iterator it = connection_pool_.begin();
    ConnectionPoolType::iterator tmp;
    while (it != connection_pool_.end()) {
      if (it->last_used + life_time_ < now) {
        tmp = it;
	it++;
	garbage.splice(garbage.begin(), connection_pool_, tmp);
	size_--;
      } else {
        break;
      }
    }

    if (size_ > limit_) {
      garbage.splice(garbage.begin(), 
		     connection_pool_, 
		     connection_pool_.begin());
      size_--;
    }
  }
}

void ConnectionPool::GC() {
  Put(nullptr);
}

void ConnectionPool::Clear() {
  ConnectionPoolType garbage;
  {
    threading::Guard l(lock_);
    garbage.swap(connection_pool_);
    size_ = 0;
  }
}

} // namespace
