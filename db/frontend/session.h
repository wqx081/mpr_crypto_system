#ifndef DB_FRONTEND_SESSION_H_
#define DB_FRONTEND_SESSION_H_
#include "db/frontend/common.h"
#include "db/frontend/statement.h"

#include "db/backend/db_connection.h"

namespace db {

class Session final {
 public:
  Session();
  Session(const Session&);
  const Session& operator=(const Session&);
  ~Session();
  Session(const ConnectionInfo& info);
  Session(const std::string& str);
  Session(const ConnectionInfo& info, const OnceFunctor& f);
  Session(const std::string& str, const OnceFunctor& f);

  Session(DBConnectionPtr connection, const OnceFunctor& f);
  Session(DBConnectionPtr connection);

  void Open(const ConnectionInfo& info);
  void Open(const std::string& str);
  void Close();
  bool IsOpen();
  
  Statement Prepare(const std::string& query);
  Statement operator<<(const base::StringPiece& str);

  Statement NewDirectStatement(const base::StringPiece& query);
  Statement NewPrepareStatement(const base::StringPiece& query);
  Statement NewPrepareUncachedStatement(const base::StringPiece& query);

  void ClearCache();
  void ClearConnectionPool();
  void Begin();
  void Commit();
  void Rollback();
  
  std::string Escape(const base::StringPiece& str);
  std::string Escape(const char* begin, const char* end);

  std::string Driver();
  std::string Engine();

  bool Recyclable();
  void SetRecyclable(bool v);

  bool OnceCalled();
  void SetOnceCalled(bool v);
  void Once(const OnceFunctor& f);

  ConnectionData* GetConnectionData(const std::type_info& type);
  ConnectionData* ReleaseConnectionData(const std::type_info& type);
  void ResetConnectionData(const std::type_info& type, ConnectionData* data=nullptr);

  template<typename T>
  T* GetConnectionData() {
    return static_cast<T*>(GetConnectionData(typeid(T)));
  }

  template<typename T>
  T* ReleaseConnectionData() {
    return static_cast<T*>(ReleaseConnectionData(typeid(T)));
  }

  template<typename T>
  void ResetConnectionData(T* data=nullptr) {
    ResetConnectionData(typeid(T), data);
  }

 private:
  DBConnectionPtr db_connection_;  
};

} // namespace db
#endif // DB_FRONTEND_SESSION_H_
