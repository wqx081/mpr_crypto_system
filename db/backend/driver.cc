#include "db/backend/driver.h"
#include "db/backend/connection.h"
#include "db/connection_info.h"

namespace db {
namespace backend {

Connection* Driver::Connect(const ConnectionInfo& connection_info) {
  return Open(connection_info);
}

bool LoadableDriver::InUse() {
  return !HasZeroRef();
}

Connection* LoadableDriver::Connect(const ConnectionInfo& connection_info) {
  Connection* connection = Open(connection_info);
  connection->SetDriver(SimpleRefPtr<LoadableDriver>(this));
  return connection;
}

StaticDriver::StaticDriver(ConnectFunctionType conn)
    : connect_fn_(conn) {}

StaticDriver::~StaticDriver() {}

bool StaticDriver::InUse() {
  return true;
}

Connection* StaticDriver::Open(const ConnectionInfo& connection_info) {
  return connect_fn_(connection_info);
}

} // namespace backend
} // namespace db
