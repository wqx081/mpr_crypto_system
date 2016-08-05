#ifndef DB_DRIVERS_MYSQL_CONNECTION_H_
#define DB_DRIVERS_MYSQL_CONNECTION_H_

#include "db/drivers/mysql/common.h"
#include "db/drivers/mysql/mysql_result.h"
#include "db/drivers/mysql/mysql_statement.h"
#include "db/exception.h"
#include "base/string_piece.h"

extern "C" {
db::backend::Connection* DbMysqlGetConnection(const db::ConnectionInfo& connection_info);
}

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
    std::string database = connection_info.Get("database", "");
    const char* pdatabase = database.empty() ? nullptr : database.c_str();
    int port = connection_info.Get("port", 0);
    std::string unix_socket = connection_info.Get("unix_socket", "");
    const char* punix_socket = unix_socket.empty() ? nullptr : unix_socket.c_str();
   
    std::string default_auth = connection_info.Get("default_auth", "");
    if (!default_auth.empty()) {
      MysqlSetOption(MYSQL_DEFAULT_AUTH, default_auth.c_str());
    }

    std::string init_command = connection_info.Get("init_command", "");
    if (!init_command.empty()) {
      MysqlSetOption(MYSQL_INIT_COMMAND, init_command.c_str());
    }

    if (connection_info.Has("opt_compress")) {
      if (connection_info.Get("opt_compress", 1)) {
        MysqlSetOption(MYSQL_OPT_COMPRESS, nullptr);
      }
    }

    if (connection_info.Has("opt_connect_timeout")) {
      unsigned connect_timeout = connection_info.Get("opt_connect_timeout", 0);
      if (connect_timeout) {
        MysqlSetOption(MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);
      }
    }
    if (connection_info.Has("opt_guess_connection")) {
      if (connection_info.Get("opt_guess_connection", 1)) {
        MysqlSetOption(MYSQL_OPT_GUESS_CONNECTION, nullptr);
      }
    }

    //Real connection
    if (!::mysql_real_connect(conn_, 
                              phost, 
                              puser, 
                              ppass, 
                              pdatabase, 
                              port, 
                              punix_socket, 
                              0)) {
      std::string err = "unknown";
      try {
        err = ::mysql_error(conn_);
      } catch (...) {
      }
      ::mysql_close(conn_);
      throw MyException(err);
    }
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

  virtual void Commit() override {
    Execute("COMMIT");
  }

  virtual void Rollback() override {
    try {
      Execute("ROLLBACK");
    } catch (...) {
    }
  }

  virtual backend::Statement* PrepareStatement(const std::string& query) override {
    return new PreMysqlStatement(query, conn_);
  }

  virtual backend::Statement* NewStatement(const std::string& query) override {
    return new UnPreMysqlStatement(query, conn_);
  }

  virtual std::string Escape(const base::StringPiece& str) override {
    return Escape(str.data(), str.data() + str.size());
  }

  virtual std::string Escape(const char* b, const char* e) override {
    std::vector<char> buf(2 * (e - b) + 1);
    size_t len = ::mysql_real_escape_string(conn_, &buf.front(), b, e - b);
    std::string result;
    result.assign(&buf.front(), len);
    return result;
  }

  virtual std::string Driver() override {
    return "mysql";
  }  

  virtual std::string Engine() override {
    return "mysql";
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

#endif
