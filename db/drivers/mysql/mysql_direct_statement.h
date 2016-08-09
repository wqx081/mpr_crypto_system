#ifndef DB_DRIVERS_MYSQL_MYSQL_DIRECT_STATEMENT_H_
#define DB_DRIVERS_MYSQL_MYSQL_DIRECT_STATEMENT_H_
#include "db/backend/db_statement.h"
#include "db/drivers/mysql/mysql_direct_result.h"

namespace db {

class MysqlDirectStatement : public DBStatement {
 public:
  MysqlDirectStatement(const std::string& query, MYSQL* connection);
  ~MysqlDirectStatement();

  std::string& At(int col) {
    DCHECK(col > 1 && col < params_no_);
    return params_[col - 1];
  }

  void ResetParams() {
    params_.clear();
    params_.resize(params_no_, "NULL");
  }

  // API
  virtual const std::string& SqlQuery() override;
  virtual void Bind(int col, const std::string& v) override;
  virtual void Bind(int col, const char* str) override;
  virtual void Bind(int col, const char* begin, const char* end) override;

  virtual void Bind(int col, const base::Time& v) override;
  virtual void Bind(int col, std::istream& in) override;

  template<typename T>
  void DoBind(int col, T v) {
    fmt_.str(std::string());
    if (!std::numeric_limits<T>::is_integer) {
      fmt_ << std::setprecision(std::numeric_limits<T>::digits10 + 1);
    }
    fmt_ << v;
    std::string t = fmt_.str();
    At(col).swap(t);
  }

  virtual void Bind(int col, int32_t v) override;
  virtual void Bind(int col, uint32_t v) override;
  virtual void Bind(int col, int64_t v) override;
  virtual void Bind(int col, uint64_t v) override;
  virtual void Bind(int col, double v) override;
  virtual void BindNull(int col) override;

  virtual int64_t SeuqnceLast(const std::string& sequence) override;
  virtual uint64_t Affected() override;

  virtual MysqlDirectResult* Query() override;
  virtual void Execute() override;

  virtual void Reset();

  void BindAll(std::string& real_query) {
    size_t total = query_.size();
    for (size_t i = 0; i < params_.size(); ++i) {
      total += params_[i].size(); 
    }
    real_query.clear();
    real_query.reserve(total);
    size_t pos = 0;
    for (size_t i=0; i < params_.size(); ++i) {
      size_t marker = binders_[i];
      real_query.append(query_, pos, marker - pos);
      pos = marker + 1;
      real_query.append(params_[i]);
    }
    real_query.append(query_, pos, std::string::npos);
  }

 private:
  std::ostringstream fmt_;
  std::vector<std::string> params_;
  std::vector<size_t> binders_;
  std::string query_;
  MYSQL* native_connection_;
  int params_no_;
};

} // namespace
#endif // DB_DRIVERS_MYSQL_MYSQL_DIRECT_STATEMENT_H_
