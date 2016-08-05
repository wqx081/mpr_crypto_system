#ifndef DB_CONNECTIONS_MANAGER_H_
#define DB_CONNECTIONS_MANAGER_H_
#include "base/macros.h"

#include "db/simple_ref_counted.h"
#include "threading/mutex.h"

#include <map>
#include <string>
#include <memory>

namespace db {

class ConnectionPool;
class ConnectionInfo;

namespace backend {
class Connection;
} // namespace backend

class ConnectionsManager {
 public:
  static ConnectionsManager& GetInstance();
  SimpleRefPtr<backend::Connection> Open(const std::string& connection_str);
  SimpleRefPtr<backend::Connection> Open(const ConnectionInfo& connection_info);
  void GC();

 private:
  threading::Mutex lock_;
  typedef std::map<std::string, SimpleRefPtr<ConnectionPool>> ConnectionsType;
  ConnectionsType connections_;
 
  ConnectionsManager();
  ~ConnectionsManager();


  DISALLOW_COPY_AND_ASSIGN(ConnectionsManager);
};

} // namespace db
#endif // DB_CONNECTIONS_MANAGER_H_
