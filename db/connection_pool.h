#ifndef DB_CONNECTION_POOL_H_
#define DB_CONNECTION_POOL_H_
#include "base/macros.h"
#include "base/time.h"

#include "db/simple_ref_counted.h"
#include "db/connection_info.h"

#include "threading/mutex.h"
#include <memory>
#include <list>

namespace db {

namespace backend {

class Connection;

} // namespace backend

class ConnectionPool : public SimpleRefCounted {
 public:
  static SimpleRefPtr<ConnectionPool> NewConnectionPool(const std::string& connection_tr);
  static SimpleRefPtr<ConnectionPool> NewConnectionPool(const ConnectionInfo& connection_info);

  typedef SimpleRefPtr<ConnectionPool> pointer;

  ~ConnectionPool();

  SimpleRefPtr<backend::Connection> Open();
  void GC();
  void Clear();
  
  // internal
  void Put(backend::Connection* connection);

 private:
  SimpleRefPtr<backend::Connection> get();
//  struct Data;
//  std::unique_ptr<Data> data_;
  
  struct Entry {
    Entry() {}
    SimpleRefPtr<backend::Connection> connection;
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
