#include "db/drivers/mysql/mysql_connection.h"


extern "C" {

//TODO
db::backend::Connection* DbMysqlGetConnection(const db::ConnectionInfo&
                                              connection_info) {
  (void) connection_info;
  return nullptr;
}

}
