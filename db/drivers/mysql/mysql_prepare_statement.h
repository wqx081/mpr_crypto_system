#ifndef DB_DRIVERS_MYSQL_MYSQL_PREPARE_STATEMENT_H_
#define DB_DRIVERS_MYSQL_MYSQL_PREPARE_STATEMENT_H_
#include "db/drivers/mysql/common.h"
#include "db/backend/db_statement.h"
#include "db/drivers/mysql/mysql_prepare_result.h"

namespace db {

class MysqlPrepareStatement : public DBStatement {
 private:
  struct Param {
    Param() : is_null(1), is_blob(false), length(0), buffer(nullptr) {}

    void Set(const char* begin, const char* end, bool blob=false) {
      length = end - begin;
      buffer = const_cast<char *>(begin);
      is_blob = blob;
      is_null = 0;
    }

    void SetString(const base::StringPiece& str) {
      value = str.as_string();
      buffer = const_cast<char *>(value.c_str());
      length = value.size();
      is_null = 0;
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

    my_bool is_null;
    bool is_blob;
    unsigned long length;
    std::string value;
    void* buffer;
  };

 public:
  virtual const std::string& SqlQuery() override;
  virtual void Bind(int col, const base::StringPiece&) override;
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

  virtual void Bind(int col, int v) override;  
  virtual void Bind(int col, unsigned int v) override;  
  virtual void Bind(int col, long v) override;  
  virtual void Bind(int col, unsigned long v) override;  
  virtual void Bind(int col, double v) override;  
  virtual void Bind(int col, long double v) override;  
  virtual void Bind(int col, long long v) override;  
  virtual void Bind(int col, unsigned long long v) override;  

  virtual void BindNull(int col) override;
  virtual long long SequenceLast(const base::StringPiece&) override;
  virtual unsigned long long Affected() override;
  
  void BindAll() {
    if (!params_.empty()) {
      for (unsigned int i = 0; i < params_.size(); ++i) {
        params_[i].BindIt(&native_bind_[i]);
      }
      if (::mysql_stmt_bind_param(native_statement_, &native_bind_.front())) {
        throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
      }
    }
  }
  
  virtual MysqlPrepareResult* Query() override;
  virtual void Execute() override;

  
  MysqlPrepareStatement(const std::string& q, MYSQL* conn);
  virtual ~MysqlPrepareStatement() override;
  void ResetData();
  virtual void Reset();

 private:
  Param& At(int col) {
    if (col < 1 || col > params_count_) {
      throw InvalidPlaceholder();
    }
    return params_[col - 1];
  }

  std::ostringstream fmt_;
  std::vector<Param> params_;
  std::vector<MYSQL_BIND> native_bind_;
  std::string query_;
  MYSQL_STMT* native_statement_;
  int params_count_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_PREPARE_STATEMENT_H_
