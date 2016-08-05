#include "db/connection_pool.h"
#include "db/driver_manager.h"
#include "db/connection_info.h"
#include "db/simple_ref_counted.h"
#include "db/backend/connection.h"
#include "base/time.h"
#include "threading/mutex.h"

#include <stdlib.h>

namespace db {

SimpleRefPtr<ConnectionPool>
ConnectionPool::NewConnectionPool(const ConnectionInfo& connection_info) {
  SimpleRefPtr<ConnectionPool> pool = new ConnectionPool(connection_info);
  return pool;
}

SimpleRefPtr<ConnectionPool>
ConnectionPool::NewConnectionPool(const std::string& connection_string) {
  ConnectionInfo info(connection_string);
  SimpleRefPtr<ConnectionPool> pool = new ConnectionPool(info);
  return pool;
}

ConnectionPool::ConnectionPool(const ConnectionInfo& connection_info)
    : limit_(0),
      connection_info_(connection_info),
      size_(0) {
  limit_ = connection_info_.Get("@pool_size", 16);
  life_time_ = base::TimeDelta::FromSeconds(connection_info_.Get("@pool_max_idel", 600));
}

ConnectionPool::~ConnectionPool() {}


SimpleRefPtr<backend::Connection> ConnectionPool::Open() {
  if (limit_ == 0) {
    return DriverManager::GetInstance().Connect(connection_info_);
  }
  SimpleRefPtr<backend::Connection> connection = get();
  if (!connection) {
    connection = DriverManager::GetInstance().Connect(connection_info_);
  }
  connection->SetPool(this);
  return connection;
}

SimpleRefPtr<backend::Connection>
ConnectionPool::get() {
  if (limit_ == 0) {
    return 0;
  }
  SimpleRefPtr<backend::Connection> connection;
  ConnectionPoolType garbage;
  base::Time now = base::Time::Now();  
  {
    threading::Guard g(lock_);
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
      connection = connection_pool_.back().connection;
      connection_pool_.pop_back();
      size_--;
    }
  }
  return connection;
}

void ConnectionPool::Put(backend::Connection* connection) {
  std::unique_ptr<backend::Connection> conn(connection);
  if (limit_ == 0) {
    return;
  }

  ConnectionPoolType garbage;
  base::Time now = base::Time::Now();
  {
    threading::Guard g(lock_);
    if (conn.get()) {
      connection_pool_.push_back(Entry());
      connection_pool_.back().last_used = now;
      connection_pool_.back().connection = conn.release();
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
    threading::Guard g(lock_);
    garbage.swap(connection_pool_);
    size_ = 0;
  }
}


} // namespace db
