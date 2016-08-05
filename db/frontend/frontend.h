#ifndef DB_FRONTEND_FRONTEND_H_
#define DB_FRONTEND_FRONTEND_H_
#include "db/exception.h"
#include "db/simple_ref_counted.h"
#include "db/backend/backend.h"
#include "db/connection_data.h"
#include "db/connection_info.h"

#include <string>

namespace db {

class Result;
class Statement;
class Session;

class ConnectionThrowGuard {
 public:
  ConnectionThrowGuard(const SimpleRefPtr<backend::Connection>& connection)
    : connection_(connection.get()) {}

  void Done() {
    connection_ = nullptr;
  }

  ~ConnectionThrowGuard() {
    if (connection_ && std::uncaught_exception()) {
      connection_->SetRecyclable(false);
    }
  }
 private:
  backend::Connection* connection_;      
};


enum NullTagType {
  kNullValue,
  kNotNullValue
};

namespace tags {

template<typename T>
struct IntoTag {
  T& value;
  NullTagType& tag;
  IntoTag(T& v, NullTagType& t) : value(v), tag(t) {}
};

template<typename T>
struct UseTag {
  T value;
  NullTagType tag;
  UseTag(T v, NullTagType t) : value(v), tag(t) {}
};

} // namespace tags

template<typename T>
inline tags::IntoTag<T> Into(T& value, NullTagType& tag) {
  return tags::IntoTag<T>(value, tag);
}

inline tags::UseTag<const std::string&> Use(const std::string& value, NullTagType tag) {
  return tags::UseTag<const std::string&>(value, tag);
}

inline tags::UseTag<const char*> Use(const char* value, NullTagType tag) {
  return tags::UseTag<const char*>(value, tag);
}

template<typename T>
tags::UseTag<T> Use(T value, NullTagType tag) {
 return tags::UseTag<T>(value, tag);
}


} // namespace db
#endif // DB_FRONTEND_FRONTEND_H_
