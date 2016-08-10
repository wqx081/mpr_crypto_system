#ifndef DB_FRONTEND_SESSION_H_
#define DB_FRONTEND_SESSION_H_
#include "db/frontend/common.h"
#include "db/frontend/statement.h"

#include "db/common/connection_info.h"

namespace db {

class Session {
 public:
  Session();
  Session(const Session& other);
  const Session& operator=(const Session& other);
  ~Session();
  
  Session(const ConnectionInfo& info);
  Session(const std::string& info);
  Session(const ConnectionInfo& ifo, const OnceFunctor& f);
  Session(const std::string& info, const OnceFunctor& f);
  Session(scoped_ref_ptr<DBConnection> db_connection, const OnceFunctor& f);
  Session(scoped_ref_ptr<DBConnection> db_connection);

  void Open(const ConnectionInfo& info);
  void Open(const std::string& info);
  void Close();
  bool IsOpen();
  
  Statement Prepare(const std::string& query);
  Statement operator<<(const std::string& query);
  Statement operator<<(const char* s);

  Statement NewDirectStatement(const std::string& query);
  Statement NewPreparedStatement(const std::string& query);
  Statement NewPreparedUncachedStatement(const std::string& query);

  void ClearCache();
  //void ClearPool();

  void Begin();
  void Commit();
  void Rollback();
  std::string Escape(const char* begin, const char* end);
  std::string Escape(const char* str);
  std::string Escape(const std::string& str);

  std::string Driver();
  std::string Engine();
  
  bool recyclable() const;
  void set_recyclable(bool v);

  bool once_called() const;
  void set_once_called(bool v);
  void Once(const OnceFunctor& f);

// TODO
// ConnectionData

 private:
  scoped_ref_ptr<DBConnection> db_connection_;
};

} // namespace db
#endif // DB_FRONTEND_SESSION_H_
