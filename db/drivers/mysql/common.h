#ifndef DB_DRIVERS_MYSQL_COMMON_H_
#define DB_DRIVERS_MYSQL_COMMON_H_
#include <mysql/mysql.h>
#include "db/backend/connection.h"
#include "db/backend/result.h"
#include "db/backend/driver.h"
#include "db/backend/statement.h"

#include "db/exception.h"
#include "db/numeric_util.h"
#include <sstream>
#include <vector>
#include <limits>
#include <iomanip>
#include <stdlib.h>
#include <string.h>

#include <iostream>

namespace db {
namespace mysql_backend {

class MyException : public db::DBException {
 public:
  MyException(const std::string& msg)
    : DBException("db::mysql::" + msg) {}
};

} // namespace mysql_backend
} // namespace db
#endif // DB_DRIVERS_MYSQL_COMMON_H_
