#include "db/connections_manager.h"
#include "db/connection_pool.h"
#include "db/backend/connection.h"
#include "db/driver_manager.h"


namespace db {

namespace {

struct Initiliazer {
  Initiliazer() {
    ConnectionsManager::GetInstance();
  }
} initializer;

} // namespace

ConnectionsManager::ConnectionsManager() {}
ConnectionsManager::~ConnectionsManager() {}


ConnectionsManager& ConnectionsManager::GetInstance() {
  static ConnectionsManager instance;
  return instance;
}

SimpleRefPtr<backend::Connection>
ConnectionsManager::Open(const std::string& connection_str) {

  SimpleRefPtr<ConnectionPool> pool;
  if (connection_str.find("@pool_size") != std::string::npos) {
    threading::Guard g(lock_);
    ConnectionsType::iterator it = connections_.find(connection_str);
    if (it != connections_.end()) {
      pool = it->second;
    }
  }
  if (pool) {
    return pool->Open();
  } else {
    ConnectionInfo connection_info(connection_str);
    return Open(connection_info);
  }
}

SimpleRefPtr<backend::Connection>
ConnectionsManager::Open(const ConnectionInfo& connection_info) {
  if (connection_info.Get("@pool_size", 0) == 0) {
    return DriverManager::GetInstance().Connect(connection_info);
  }
  SimpleRefPtr<ConnectionPool> pool;
  {
    threading::Guard g(lock_);
    SimpleRefPtr<ConnectionPool>& pool_ref = connections_[connection_info.connection_string];
    if (!pool_ref) {
      pool_ref = ConnectionPool::NewConnectionPool(connection_info);
    }
    pool = pool_ref;
  }
  return pool->Open();
}

void ConnectionsManager::GC() {
  std::vector<SimpleRefPtr<ConnectionPool>> pools;
  pools.reserve(100);
  {
    threading::Guard g(lock_);
    for (ConnectionsType::iterator it = connections_.begin();
         it != connections_.end();
         ++it) {
      pools.push_back(it->second);
    }
  }
  for (unsigned int i = 0; i < pools.size(); ++i) {
    pools[i]->GC();
  }
  pools.clear();
  {
    threading::Guard g(lock_);
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
