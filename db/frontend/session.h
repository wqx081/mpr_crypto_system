#ifndef DB_FRONTEND_SESSION_H_
#define DB_FRONTEND_SESSION_H_
#include "db/frontend/frontend.h"
#include "db/frontend/statement.h"
#include <functional>

namespace db {

namespace details {

template<typename Object>
class Functor {
 public:

  Functor(const Functor& other)
    : functor_(other.functor_),
      wrapper_(other.wrapper_) {}

  const Functor& operator=(const Functor& other) {
    functor_ = other.functor_;
    wrapper_ = other.wrapper_;
    return *this;
  }

  Functor(void (*func)(Object& )) {
    functor_ = reinterpret_cast<void *>(reinterpret_cast<size_t>(func));
    wrapper_ = &Functor::CallFunc;
  }
  
  template<typename RealFunctor>
  Functor(const RealFunctor& f) {
    functor_ = reinterpret_cast<const void *>(&f);
    wrapper_ = &Functor<Object>::template CallIt<RealFunctor>;
  }

  void operator()(Object& object) const {
    wrapper_(functor_, object);
  }

 private:
  const void* functor_;
  void (*wrapper_)(const void*, Object& );

  static void CallFunc(const void* pointer, Object& parameter) {
    typedef void function_type(Object& );
    function_type* fn = reinterpret_cast<function_type *>(
      reinterpret_cast<size_t>((pointer)));
    f(parameter);
  }

  template<typename RealFunctor>
  static void CallIt(const void* pointer, Object& parameter) {
    const RealFunctor* f_ptr = reinterpret_cast<const RealFunctor *>(pointer);
    const RealFunctor& f_ref = *f_ptr;
    f_ref(parameter);
  }
};

} // namespace details

class Session {
 public:
  typedef details::Functor<Session> OnceFunctor;

 public:
  Session();
  Session(const Session& other);
  const Session& operator=(const Session& other);
  ~Session();
  Session(const ConnectionInfo& connection_info);
  Session(const std::string& connection_str);

  Session(const ConnectionInfo& connection_info, const OnceFunctor& f);
  Session(const std::string& connection_str, const OnceFunctor& f);
  Session(SimpleRefPtr<backend::Connection> connection, const OnceFunctor& f);
  Session(SimpleRefPtr<backend::Connection> connection);
  
  
  void Open(const ConnectionInfo& connection_info);
  void Open(const std::string& connection_str);

  void Close();
  bool IsOpen();

  Statement Prepare(const std::string& query);
  Statement operator<<(const std::string& query);
  Statement operator<<(const char* s);
  Statement NewStatement(const std::string& query);
  Statement NewPreparedStatement(const std::string& query);
  Statement NewPreparedUncachedStatement(const std::string& query);

  void ClearCache();
  void ClearConnectionPool();
  void Begin();
  void Commit();
  void Rollback();
  std::string Escape(const base::StringPiece& s);
  std::string Escape(const char* b, const char* e);

  std::string Driver();
  std::string Engine();

  bool Recyclable();
  void SetRecyclable(bool value);

  bool OnceCalled();
  void SetOnceCalled(bool state);
  void Once(const OnceFunctor& f);
  
  ConnectionData* GetConnectionData(const std::type_info& t);
  ConnectionData* ReleaseConnectionData(const std::type_info& t);
  void SetConnectionData(const std::type_info& t, ConnectionData* data=nullptr);

  template<typename T>
  T* GetConnectionData() {
    return static_cast<T*>(GetConnectionData(typeid(T)));
  }

  template<typename T>
  T* ReleaseConnectionData() {
    return static_cast<T*>(ReleaseConnectionData(typeid(T)));
  }

  template<typename T>
  void ReleaseConnectionData(T* data=nullptr) {
    ReleaseConnectionData(typeid(T), data);
  }

 private:
  SimpleRefPtr<backend::Connection> connection_;  
}; 

} // namespace db
#endif // DB_FRONTEND_SESSION_H_
