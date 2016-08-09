#include "db/drivers/mysql/mysql_connection.h"
#include "db/drivers/mysql/mysql_direct_statement.h"
#include "db/drivers/mysql/mysql_prepared_statement.h"
#include "db/backend/db_connection.h"

namespace db {

MysqlConnection::MysqlConnection(const ConnectionInfo& info)
  : DBConnection(info),
    native_connection_(nullptr) {

  native_connection_ = ::mysql_init(nullptr);
  DCHECK(native_connection_);

  std::string host = info.Get("host","");
  char const *phost = host.empty() ? 0 : host.c_str();
  std::string user = info.Get("user","");
  char const *puser = user.empty() ? 0 : user.c_str();
  std::string password = info.Get("password","");
  char const *ppassword = password.empty() ? 0 : password.c_str();
  std::string database = info.Get("database","");
  char const *pdatabase = database.empty() ? 0 : database.c_str();
  int port = info.Get("port",0);
  std::string unix_socket = info.Get("unix_socket","");
  char const *punix_socket = unix_socket.empty() ? 0 : unix_socket.c_str();

  if(info.Has("opt_read_timeout")) {
    if(unsigned read_timeout = info.Get("opt_read_timeout", 0)) {
      MysqlSetOption(MYSQL_OPT_READ_TIMEOUT, &read_timeout);
    }
  }
  if(info.Has("opt_reconnect")) {
    if(unsigned reconnect = info.Get("opt_reconnect", 1)) {
      my_bool value = reconnect;
      MysqlSetOption(MYSQL_OPT_RECONNECT, &value);
    }
  }

  if(info.Has("opt_write_timeout")) {
    if(unsigned write_timeout = info.Get("opt_write_timeout", 0)) {
      MysqlSetOption(MYSQL_OPT_WRITE_TIMEOUT, &write_timeout);
    }
  }
  std::string set_charset_name = info.Get("set_charset_name", "");
  if(!set_charset_name.empty()) {
    MysqlSetOption(MYSQL_SET_CHARSET_NAME, set_charset_name.c_str());
  }


  DCHECK(::mysql_real_connect(native_connection_,
                              phost,
                              puser,
                              ppassword,
                              pdatabase,
                              port,
                              punix_socket,
                              0) != 0);
}

void MysqlConnection::Begin() {
  Execute("BEGIN");
}

void MysqlConnection::Commit() {
  Execute("COMMIT");
}

void MysqlConnection::Rollback() {
  try {
    Execute("ROLLBACK");
  } catch (...) {
  }
}

DBStatement*
MysqlConnection::NewDirectStatement(const std::string& query) {
  return new MysqlDirectStatement(query, native_connection_);
}

DBStatement*
MysqlConnection::NewPreparedStatement(const std::string& query) {
  return new MysqlPreparedStatement(query, native_connection_);
}

std::string MysqlConnection::Escape(const std::string& str) {
  return Escape(str.c_str(), str.c_str() + str.size());
}

std::string MysqlConnection::Escape(const char* str) {
  return Escape(str, str + strlen(str));
}

std::string MysqlConnection::Escape(const char* begin, const char* end) {
  std::vector<char> buf(2 * (end - begin) + 1);
  size_t len = ::mysql_real_escape_string(native_connection_, &buf.front(), begin, end - begin);
  std::string result;
  result.assign(&buf.front(), len);
  return result;
}

} // namespace db
