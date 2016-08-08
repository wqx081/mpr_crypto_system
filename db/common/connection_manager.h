#ifndef DB_COMMON_CONNECTION_MANAGER_H_
#define DB_COMMON_CONNECTION_MANAGER_H_
#include "base/macros.h"

#include "db/common/simple_ref_counted.h"
#include "db/common/connection_pool.h"
#include "db/common/connection_info.h"

#include "db/backend/db_connection.h"

#include "threading/mutex.h"

#include <map>
#include <string>
#include <memory>

namespace db {

class ConnectionsManager final {
 public:
  static ConnectionsManager& GetInstance();
  DBConnectionPtr Open(const ConnectionInfo& info);
  DBConnectionPtr Open(const std::string& str);
  void GC();

 private:
  using ConnectionsType = std::map<std::string, ConnectionPoolPtr>;

  threading::Mutex lock_;
  ConnectionsType connections_;

  // for singleton pattern
  ConnectionsManager();
  ~ConnectionsManager();

  DISALLOW_COPY_AND_ASSIGN(ConnectionsManager);
};

} // namespace
#endif // DB_COMMON_CONNECTION_MANAGER_H_
