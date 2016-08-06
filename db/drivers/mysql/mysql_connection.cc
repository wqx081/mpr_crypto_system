#include "db/drivers/mysql/mysql_connection.h"
#include "db/drivers/mysql/mysql_direct_statement.h"
#include "db/drivers/mysql/mysql_prepare_statement.h"

namespace db {

MysqlConnection::MysqlConnection(const ConnectionInfo& info)
  : DBConnection(info),
    native_connection_(nullptr) {

  native_connection_ = ::mysql_init(nullptr);
  if (!native_connection_) {
    throw mysql_backend::MyException("db:mysql failed to create native connection");
  }

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

  // connecting
  if (!::mysql_real_connect(native_connection_,
                            phost,
                            puser,
                            ppassword,
                            pdatabase,
                            port,
                            punix_socket,
                            0)) {
    std::string msg("Unknown");
    try {
      msg = ::mysql_error(native_connection_);
    } catch (...) {
    }
    throw mysql_backend::MyException(msg);
  }
}

MysqlConnection::~MysqlConnection() {
  ::mysql_close(native_connection_);
}

void MysqlConnection::Execute(const base::StringPiece& s) {
  if (::mysql_real_query(native_connection_, s.data(), s.size())) {
    throw mysql_backend::MyException(::mysql_error(native_connection_));
  }
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

DBStatement* MysqlConnection::NewPrepareStatement(const std::string& query) {
  return new MysqlPrepareStatement(query, native_connection_);
}

DBStatement* MysqlConnection::NewDirectStatement(const std::string& query) {
  return new MysqlDirectStatement(query, native_connection_);
}

std::string MysqlConnection::Escape(const base::StringPiece& str) {
  return Escape(str.data(), str.data() + str.size());
}

std::string MysqlConnection::Escape(const char* begin, const char* end) {
  std::vector<char> buf(2 * (end - begin) + 1);
  size_t len = ::mysql_real_escape_string(native_connection_, &buf.front(), begin, end - begin);
  std::string result;
  result.assign(&buf.front(), len);
  return result;
}

void MysqlConnection::MysqlSetOption(mysql_option option, const void* arg) {
  if (::mysql_options(native_connection_, option, static_cast<const char *>(arg))) {
    throw mysql_backend::MyException("db::mysql failed to set option");
  }
}

} // namespace db

extern "C" {

db::DBConnection* db_mysql_get_connection(const db::ConnectionInfo& info) {
  return new db::MysqlConnection(info);
}

} // extern "C"
