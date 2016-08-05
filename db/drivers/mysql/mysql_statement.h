#ifndef DB_DRIVERS_MYSQL_STATEMENT_H_
#define DB_DRIVERS_MYSQL_STATEMENT_H_

#include "db/drivers/mysql/common.h"
#include "db/drivers/mysql/mysql_result.h"

namespace db {
namespace mysql_backend {

class UnPreMysqlStatement : public backend::Statement {
 public:
  virtual const std::string& SqlQuery() override {
    return query_;
  }
  virtual void Bind(int col, const std::string& s) override {
    Bind(col, s.c_str(), s.c_str() + s.size());
  }

  virtual void Bind(int col, const char* s) override {
    Bind(col, s, s + ::strlen(s));
  }

  virtual void Bind(int col, const char* b, const char* e) override {
    std::vector<char> buf(2 * (e - b) + 1);
    size_t len = ::mysql_real_escape_string(conn_,
		                            &buf.front(),
					    b,
					    e - b);
    std::string& str = At(col);
    str.clear();
    str.reserve(e - b + 2);
    str += '\'';
    str.append(&buf.front(), len);
    str += '\'';
  }

  virtual void Bind(int col, const base::Time& v) override {
    std::string& str = At(col);
    str.clear();
    str.reserve(30);
    str += '\'';
    str += v.Format();
    str += '\'';
  }

  virtual void Bind(int col, std::istream& v) override {
    std::ostringstream ss;
    ss << v.rdbuf();
    std::string str = ss.str();
    Bind(col, str);
  }

  template<typename T>
  void DoBind(int col, T v) {
    fmt_.str(std::string());
    if (!std::numeric_limits<T>::is_integer) {
      fmt_ << std::setprecision(std::numeric_limits<T>::digits10 + 1);
    }
    fmt_ << v;
    std::string str = fmt_.str();
    At(col).swap(str);
  }

  virtual void Bind(int col, int v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, unsigned v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, unsigned long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, long long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, unsigned long long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, double v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, long double v) override {
    DoBind(col, v);
  }

  virtual void BindNull(int col) override {
    At(col) = "NULL";
  }
  virtual long long SequenceLast(const std::string& /* sequence */)  override 
    {
    return ::mysql_insert_id(conn_);   
  }

  virtual unsigned long long Affected() override {
    return ::mysql_affected_rows(conn_);
  }

  void BindAll(std::string& real_query) {
    size_t total = query_.size();
    for (unsigned i = 0; i < params_.size(); ++i) {
      total += params_[i].size();
    }
    real_query.clear();
    real_query.reserve(total);
    size_t pos = 0;
    for (unsigned i = 0; i < params_.size(); ++i) {
      size_t marker = binders_[i];
      real_query.append(query_, pos, marker - pos);
      pos = marker + 1;
      real_query.append(params_[i]);
    }

    real_query.append(query_, pos, std::string::npos);
  }

  virtual UnPreMysqlResult* Query() override {
    std::string real_query;
    BindAll(real_query);
    ResetParams();
    if (::mysql_real_query(conn_,
                           real_query.c_str(),
			   real_query.size())) {
      throw MyException(::mysql_error(conn_));
    }
    return new UnPreMysqlResult(conn_);
  }

  virtual void Execute() override {
    std::string real_query;
    BindAll(real_query);
    ResetParams();
    if (::mysql_real_query(conn_,
                           real_query.c_str(),
			   real_query.size())) {
      throw MyException(::mysql_error(conn_));
    }

    MYSQL_RES* res = ::mysql_store_result(conn_);
    if (res) {
      ::mysql_free_result(res);
      throw MyException("Calling execute() on query");
    }
  }

  std::string& At(int col) {
    if (col < 1 || col > params_no_) {
      throw InvalidPlaceholder();
    }
    return params_[col - 1];
  }

  void ResetParams() {
    params_.clear();
    params_.resize(params_no_, "NULL");
  }

  virtual ~UnPreMysqlStatement() override {}
  virtual void Reset() override {}

  UnPreMysqlStatement(const std::string& query, MYSQL* conn)
      : query_(query),
	conn_(conn),
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
      throw MyException("unterminated string found in query");
    }

    ResetParams();
  }

 private:
  std::ostringstream fmt_;
  std::vector<std::string> params_;
  std::vector<size_t> binders_;

  std::string query_;
  MYSQL* conn_;
  int params_no_;
};

/////
class PreMysqlStatement : public backend::Statement {
 private:
  struct Param {
    Param() : is_null(true), is_blob(false), length(0), buffer(nullptr) {}

    void Set(const char* b, const char* e, bool blob=false) {
      length = e - b;
      buffer = const_cast<char *>(b);
      is_blob = blob;
      is_null = 0;
    }

