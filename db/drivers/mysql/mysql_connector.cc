#include "db/drivers/mysql/mysql_connector.h"

namespace db {

// static
bool MysqlConnector::Create(const ConnectionInfo& connection_info,
                            std::unique_ptr<ConnectorInterface>* out_connector) {
  std::unique_ptr<MysqlConnector> ret(new MysqlConnector(connection_info));
  *out_connector = std::move(ret);
  return true;
}

namespace {

class MysqlConnectorFactory : public ConnectorFactory {
 public:
  bool AcceptsOptions(const ConnectionInfo& connection_info) override {
    return connection_info.driver == "mysql";  
  }

  bool NewConnector(const ConnectionInfo& connection_info,
                    std::unique_ptr<ConnectorInterface>* out_connector) override {
    return MysqlConnector::Create(connection_info, out_connector); 
  } 
};

// Registers a 'ConnectorFactory' for 'MysqlConnector' instances.
class MysqlConnectorRegistrar {
 public:
  MysqlConnectorRegistrar() {
    ConnectorFactory::Register("mysql", new MysqlConnectorFactory());  
  }
};
static MysqlConnectorRegistrar mysql_registrar;

} // namespace
} // namespace db
