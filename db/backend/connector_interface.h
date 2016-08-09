#ifndef DB_BACKEND_CONNECTOR_INTERFACE_H_
#define DB_BACKEND_CONNECTOR_INTERFACE_H_
#include <memory>

#include "base/macros.h"
#include "db/common/connection_info.h"
#include "db/backend/db_connection.h"

namespace db {

class ConnectorInterface {
 public:
  ConnectorInterface() {}
  virtual ~ConnectorInterface() {}

  virtual DBConnection* Connect() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(ConnectorInterface);
};

class ConnectorFactory {
 public:
  virtual bool NewConnector(const ConnectionInfo& info,
                            std::unique_ptr<ConnectorInterface>* out_connector) = 0;
  virtual bool AcceptsOptions(const ConnectionInfo& info) = 0;

  virtual ~ConnectorFactory() {}

  static void Register(const std::string& driver, ConnectorFactory* factory);
  static bool GetFactory(const ConnectionInfo& info, ConnectorFactory** out_factory);
};

bool NewConnector(const ConnectionInfo& info,
                  std::unique_ptr<ConnectorInterface>* out_connector);

} // namespace db
#endif // DB_BACKEND_CONNECTOR_INTERFACE_H_
