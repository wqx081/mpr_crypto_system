#ifndef DB_DRIVERS_MYSQL_MYSQL_DIRECT_STATEMENT_H_
#define DB_DRIVERS_MYSQL_MYSQL_DIRECT_STATEMENT_H_
#include "db/drivers/mysql/mysql_direct_result.h"
#include "db/backend/db_statement.h"

namespace db {

class MysqlDirectStatement : public DBStatement {
 public:
  virtual const std::string& SqlQuery() override;

  virtual void Bind(int col, const base::StringPiece& str) override;
  virtual void Bind(int col, const char* b, const char* e) override;
  virtual void Bind(int col, const base::Time& v) override;
  virtual void Bind(int col, std::istream& v) override;
  
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

  virtual void Bind(int col, int v) override;
  virtual void Bind(int col, unsigned v) override;
  virtual void Bind(int col, long v) override;
  virtual void Bind(int col, unsigned long v) override;
  virtual void Bind(int col, long long v) override;
  virtual void Bind(int col, unsigned long long v) override;
  virtual void Bind(int col, double v) override;
  virtual void Bind(int col, long double v) override;

  virtual void BindNull(int col) override;
  virtual long long SequenceLast(const base::StringPiece&) override;
  virtual unsigned long long Affected() override;

  void BindAll(std::string& real_query);

  virtual MysqlDirectResult* Query() override;
  virtual void Execute() override;

  std::string& At(int col) {
    if (col < 1 && col > params_no_) {
      throw InvalidPlaceholder();
    }
    return params_[col - 1];
  }

  void ResetParams();
  MysqlDirectStatement(const std::string& query, MYSQL* conn);

  virtual ~MysqlDirectStatement() override {}
  virtual void Reset() override {}

 private:
  std::ostringstream fmt_;
  std::vector<std::string> params_;
  std::vector<size_t> binders_;

  std::string query_;
  MYSQL* native_connection_;
  int params_no_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_DIRECT_STATEMENT_H_
