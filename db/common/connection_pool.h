#ifndef DB_CONNECTION_POOL_H_
#define DB_CONNECTION_POOL_H_

#include "base/macros.h"
#include "base/time.h"

#include "db/common/simple_ref_counted.h"
#include "db/common/connection_info.h"

#include "threading/mutex.h"
#include <memory>
#include <list>

namespace db {

class DBConnection;
using DBConnectionPtr = SimpleRefPtr<DBConnection>;
class ConnectionPool;
using ConnectionPoolPtr = SimpleRefPtr<ConnectionPool>;


class ConnectionPool : public SimpleRefCounted {
 public:
  static ConnectionPoolPtr NewConnectionPool(const std::string& connection_tr);
  static ConnectionPoolPtr NewConnectionPool(const ConnectionInfo& connection_info);

  typedef SimpleRefPtr<ConnectionPool> pointer;

  ~ConnectionPool();

  DBConnectionPtr Open();
  void GC();
  void Clear();
  
  // internal
  void Put(DBConnection* connection); 

 private:
  DBConnectionPtr get();
//  struct Data;
//  std::unique_ptr<Data> data_;
  
  struct Entry {
    Entry() {}
    DBConnectionPtr db_connection;
    base::Time last_used;
  };
  typedef std::list<Entry> ConnectionPoolType;

  size_t limit_;
  base::TimeDelta life_time_;
  ConnectionInfo connection_info_;

  threading::Mutex lock_;
  size_t size_; // Guard by lock_
  ConnectionPoolType connection_pool_; // Guard by lock_


  ConnectionPool();
  ConnectionPool(const ConnectionInfo& connection_info);

  DISALLOW_COPY_AND_ASSIGN(ConnectionPool);
};

} // namespace db
#endif // DB_CONNECTION_POOL_H_