    void SetString(const std::string& str) {
      value = str;
      buffer = const_cast<char *>(value.c_str());
      length = value.size();
      is_null = 0;
    }
    void Set(const base::Time& t) {
      SetString(t.Format());
    }
    void BindIt(MYSQL_BIND* bnd) {
      bnd->is_null = &is_null;
      if (!is_null) {
        bnd->buffer_type = is_blob ? MYSQL_TYPE_BLOB 
		                   : MYSQL_TYPE_STRING;
	bnd->buffer = buffer;
	bnd->buffer_length = length;
	bnd->length = &length;
      } else {
        bnd->buffer_type = MYSQL_TYPE_NULL;
      }
    }
    my_bool is_null;
    bool is_blob;
    unsigned long length;
    std::string value;
    void* buffer;
  };

 public:
  // APIs
  virtual const std::string& SqlQuery() override {
    return query_;
  }

  virtual void Bind(int col, const std::string& str) override {
    At(col).Set(str.c_str(), str.c_str() + str.size());
  }

  virtual void Bind(int col, const char* str) override {
    At(col).Set(str, str + ::strlen(str));
  }

  virtual void Bind(int col, const char* b, const char* e) override {
    At(col).Set(b, e);
  }

  virtual void Bind(int col, const base::Time& t) override {
    At(col).Set(t);
  }

  virtual void Bind(int col, std::istream& v) override {
    std::ostringstream ss;
    ss << v.rdbuf();
    At(col).SetString(ss.str());
    At(col).is_blob = true;
  }

  template<typename T>
  void DoBind(int col, T v) {
    fmt_.str(std::string());
    if (!std::numeric_limits<T>::is_integer) {
      fmt_<< std::setprecision(std::numeric_limits<T>::digits10 + 1);
    }
    fmt_ << v;
    At(col).SetString(fmt_.str());
  }

  virtual void Bind(int col, int v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, unsigned v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, unsigned long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, long long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, unsigned long long v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, double v) override {
    DoBind(col, v);
  }
  virtual void Bind(int col, long double v) override {
    DoBind(col, v);
  }

  virtual void BindNull(int col) override {
    At(col) = Param();
  }

  virtual long long SequenceLast(const std::string& 
      /*sequence*/) override {
    return ::mysql_stmt_insert_id(stmt_);
  }

  virtual unsigned long long Affected() override {
    return ::mysql_stmt_affected_rows(stmt_);
  }

  void BindAll() {
    if (!params_.empty()) {
      for (unsigned int i = 0; i < params_.size(); ++i) {
        params_[i].BindIt(&bind_[i]);
      }

      if (::mysql_stmt_bind_param(stmt_, &bind_.front())) {
        throw MyException(::mysql_stmt_error(stmt_));
      }
    }
  }

  virtual PreMysqlResult* Query() override {
    BindAll();
    if (::mysql_stmt_execute(stmt_)) {
      throw MyException(::mysql_stmt_error(stmt_));
    }
    return new PreMysqlResult(stmt_);
  }

  virtual void Execute() override {

    BindAll();
    if (::mysql_stmt_execute(stmt_)) {
      throw MyException(::mysql_stmt_error(stmt_));
    }
    if (::mysql_stmt_store_result(stmt_)) {
      throw MyException(::mysql_stmt_error(stmt_));
    }

    MYSQL_RES* res = ::mysql_stmt_result_metadata(stmt_);
    if (res) {
      ::mysql_free_result(res);
      throw MyException("Calling execute() on query!");
    }
  }

  // 
  PreMysqlStatement(const std::string& query, MYSQL* conn)
      : query_(query),
	stmt_(nullptr),
	params_count_(0) {

    fmt_.imbue(std::locale::classic());
    stmt_ = ::mysql_stmt_init(conn);
    try {
      if (!stmt_) {
        throw MyException(" Failed to create a statement");
      }
      if (::mysql_stmt_prepare(stmt_, query.c_str(), query.size())) {
        throw MyException(::mysql_stmt_error(stmt_));
      }
      params_count_ = ::mysql_stmt_param_count(stmt_);
      ResetData();
    } catch(...) {
      if (stmt_) {
        ::mysql_stmt_close(stmt_);
      }
      throw;
    }
  }

  virtual ~PreMysqlStatement() override {
    ::mysql_stmt_close(stmt_);
  }

  virtual void Reset() override {
    ResetData();
    ::mysql_stmt_reset(stmt_);
  }

  void ResetData() {
    params_.resize(0);
    params_.resize(params_count_);
    bind_.resize(0);
    bind_.resize(params_count_, MYSQL_BIND());
  }

 private:
  Param& At(int col);

  std::ostringstream fmt_;
  std::vector<Param> params_;
  std::vector<MYSQL_BIND> bind_;
  std::string query_;
  MYSQL_STMT* stmt_;
  int params_count_;
};


} // namespace mysql_backend
} // namespace db
#endif
