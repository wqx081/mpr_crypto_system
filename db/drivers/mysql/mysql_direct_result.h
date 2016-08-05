#ifndef DB_DRIVERS_MYSQL_MYSQL_DIRECT_RESULT_H_
#define DB_DRIVERS_MYSQL_MYSQL_DIRECT_RESULT_H_
#include "db/drivers/mysql/common.h"
#include "db/backend/db_result.h"
#include "db/common/numeric_util.h"
#include "base/time.h"

namespace db {

class MysqlDirectResult : public DBResult {
 public:
  virtual NextRowStatus HasNext() override;
  virtual bool Next() override;

  const char* At(int col);
  const char* At(int col, size_t& len);

  template<typename T>
  bool DoFetch(int col, T& v) {
    size_t len;
    const char* sp = At(col, len);
    if (!sp) {
      return false;
    } 
    v = ParseNumber<T>(std::string(sp, len), fmt_);
    return true;
  }

  // API
  virtual bool Fetch(int col, short& v) override;
  virtual bool Fetch(int col, unsigned short& v) override;
  virtual bool Fetch(int col, int& v) override;
  virtual bool Fetch(int col, unsigned int& v) override;
  virtual bool Fetch(int col, long& v) override;
  virtual bool Fetch(int col, unsigned long& v) override;
  virtual bool Fetch(int col, long long& v) override;
  virtual bool Fetch(int col, unsigned long long& v) override;
  virtual bool Fetch(int col, float& v) override;
  virtual bool Fetch(int col, double& v) override;
  virtual bool Fetch(int col, long double& v) override;
  virtual bool Fetch(int col, std::string& v) override;
  virtual bool Fetch(int col, std::ostream& v) override;
  virtual bool Fetch(int col, base::Time* v) override;

  virtual bool IsNull(int col) override;
  virtual int Columns() override;
  virtual std::string ColumnToName(int col) override;
  virtual int NameToColumn(const base::StringPiece& name) override;

  MysqlDirectResult(MYSQL* conn)
    : native_result_(nullptr),
      columns_(0),
      current_row_(0),
      native_row_(nullptr) {
    fmt_.imbue(std::locale::classic());
    native_result_ = ::mysql_store_result(conn);
    if (!native_result_) {
      columns_ = ::mysql_field_count(conn);
      if (columns_ == 0) {
        throw mysql_backend::MyException("Seems that the query does not produce any result");
      } 
    } else {
      columns_ = ::mysql_num_fields(native_result_);
    }
  }

  ~MysqlDirectResult() {
    if (native_result_) {
      ::mysql_free_result(native_result_);
    }
  }

 private:
  std::istringstream fmt_;
  MYSQL_RES* native_result_;
  int columns_;
  unsigned current_row_;
  MYSQL_ROW native_row_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_DIRECT_RESULT_H_
