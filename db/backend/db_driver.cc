#include "db/backend/db_driver.h"
#include "db/backend/db_connection.h"

namespace db {

DBConnection* DBDriver::Connect(const ConnectionInfo& info) {
  return Open(info);
}

bool DBLoadableDriver::InUse() {
  return !HasZeroRef();
}

DBConnection* DBLoadableDriver::Connect(const ConnectionInfo& info) {
  DBConnection* connection = Open(info);
  connection->SetDriver(DBLoadableDriverPtr(this));
  return connection;
}

DBStaticDriver::DBStaticDriver(ConnectFunctionType connect_fn)
  : connect_fn_(connect_fn) {}

DBStaticDriver::~DBStaticDriver() {}

bool DBStaticDriver::InUse() {
  return true;
}

DBConnection* DBStaticDriver::Open(const ConnectionInfo& info) {
  return connect_fn_(info);
}

} // namespace db
