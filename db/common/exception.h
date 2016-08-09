#ifndef DB_COMMON_EXCEPTION_H_
#define DB_COMMON_EXCEPTION_H_
#include <stdexcept>
#include <string>

namespace db {

class DBException : public std::runtime_error {
 public:
  DBException(const std::string& msg) : std::runtime_error(msg) {}
};

} // namespace db
#endif // DB_COMMON_EXCEPTION_H_
