#ifndef DB_BACKEND_DB_DRIVER_H_
#define DB_BACKEND_DB_DRIVER_H_
#include "base/macros.h"
#include "db/common/connection_info.h"
#include "db/backend/common.h"

namespace db {

class DBConnection;

class DBDriver : public SimpleRefCounted {
 public:
  DBDriver() {}
  virtual ~DBDriver() {}

  virtual bool InUse() = 0;
  virtual DBConnection* Open(const ConnectionInfo& connection_info) = 0;
  virtual DBConnection* Connect(const ConnectionInfo& connection_info);

 private:
  DISALLOW_COPY_AND_ASSIGN(DBDriver);
};

class DBLoadableDriver : public DBDriver {
 public:
  DBLoadableDriver() {}
  virtual ~DBLoadableDriver() override {}

  virtual bool InUse() override;
  virtual DBConnection* Connect(const ConnectionInfo& connection_info) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(DBLoadableDriver);
};

extern "C" {
typedef db::DBConnection* db_backend_connect_function(const ConnectionInfo&);
} // extern "C"

class DBStaticDriver : public DBDriver {
 public:
  using ConnectFunctionType = db_backend_connect_function*;

  DBStaticDriver(ConnectFunctionType connect_fn);
  ~DBStaticDriver();

  virtual bool InUse() override;
  DBConnection* Open(const ConnectionInfo& connection_info) override;
 
 private:
  ConnectFunctionType connect_fn_;
};

using DBLoadableDriverPtr = SimpleRefPtr<DBLoadableDriver>;
using DBStaticDriverPtr = SimpleRefPtr<DBStaticDriver>;

} // namespace db
#endif // DB_BACKEND_DB_DRIVER_H_
