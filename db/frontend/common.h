#ifndef DB_FRONTEND_COMMON_H_
#define DB_FRONTEND_COMMON_H_
#include "base/macros.h"
#include "base/ref_counted.h"
#include "base/time.h"

#include "db/common/connection_info.h"
#include "db/common/exception.h"
#include "db/backend/db_result.h"
#include "db/backend/db_statement.h"
#include "db/backend/db_connection.h"

#include <iosfwd>
#include <ctime>
#include <string>
#include <memory>
#include <typeinfo>

namespace db {

enum NullTagType {
 kNullValue = 0,
 kNotNullValue = 1,
};

namespace tags {

template<typename T>
struct IntoTag {
  T &value;
  NullTagType &tag;
  IntoTag(T &v, NullTagType&t) : value(v),tag(t) {}
};
	         
template<typename T>
struct UseTag {
  T value;
  NullTagType tag;
  UseTag(T v, NullTagType t) : value(v),tag(t) {}
};

} // namespace tags

template<typename T>
tags::IntoTag<T> Into(T& value, NullTagType& tag) {
  return tags::IntoTag<T>(value, tag);
}

inline tags::UseTag<const std::string&> Use(const std::string& v,
		                            NullTagType tag) {
  return tags::UseTag<const std::string& >(v, tag);
} 

inline tags::UseTag<const char*> Use(const char* v,
		                     NullTagType tag) {
  return tags::UseTag<const char *>(v, tag);
}

template<typename T>
tags::UseTag<T> Use(T value, NullTagType tag) {
  return tags::UseTag<T>(value, tag);
}

namespace details {

template<typename Object>
class Functor {
 public:
  Functor(const Functor& other)
    : functor_(other.functor_),
      wrapper_(other.wrapper_) {}

  const Functor& operator=(const Functor& other) {
    if (this != &other) {
      functor_ = other.functor_;
      wrapper_ = other.wrapper_;
    }
    return *this;
  }
  Functor(void (*func)(Object&)) {
    functor_ = reinterpret_cast<void *>(reinterpret_cast<size_t>(func));
    wrapper_ = &Functor::CallFunc;
  }
  template<typename RealFunctor>
  Functor(const RealFunctor& f) {
    functor_ = reinterpret_cast<const void *>(&f);
    wrapper_ = &Functor<Object>::template CallIt<RealFunctor>;
  }
  void operator()(Object& p) const {
    wrapper_(functor_, p);
  }
 private:
  static void CallFunc(const void* pointer, Object& parameter) {
    typedef void FunctionType(Object&);
    FunctionType* f = reinterpret_cast<FunctionType *>(reinterpret_cast<size_t>((pointer))); 
    f(parameter);
  }
  template<typename RealFunctor>
  static void CallIt(const void* pointer, Object& parameter) {
    const RealFunctor* f_ptr = reinterpret_cast<const RealFunctor *>(pointer);
    const RealFunctor& f_ref = *f_ptr;
    f_ref(parameter);
  }
  const void* functor_;
  void (*wrapper_)(const void*, Object&);
};

} // namespace details

class Session;
using OnceFunctor = details::Functor<Session>;


class DBConnectionThrowGuard {
 public:
  DBConnectionThrowGuard(const scoped_ref_ptr<DBConnection>& conn)
    : db_connection_(conn.get()) {}

  void Done() {
    db_connection_ = nullptr;
  }

  ~DBConnectionThrowGuard() {
    if (db_connection_ && std::uncaught_exception()) {
      db_connection_->set_recyclable(false);
    }
  }

 private:
  DBConnection* db_connection_;
};

} // namespace db
#endif // DB_FRONTEND_COMMON_H_
