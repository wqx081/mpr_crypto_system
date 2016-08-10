#ifndef DB_COMMON_CONNECTION_POOL_H_
#define DB_COMMON_CONNECTION_POOL_H_

#include "base/ref_counted.h"
#include "threading/mutex.h"
#include "db/common/connection_info.h"
#include "base/macros.h"
#include "base/time.h"

#include <memory>
#include <list>

namespace db {

class DBConnection;

class ConnectionPool : public base::RefCountedThreadSafe<ConnectionPool> {
 public:
  typedef scoped_ref_ptr<ConnectionPool> pointer;

  static scoped_ref_ptr<ConnectionPool> Create(const std::string& str);
  static scoped_ref_ptr<ConnectionPool> Create(const ConnectionInfo& info);

  scoped_ref_ptr<DBConnection> Open();
  void GC();
  void Clear();
  void Put(DBConnection* connection);

 private:
  ConnectionPool();
  ConnectionPool(const ConnectionInfo& info);

  scoped_ref_ptr<DBConnection> get();

  struct Entry {
    scoped_ref_ptr<DBConnection> db_connection;
    base::Time last_used;
  };

  using ConnectionPoolType = std::list<Entry>;

  size_t limit_;
  base::TimeDelta life_time_;
  ConnectionInfo connection_info_;

  threading::Mutex lock_;
  size_t size_;
  ConnectionPoolType connection_pool_;

  DISALLOW_COPY_AND_ASSIGN(ConnectionPool);
 private:
  friend base::RefCountedThreadSafe<ConnectionPool>;
  ~ConnectionPool();
};

} // namespace db
#endif // DB_COMMON_CONNECTION_POOL_H_
