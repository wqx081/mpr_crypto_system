#ifndef DB_EXCEPTION_H_
#define DB_EXCEPTION_H_
#include <stdexcept>
#include <string>

namespace db {

class DBException : public std::runtime_error {
 public:
  DBException(const std::string& v) : std::runtime_error(v) {}
};

class BadValueCast : public DBException {
 public:
  BadValueCast(): DBException("db::bad_value_cast can't convert data") {}
};

class NullValueFetch : public DBException {
 public:
  NullValueFetch() : DBException("db::null_value_fetch attempt fetch null column") {}
};


} // namespace db
#endif // DB_EXCEPTION_H_
