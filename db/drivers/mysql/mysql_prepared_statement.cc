#include "db/drivers/mysql/mysql_prepared_statement.h"

namespace db {

MysqlPreparedStatement::MysqlPreparedStatement(const std::string& query, 
		MYSQL* connection)
  : query_(query),
    native_statement_(nullptr),
    params_count_(0) {

  fmt_.imbue(std::locale::classic());    

  native_statement_ = ::mysql_stmt_init(connection);
  DCHECK(native_statement_);
  DCHECK(::mysql_stmt_prepare(native_statement_,
			      query.c_str(),
			      query.size()) == 0);
  params_count_ = ::mysql_stmt_param_count(native_statement_);
  ResetData();
}

MysqlPreparedStatement::~MysqlPreparedStatement() {
  ::mysql_stmt_close(native_statement_);
}

void MysqlPreparedStatement::Reset() {
  ResetData();
  ::mysql_stmt_reset(native_statement_);
}

const std::string& MysqlPreparedStatement::SqlQuery() {
  return query_;
}

void MysqlPreparedStatement::Bind(int col, const std::string& str) {
  At(col).Set(str.c_str(), str.c_str() + str.size());
}

void MysqlPreparedStatement::Bind(int col, const char* str) {
  At(col).Set(str, str + strlen(str));
}

void MysqlPreparedStatement::Bind(int col, 
		                  const char* begin, 
				  const char* end) {
  At(col).Set(begin, end);
}

void MysqlPreparedStatement::Bind(int col, const base::Time& v) {
  At(col).Set(v);
}

void MysqlPreparedStatement::Bind(int col, std::istream& v) {
  std::ostringstream ss;
  ss << v.rdbuf();
  At(col).SetString(ss.str());
  At(col).is_blob = true;
}

void MysqlPreparedStatement::Bind(int col, int32_t v) {
  DoBind(col, v);
}

void MysqlPreparedStatement::Bind(int col, uint32_t v) {
  DoBind(col, v);
}

void MysqlPreparedStatement::Bind(int col, int64_t v) {
  DoBind(col, v);
}

void MysqlPreparedStatement::Bind(int col, uint64_t v) {
  DoBind(col, v);
}

void MysqlPreparedStatement::Bind(int col, double v) {
  DoBind(col, v);
}

void MysqlPreparedStatement::BindNull(int col) {
  At(col) = Param();
}

int64_t MysqlPreparedStatement::SequenceLast(const std::string& /**/) {
  return ::mysql_stmt_insert_id(native_statement_);
}

uint64_t MysqlPreparedStatement::Affected() {
  return ::mysql_stmt_affected_rows(native_statement_);
}

MysqlPreparedResult* MysqlPreparedStatement::Query() {
  BindAll();
  DCHECK(::mysql_stmt_execute(native_statement_) == 0);
  return new MysqlPreparedResult(native_statement_);
}

void MysqlPreparedStatement::Execute() {
  BindAll();
  DCHECK(::mysql_stmt_execute(native_statement_) == 0);
  DCHECK(::mysql_stmt_store_result(native_statement_) == 0);
  MYSQL_RES* r = ::mysql_stmt_result_metadata(native_statement_);
  DCHECK(r);
}

} // namespace db
