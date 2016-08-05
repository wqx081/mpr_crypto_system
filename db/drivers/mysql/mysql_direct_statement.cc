#include "db/drivers/mysql/mysql_direct_statement.h"

namespace db {

MysqlDirectStatement::MysqlDirectStatement(const std::string& query,
		                           MYSQL* conn)
  : query_(query),
    native_connection_(conn),
    params_no_(0) {

  fmt_.imbue(std::locale::classic());
  bool inside_text = false;
  for (size_t i = 0; i < query_.size(); ++i) {
    if (query_[i] == '\'') {
      inside_text = !inside_text;
    }
    if (query_[i] == '?' && !inside_text) {
      params_no_++;
      binders_.push_back(i);
    }
  }
  if (inside_text) {
    throw mysql_backend::MyException("Unterminated string found in query");
  }
  ResetParams();
}

const std::string& MysqlDirectStatement::SqlQuery() {
  return query_;
}

void MysqlDirectStatement::Bind(int col, const base::StringPiece& str) {
  Bind(col, str.data(), str.data() + str.size());
}

void MysqlDirectStatement::Bind(int col, const char* begin, const char* end) {
  std::vector<char> buf(2 * (end - begin) + 1);  
  size_t len = ::mysql_real_escape_string(native_connection_,
		                          &buf.front(),
					  begin,
					  end - begin);
  std::string& str = At(col);
  str.clear();
  str.reserve(end - begin + 2);
  str += '\'';
  str.append(&buf.front(), len);
  str += '\'';
}

void MysqlDirectStatement::Bind(int col, const base::Time& v) {
  std::string &str = At(col);
  str.clear();
  str.reserve(30);
  str += '\'';
  str += v.Format();
  str += '\'';
}

void MysqlDirectStatement::Bind(int col, std::istream& v) {
  std::ostringstream ss;
  ss << v.rdbuf();
  std::string str = ss.str();
  Bind(col, str);
}

void MysqlDirectStatement::Bind(int col, int v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, unsigned int v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, long v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, unsigned long v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, long long v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, unsigned long long v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, double v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, long double v) {
  DoBind(col, v);
}

void MysqlDirectStatement::BindNull(int col) {
  At(col) = "NULL";
}

long long MysqlDirectStatement::SequenceLast(const base::StringPiece& /**/) {
  return ::mysql_insert_id(native_connection_);  
}

unsigned long long MysqlDirectStatement::Affected() {
  return ::mysql_affected_rows(native_connection_);
}

void MysqlDirectStatement::BindAll(std::string& real_query) {
  size_t total = query_.size();
  for (unsigned i = 0; i < params_.size(); ++i) {
    total += params_[i].size();
  }
  real_query.clear();
  real_query.reserve(total);
  size_t pos = 0;
  for (unsigned int i = 0; i < params_.size(); ++i) {
    size_t marker = binders_[i];
    real_query.append(query_, pos, marker - pos);
    pos = marker + 1;
    real_query.append(params_[i]);
  }
  real_query.append(query_, pos, std::string::npos);
}

MysqlDirectResult* MysqlDirectStatement::Query() {
  std::string real_query;
  BindAll(real_query);
  ResetParams();
  if (::mysql_real_query(native_connection_,
                         real_query.c_str(),
			 real_query.size())) {
    throw mysql_backend::MyException(::mysql_error(native_connection_));
  }
  return new MysqlDirectResult(native_connection_);
}

void MysqlDirectStatement::Execute() {
  std::string real_query;
  BindAll(real_query);
  ResetParams();
  if (::mysql_real_query(native_connection_,
                         real_query.c_str(),
			 real_query.size())) {
    throw mysql_backend::MyException(::mysql_error(native_connection_));
  }
  MYSQL_RES* res = ::mysql_store_result(native_connection_);
  if (res) {
    ::mysql_free_result(res);
    throw mysql_backend::MyException("calling execute() on query!");
  }
}

void MysqlDirectStatement::ResetParams() {
  params_.clear();
  params_.resize(params_no_, "NULL");
}

} // namespace db
