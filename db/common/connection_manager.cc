#include "db/common/connection_manager.h"
#include "db/backend/connector_interface.h"

namespace db {

ConnectionManager::ConnectionManager() {}
ConnectionManager::~ConnectionManager() {}


ConnectionManager& ConnectionManager::GetInstance() {
  static ConnectionManager mgr;
  return mgr;
}

scoped_ref_ptr<DBConnection>
ConnectionManager::Open(const std::string& str) {
  scoped_ref_ptr<ConnectionPool> pool;

  if (str.find("@pool_size") != std::string::npos) {
    threading::Guard l(lock_);
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

scoped_ref_ptr<DBConnection>
ConnectionManager::Open(const ConnectionInfo& info) {

  std::unique_ptr<db::ConnectorInterface> connector;

  if (info.Get("@pool_size", 0) == 0) {
    db::NewConnector(info, &connector);
    return connector->Connect();
  }
  scoped_ref_ptr<ConnectionPool> pool;
  {
    threading::Guard l(lock_);
    scoped_ref_ptr<ConnectionPool>& ref = connections_[info.connection_string];
    if (!ref) {
      ref = ConnectionPool::Create(info);
      pool = ref;
    }
  }
  return pool->Open();
}

void ConnectionManager::GC() {
  std::vector<scoped_ref_ptr<ConnectionPool>> pools;
  pools.reserve(100);
  {
    threading::Guard l(lock_);
    for (ConnectionsType::iterator it = connections_.begin();
         it != connections_.end();
	 ++it) {
      pools.push_back(it->second);
    }
  }

  for (size_t i = 0; i < pools.size(); ++i) {
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
	ConnectionsType::iterator tmp = it;
	++it;
	connections_.erase(tmp);
      } else {
        ++it;
      }
    }
  }
  pools.clear();
}

#if 0
namespace {
struct Initializer {
  Initializer() {
    ConnectionManager::GetInstance();
  }
} init;
} // namespace
#endif

} // namespace db
