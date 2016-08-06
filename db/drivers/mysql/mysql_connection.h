#ifndef DB_DRIVERS_MYSQL_MYSQL_CONNECTION_H_
#define DB_DRIVERS_MYSQL_MYSQL_CONNECTION_H_
#include "db/backend/db_connection.h"
#include "db/drivers/mysql/common.h"

extern "C" {
db::DBConnection* db_mysql_get_connection(const db::ConnectionInfo& info);
}// extern "C"

namespace db {

class MysqlConnection;

class MysqlConnection : public DBConnection {
 public:
  MysqlConnection(const ConnectionInfo& info);
  virtual ~MysqlConnection() override;
  
  void Execute(const base::StringPiece&);

  // API
  virtual void Begin() override;
  virtual void Commit() override;
  virtual void Rollback() override;

  virtual DBStatement* NewPrepareStatement(const std::string& query) override;
  virtual DBStatement* NewDirectStatement(const std::string& query) override;
  
  virtual std::string Escape(const base::StringPiece& str) override;
  virtual std::string Escape(const char* begin, const char* end) override;

  virtual std::string Driver() override { return "mysql"; }
  virtual std::string Engine() override { return "mysql"; }

  

 private:
  void MysqlSetOption(mysql_option opton, const void* arg);
  ConnectionInfo connection_info_;
  MYSQL* native_connection_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_CONNECTION_H_
