#ifndef DB_COMMON_CONNECTION_MANAGER_H_
#define DB_COMMON_CONNECTION_MANAGER_H_
#include "base/macros.h"
#include "base/ref_counted.h"
#include "db/common/connection_pool.h"

namespace db {

class ConnectionManager {
 public:
  static ConnectionManager& GetInstance();
  scoped_ref_ptr<DBConnection> Open(const std::string& str);
  scoped_ref_ptr<DBConnection> Open(const ConnectionInfo& info);
  void GC();
 private:
  ConnectionManager();
  ~ConnectionManager();

  threading::Mutex lock_;
  using ConnectionsType = std::map<std::string, 
	                           scoped_ref_ptr<ConnectionPool>>;
  ConnectionsType connections_;

  DISALLOW_COPY_AND_ASSIGN(ConnectionManager);
};

} // namespace db
#endif // DB_COMMON_CONNECTION_MANAGER_H_
