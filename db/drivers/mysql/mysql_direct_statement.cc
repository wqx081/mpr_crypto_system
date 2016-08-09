#include "db/drivers/mysql/mysql_direct_statement.h"
#include "db/drivers/mysql/mysql_direct_result.h"
#include "base/time.h"

namespace db {

MysqlDirectStatement::MysqlDirectStatement(const std::string& query, 
                                           MYSQL* connection)
  : query_(query),
    native_connection_(connection),
    params_no_(0) {
  fmt_.imbue(std::locale::classic());
  bool inside_text = false;
  for (size_t i = 0; i < query_.size(); ++i) {
    if (query_[i] == '\'') {
      inside_text = ! inside_text;
    }
    if (query_[i] == '?' && !inside_text) {
      params_no_++;
      binders_.push_back(i);
    }
  }
  DCHECK(inside_text != true);
  ResetParams();
}

MysqlDirectStatement::~MysqlDirectStatement() {}

const std::string& MysqlDirectStatement::SqlQuery() {
  return query_;
}

void MysqlDirectStatement::Bind(int col, const std::string& v) {
  Bind(col, v.c_str(), v.c_str() + v.size());
}

void MysqlDirectStatement::Bind(int col,char const *s) {
  Bind(col,s,s+strlen(s));
}

void MysqlDirectStatement::Bind(int col,char const *b,char const *e) {
  std::vector<char> buf(2*(e-b)+1);
  size_t len = mysql_real_escape_string(native_connection_,&buf.front(),b,e-b);
  std::string &s=At(col);
  s.clear();
  s.reserve(e-b+2);
  s+='\'';
  s.append(&buf.front(),len);
  s+='\'';
}

void MysqlDirectStatement::Bind(int col, const base::Time& v) {
  std::string& s = At(col);
  s.clear();
  s.reserve(30);
  s += '\'';
  s += v.Format();
  s += '\'';
}

void MysqlDirectStatement::Bind(int col,std::istream &v) {
  std::ostringstream ss;
  ss << v.rdbuf();
  std::string tmp=ss.str();
  Bind(col,tmp);
}

void MysqlDirectStatement::Bind(int col, int32_t v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, uint32_t v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, int64_t v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, uint64_t v) {
  DoBind(col, v);
}
void MysqlDirectStatement::Bind(int col, double v) {
  DoBind(col, v);
}

void MysqlDirectStatement::BindNull(int col) {
  At(col) = "NULL";
}

int64_t MysqlDirectStatement::SeuqnceLast(const std::string& /**/) {
  return ::mysql_insert_id(native_connection_);
}

uint64_t MysqlDirectStatement::Affected() {
  return ::mysql_affected_rows(native_connection_);
}

MysqlDirectResult* 
MysqlDirectStatement::Query() {
  std::string real_query;
  BindAll(real_query);
  DCHECK(::mysql_real_query(native_connection_,
                            real_query.c_str(),
                            real_query.size()) == 0);
  return new MysqlDirectResult(native_connection_);
}

void MysqlDirectStatement::Execute() {
  std::string real_query;
  BindAll(real_query);
  ResetParams();
  DCHECK(::mysql_real_query(native_connection_,
                            real_query.c_str(),
                            real_query.size()) == 0);
  MYSQL_RES* res = ::mysql_store_result(native_connection_);
  if (res) {
    ::mysql_free_result(res);
    // TODO: throw
  }
}

void MysqlDirectStatement::Reset() {}

} // namespace db

