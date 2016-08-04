#include "db/drivers/mysql/common.h"
#include "db/drivers/mysql/mysql_result.h"
#include "db/drivers/mysql/mysql_statement.h"
#include "db/exception.h"

namespace db {
namespace mysql_backend {

class MysqlConnection : public backend::Connection {
 public:
  MysqlConnection(const ConnectionInfo& connection_info)
      : backend::Connection(connection_info),
	conn_(nullptr) {

    conn_ = ::mysql_init(nullptr);
    if (!conn_) {
      throw DBException("db::mysql failed to create connection");
    }
    std::string host = connection_info.Get("host", "");
    const char* phost= host.empty() ? nullptr : host.c_str();
    std::string user = connection_info.Get("user", "");
    const char* puser= user.empty() ? nullptr : user.c_str();
    std::string pass = connection_info.Get("password", "");
    const char* ppass= pass.empty() ? nullptr : pass.c_str();
    //TODO
  }
  virtual ~MysqlConnection() override {
    ::mysql_close(conn_);
  }

  void Execute(const std::string& str) {
    if (::mysql_real_query(conn_, str.c_str(), str.size())) {
      throw MyException(::mysql_error(conn_));
    }
  }

  virtual void Begin() override {
    Execute("BEGIN");
  }

 private:
  void MysqlSetOption(::mysql_option option, const void* arg) {
    if (::mysql_options(conn_, option, static_cast<const char *>(arg))) {
      throw DBException("db::mysql failed to set option");
    }
  }

  ConnectionInfo connection_info_;
  MYSQL* conn_;
};

} // namespace mysql_backend
} // namespace db
