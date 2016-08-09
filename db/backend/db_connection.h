#ifndef DB_BACKEND_DB_CONNECTION_H_
#define DB_BACKEND_DB_CONNECTION_H_
#include "db/backend/common.h"
#include "db/backend/db_statement.h"

#include "base/ref_counted.h"

namespace db {

class DBConnection : public base::RefCountedThreadSafe<DBConnection> {
 public:
  DBConnection(const ConnectionInfo&);
  scoped_ref_ptr<DBStatement> GetDirectStatement(const std::string& query);

  virtual void Begin() = 0;
  virtual void Commit() = 0;
  virtual void Rollback() = 0;
  virtual DBStatement* NewDirectStatement(const std::string& query) = 0;
  virtual std::string Escape(const std::string& ) = 0;
  virtual std::string Escape(const char* str) = 0;
  virtual std::string Escape(const char* begin, const char* end) = 0;
  virtual std::string Driver() = 0;
  virtual std::string Engine() = 0;
  
  void ClearCache();
  bool once_called() const;
  void set_once_called(bool v);
  
  bool recyclable() const;
  void set_recyclable(bool value);

 protected:
  virtual ~DBConnection();

 private:
  DBStatementCache cache_;  
  unsigned default_is_prepared_ : 1;
  unsigned once_called_ : 1;
  unsigned recyclable_ : 1;
  unsigned reserverd_ : 29;

 private:
  friend base::RefCountedThreadSafe<DBConnection>;
};

} // namespace db
#endif // DB_BACKEND_DB_CONNECTION_H_
