#ifndef DB_COMMON_EXCEPTION_H_
#define DB_COMMON_EXCEPTION_H_
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


class EmptyRowAccess : public DBException {
 public:
  EmptyRowAccess() : DBException("db::empty_row_access attempt to fetch "
		  "from empty column") {}
};

class InvalidColumn : public DBException {
 public:
  InvalidColumn() : DBException("db::invalid_column attempt access to "
		  "invalid column") {}
};

class InvalidPlaceholder : public DBException {
 public:
  InvalidPlaceholder() : DBException("db::invalid_placeholder attempt bind "
		  "to invalid placeholder") {}
};

class MultipleRowsQuery : public DBException {
 public:
  MultipleRowsQuery() : DBException("db::multiple_rows_query multiple rows result for a "
                                    "single row request") {}
};

} // namespace db
#endif // DB_COMMON_EXCEPTION_H_
