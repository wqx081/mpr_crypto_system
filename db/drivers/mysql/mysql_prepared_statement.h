#ifndef DB_DRIVERS_MYSQL_MYSQL_PREPARED_STATEMENT_H_
#define DB_DRIVERS_MYSQL_MYSQL_PREPARED_STATEMENT_H_
#include "db/drivers/mysql/common.h"
#include "db/backend/db_statement.h"
#include "db/drivers/mysql/mysql_prepared_result.h"

namespace db {

class MysqlPreparedStatement : public DBStatement {
 private:
  struct Param {
    my_bool is_null;
    bool is_blob;
    uint64_t length;
    std::string value;
    void* buffer;

    Param() : is_null(true), is_blob(false), length(0), buffer(nullptr) {}

    void Set(const char* begin, const char* end, bool blob=false) {
      length = end - begin;
      buffer = const_cast<char *>(begin);
      is_blob = blob;
      is_null = false;
    }
    void SetString(const std::string& str) {
      value = str;
      buffer = const_cast<char *>(value.c_str());
      length = value.size();
      is_null = false;
    }
    void Set(const base::Time& v) {
      SetString(v.Format());
    }
    void BindIt(MYSQL_BIND* bind) {
      bind->is_null = &is_null;
      if (!is_null) {
        bind->buffer_type = is_blob ? MYSQL_TYPE_BLOB : MYSQL_TYPE_STRING;
	    bind->buffer = buffer;
	    bind->buffer_length = length;
	    bind->length = &length;
      } else {
        bind->buffer_type = MYSQL_TYPE_NULL;
      }
    }
  };

 public:
  MysqlPreparedStatement(const std::string& query, MYSQL* connection);
  virtual ~MysqlPreparedStatement() override;


  virtual const std::string& SqlQuery() override;
  virtual void Bind(int col, const std::string& str) override;
  virtual void Bind(int col, const char* str) override;
  virtual void Bind(int col, const char* begin, const char* end) override;
  virtual void Bind(int col, const base::Time& v) override;
  virtual void Bind(int col, std::istream& v) override;

  template<typename T>
  void DoBind(int col, T v) {
    fmt_.str(std::string());
    if (!std::numeric_limits<T>::is_integer) {
      fmt_ << std::setprecision(std::numeric_limits<T>::digits10 + 1);
    }
    fmt_ << v;
    At(col).SetString(fmt_.str());
  }

  virtual void Bind(int col, int32_t v) override;
  virtual void Bind(int col, uint32_t v) override;
  virtual void Bind(int col, uint64_t v) override;
  virtual void Bind(int col, int64_t v) override;
  virtual void Bind(int col, double v) override;

  virtual void BindNull(int col) override;
  virtual int64_t SequenceLast(const std::string& ) override;
  virtual uint64_t Affected() override;
  
  void BindAll() {
    if (!params_.empty()) {
      for (unsigned i = 0; i < params_.size(); ++i) {
        params_[i].BindIt(&bind_[i]);
      }

      DCHECK(::mysql_stmt_bind_param(native_statement_,
			             &bind_.front()) == 0);
    }
  }

  virtual MysqlPreparedResult* Query() override;
  virtual void Execute() override;
  virtual void Reset() override;

  void ResetData() {
    params_.resize(0);
    params_.resize(params_count_);
    bind_.resize(0);
    bind_.resize(params_count_, MYSQL_BIND());
  }

 private:
  Param& At(int col) {
    DCHECK(col >= 1 && col <= params_count_);
    return params_[col - 1];
  }  
  std::ostringstream fmt_;
  std::vector<Param> params_;
  std::vector<MYSQL_BIND> bind_;
  std::string query_;
  MYSQL_STMT* native_statement_;
  int params_count_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_PREPARED_STATEMENT_H_
