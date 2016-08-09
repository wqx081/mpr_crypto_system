#ifndef DB_DRIVERS_MYSQL_MYSQL_CONNECTION_H_
#define DB_DRIVERS_MYSQL_MYSQL_CONNECTION_H_
#include "db/drivers/mysql/common.h"
#include "db/backend/db_connection.h"
#include "db/common/connection_info.h"

namespace db {

class MysqlConnection : public DBConnection {
 public:
  MysqlConnection(const ConnectionInfo& info);
  ~MysqlConnection() {}
  
  void Execute(const std::string& str) {
    DCHECK(::mysql_real_query(native_connection_,
                              str.c_str(),
                              str.size()) == 0);
  }

  virtual void Begin() override;
  virtual void Commit() override;
  virtual void Rollback() override;
  virtual DBStatement* NewPreparedStatement(const std::string& query) override;
  virtual DBStatement* NewDirectStatement(const std::string& query) override;
  virtual std::string Escape(const std::string& query) override;
  virtual std::string Escape(const char* str) override;
  virtual std::string Escape(const char* begin, const char* end) override;

  virtual std::string Driver() override { return "mysql"; }
  virtual std::string Engine() override { return "mysql"; }

 private:
  void MysqlSetOption(::mysql_option option, const void* arg) {
    DCHECK(::mysql_options(native_connection_,
                           option,
                           static_cast<const char *>(arg)) == 0);
  }

  ConnectionInfo connection_info_;  
  MYSQL* native_connection_;
};

} // namespace db

#endif //DB_DRIVERS_MYSQL_MYSQL_CONNECTION_H_
