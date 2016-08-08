#include "db/common/connection_manager.h"
#include "db/common/connection_pool.h"
#include "db/common/driver_manager.h"

namespace {

struct Initializer {
  Initializer() {
    db::ConnectionsManager::GetInstance();
  }
} init_; // Just use to Initialized ConnectionsManager

} // naespace
namespace db {

ConnectionsManager::ConnectionsManager() {}

ConnectionsManager::~ConnectionsManager() {}

ConnectionsManager& ConnectionsManager::GetInstance() {
  static ConnectionsManager instace;
  return instace;
}

DBConnectionPtr ConnectionsManager::Open(const std::string& str) {
  ConnectionPoolPtr pool;
  if (str.find("@pool_size") != std::string::npos) {
    threading::Guard g(lock_);
    ConnectionsType::iterator it = connections_.find(str);

    if (it != connections_.end()) {
      pool = it->second;
    }
  }

  if (pool) {
    return pool->Open();
  } else {
    ConnectionInfo info(str);
    return Open(info);
  }
}

DBConnectionPtr ConnectionsManager::Open(const ConnectionInfo& info) {
  if (info.Get("@pool_size", 0) == 0) {
    return DriverManager::GetInstance().Connect(info);
  }
  ConnectionPoolPtr pool;
  {
    threading::Guard g(lock_);
    ConnectionPoolPtr& pool_ref = connections_[info.connection_string];
    if (!pool_ref) {
      pool_ref = ConnectionPool::NewConnectionPool(info);
    }

    pool = pool_ref;
  }
  return pool->Open();
}

void ConnectionsManager::GC() {
  std::vector<ConnectionPoolPtr> pools;
  pools.reserve(100);
  {
    threading::Guard l(lock_);
    for (ConnectionsType::iterator it = connections_.begin();
         it != connections_.end();
	 ++it) {
      pools.push_back(it->second);
    }
  }

  for (unsigned i = 0; i < pools.size(); ++i) {
    pools[i]->GC();
  }

  pools.clear();

  {
    threading::Guard l(lock_);
    for (ConnectionsType::iterator it = connections_.begin();
         it != connections_.end();
	 ++it) {
      if (it->second->HasOneRef()) {
        pools.push_back(it->second);
	ConnectionsType::iterator t = it;
	++it;
	connections_.erase(t);
      } else {
        ++it;
      }
    }
  }
  pools.clear();
}


} // namespace db
