#ifndef DB_BACKEND_DRIVER_H_
#define DB_BACKEND_DRIVER_H_
#include "base/macros.h"
#include "db/simple_ref_counted.h"
#include "db/connection_info.h"

//#include <functional>

namespace db {
namespace backend {

class Connection;

class Driver : public SimpleRefCounted {
 public:

  Driver() {}
  virtual ~Driver() {} 
  
  virtual bool InUse() = 0;
  virtual Connection* Open(const ConnectionInfo& connection_info) = 0;
  virtual Connection* Connect(const ConnectionInfo& connection_info);

 private:
  DISALLOW_COPY_AND_ASSIGN(Driver);
};

class LoadableDriver : public Driver {
 public:
  LoadableDriver() {}
  virtual ~LoadableDriver() override {}

  virtual bool InUse() override;
  virtual Connection* Connect(const ConnectionInfo& connection_info) override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LoadableDriver);
};

extern "C" {
typedef db::backend::Connection* DbBackendConnectFunction(const ConnectionInfo&);
} // extern "C"

class StaticDriver : public Driver {
 public:
  typedef DbBackendConnectFunction* ConnectFunctionType;

  StaticDriver(ConnectFunctionType connect_fn);
  ~StaticDriver();

  virtual bool InUse() override;
  virtual Connection* Open(const ConnectionInfo& connection_info) override;

 private:
  ConnectFunctionType connect_fn_;
};


} // namespace backend
} // namespace db
#endif // DB_BACKEND_DRIVER_H_
