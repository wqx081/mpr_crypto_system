#include "db/drivers/mysql/mysql_prepare_statement.h"

namespace db {

MysqlPrepareStatement::MysqlPrepareStatement(const std::string& query,
                                             MYSQL* conn)
  : query_(query),
    native_statement_(nullptr),
    params_count_(0) {

  fmt_.imbue(std::locale::classic());
  native_statement_ = ::mysql_stmt_init(conn);  
  try {
    if (!native_statement_) {
      throw mysql_backend::MyException("Failed to create a statement");
    }
    if (::mysql_stmt_prepare(native_statement_, query.c_str(), query.size())) {
      throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
    }
    params_count_ = ::mysql_stmt_param_count(native_statement_);
    ResetData();
  } catch (...) {
    if (native_statement_) {
      ::mysql_stmt_close(native_statement_);
    }
    throw;
  }
}

MysqlPrepareStatement::~MysqlPrepareStatement() {
  ::mysql_stmt_close(native_statement_);
}

void MysqlPrepareStatement::ResetData() {
  params_.resize(0);
  params_.resize(params_count_);
  native_bind_.resize(0);
  native_bind_.resize(params_count_, MYSQL_BIND());
}

void MysqlPrepareStatement::Reset() {
  ResetData();
  ::mysql_stmt_reset(native_statement_);
}

const std::string& MysqlPrepareStatement::SqlQuery() {
  return query_;
}

void MysqlPrepareStatement::Bind(int col, const base::StringPiece& str) {
  At(col).Set(str.data(), str.data() + str.size());
}

void MysqlPrepareStatement::Bind(int col, const char* begin, const char* end) {
  At(col).Set(begin, end);
}

void MysqlPrepareStatement::Bind(int col, const base::Time& t) {
  At(col).Set(t);
}

void MysqlPrepareStatement::Bind(int col, std::istream& v) {
  std::ostringstream ss;
  ss << v.rdbuf();
  At(col).SetString(ss.str());
  At(col).is_blob = true;
}

void MysqlPrepareStatement::Bind(int col, int v) {
  DoBind(col, v);
}
void MysqlPrepareStatement::Bind(int col, unsigned int v) {
  DoBind(col, v);
}
void MysqlPrepareStatement::Bind(int col, long v) {
  DoBind(col, v);
}
void MysqlPrepareStatement::Bind(int col, unsigned long v) {
  DoBind(col, v);
}
void MysqlPrepareStatement::Bind(int col, long long v) {
  DoBind(col, v);
}
void MysqlPrepareStatement::Bind(int col, unsigned long long v) {
  DoBind(col, v);
}
void MysqlPrepareStatement::Bind(int col, double v) {
  DoBind(col, v);
}
void MysqlPrepareStatement::Bind(int col, long double v) {
  DoBind(col, v);
}

void MysqlPrepareStatement::BindNull(int col) {
  At(col) = Param();
}

long long MysqlPrepareStatement::SequenceLast(const base::StringPiece& /* string */) {
  return ::mysql_stmt_insert_id(native_statement_);
}

unsigned long long MysqlPrepareStatement::Affected() {
  return ::mysql_stmt_affected_rows(native_statement_);
}

MysqlPrepareResult* MysqlPrepareStatement::Query() {
  BindAll();
  if (::mysql_stmt_execute(native_statement_)) {
    throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
  }
  return new MysqlPrepareResult(native_statement_);
}

void MysqlPrepareStatement::Execute() {
  BindAll();
  if (::mysql_stmt_execute(native_statement_)) {
    throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
  }
  if (::mysql_stmt_store_result(native_statement_)) {
    throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
  }
  MYSQL_RES* res = ::mysql_stmt_result_metadata(native_statement_);
  if (res) {
    ::mysql_free_result(res);
    throw mysql_backend::MyException("Calling execute() on query!");
  }
}

} // namespace db
