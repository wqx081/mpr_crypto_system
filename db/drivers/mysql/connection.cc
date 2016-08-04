#include "db/backend/connection.h"
#include "db/connection_info.h"

extern "C" {

db::backend::Connection* DbMysqlGetConnection(const db::ConnectionInfo&
    connection_info) {
  (void) connection_info;
  return nullptr;
}  

}
