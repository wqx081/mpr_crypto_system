#ifndef DB_BACKEND_CONNECTION_H_
#define DB_BACKEND_CONNECTION_H_
#include "base/string_piece.h"

#include "db/simple_ref_counted.h"
#include "db/backend/statement.h"
#include "db/backend/driver.h"
#include "db/connection_info.h"
#include "db/connection_data.h"

#include <memory>

namespace db {

class ConnectionPool;

namespace backend {

class Connection : public SimpleRefCounted {
 public:
  Connection(const ConnectionInfo& connection_info);
  virtual ~Connection();

  // Internal
  void SetPool(SimpleRefPtr<ConnectionPool> connection_pool);
  SimpleRefPtr<ConnectionPool> GetPool();
  void SetDriver(SimpleRefPtr<LoadableDriver> driver);
  static void Dispose(Connection* connection);
  SimpleRefPtr<Statement> Prepare(const std::string& q);
  SimpleRefPtr<Statement> GetPreparedStatement(const std::string& q);
  SimpleRefPtr<Statement> GetPreparedUncachedStatement(const std::string& q);
  SimpleRefPtr<Statement> GetStatement(const std::string& q);


  // API
  virtual void Begin() = 0;
  virtual void Commit() = 0;
  virtual void Rollback() = 0;
  virtual Statement* PrepareStatement(const std::string& p) = 0;
  virtual Statement* NewStatement(const std::string& q) = 0;

  virtual std::string Escape(const base::StringPiece& q) = 0;
  virtual std::string Escape(const char* begin, const char* end) = 0;
  // return name of driver
  virtual std::string Driver() = 0;
  virtual std::string Engine() = 0;
  
  void ClearCache();

  bool OnceCalled() const;
  void SetOnceCalled(bool v);

  ConnectionData* GetConnectionData(const std::type_info& type) const;  
  ConnectionData* ReleaseConnectionData(const std::type_info& type);
  void ResetConnectionData(const std::type_info& type, ConnectionData* connection_data=nullptr);
  
  bool Recyclable();
  void SetRecyclable(bool value);

 private:
  struct Data;
  std::unique_ptr<Data> data_;
  
  StatementsCache cache_;
  SimpleRefPtr<LoadableDriver> driver_;
  SimpleRefPtr<ConnectionPool> pool_;
  unsigned default_is_prepared_ : 1;
  unsigned once_called_: 1;
  unsigned recyclable_ : 1;
  unsigned reserverd_ : 29;
};

} // namespace backend
} // namespace db
#endif // DB_BACKEND_CONNECTION_H_
