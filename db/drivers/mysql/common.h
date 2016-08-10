#ifndef DB_DRIVERS_MYSQL_COMMON_H_
#define DB_DRIVERS_MYSQL_COMMON_H_
#include "db/common/connection_info.h"
#include "db/common/exception.h"


#include <mysql/mysql.h>

#include <sstream>
#include <vector>
#include <limits>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
  
#include <iostream>


namespace db {

class MysqlException : public DBException {
 public:
  MysqlException(const std::string& msg) :
    DBException("db::mysql::" + msg) {}
};

} // namespace
#endif // DB_DRIVERS_MYSQL_COMMON_H_
