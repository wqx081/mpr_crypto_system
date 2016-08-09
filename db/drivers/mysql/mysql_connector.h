#ifndef DB_DRIVERS_MYSQL_MYSQL_CONNECTOR_H_
#define DB_DRIVERS_MYSQL_MYSQL_CONNECTOR_H_
#include "db/backend/connector_interface.h"
#include "db/backend/db_connection.h"
#include "db/drivers/mysql/mysql_connection.h"

namespace db {

class MysqlConnector : public ConnectorInterface {
 protected:
  MysqlConnector(const ConnectionInfo& info) : connection_info_(info) {}

 public: 
  ~MysqlConnector() {} 

  static bool Create(const ConnectionInfo& connection_info,
                     std::unique_ptr<ConnectorInterface>* out_connector);

  virtual DBConnection* Connect() override {
    return new MysqlConnection(connection_info_);
  }
 private:
  const ConnectionInfo connection_info_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_CONNECTOR_H_
