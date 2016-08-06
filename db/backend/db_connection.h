#ifndef DB_BACKEND_DB_CONNECTION_H_
#define DB_BACKEND_DB_CONNECTION_H_
#include "db/backend/common.h"
#include "db/backend/db_driver.h"
#include "db/backend/db_statement.h"
#include "db/common/connection_pool.h"

namespace db {

class DBConnection : public SimpleRefCounted {
 public:
  DBConnection(const ConnectionInfo& info);
  virtual ~DBConnection();

  // INTERNAL
  void SetConnectionPool(ConnectionPoolPtr pool);
  ConnectionPoolPtr GetConnectionPool();
  void SetDriver(DBLoadableDriverPtr driver);
  static void Dispose(DBConnection* connection);
  DBStatementPtr Prepare(const std::string& query);
  DBStatementPtr GetPrepareStatement(const std::string& query);
  DBStatementPtr GetPreparedUncachedStatement(const std::string& query);
  DBStatementPtr GetDirectStatement(const std::string& query);

  // API
  virtual void Begin() = 0;
  virtual void Commit() = 0;
  virtual void Rollback() = 0;

  virtual DBStatement* NewPrepareStatement(const std::string& query) = 0;
  virtual DBStatement* NewDirectStatement(const std::string& query) = 0;

  virtual std::string Escape(const base::StringPiece& str) = 0;
  virtual std::string Escape(const char* begin, const char* end) = 0;
  virtual std::string Driver() = 0;
  virtual std::string Engine() = 0;

  bool OnceCalled() const;
  void SetOnceCalled(bool v);

  ConnectionData* GetConnectionData(const std::type_info& type) const;
  ConnectionData* ReleaseConnectionData(const std::type_info& type);
  void ResetConnectionData(const std::type_info& type, ConnectionData* data);

  void ClearCache();

  bool Recyclable();
  void SetRecyclable(bool v);

 private:
  struct Data;
  std::unique_ptr<Data> data_;
  DBStatementsCache cache_;
  DBLoadableDriverPtr db_driver_;
  ConnectionPoolPtr connection_pool_;
  unsigned default_is_prepared_ : 1;
  unsigned once_called_ : 1;
  unsigned recyclable_ : 1;
  unsigned reserverd_ : 29;
};

} // namespace db
#endif // DB_BACKEND_DB_CONNECTION_H_
