#ifndef DB_DRIVERS_MYSQL_COMMON_H_
#define DB_DRIVERS_MYSQL_COMMON_H_
#include "db/common/exception.h"
#include "db/common/numeric_util.h"

#include <mysql/mysql.h>
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

} // namespace db
} // namespace db
#endif //DB_DRIVERS_MYSQL_COMMON_H_
