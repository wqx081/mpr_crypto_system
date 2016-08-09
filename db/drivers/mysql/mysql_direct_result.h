#ifndef DB_DRIVERS_MYSQL_MYSQL_DIRECT_RESULT_H_
#define DB_DRIVERS_MYSQL_MYSQL_DIRECT_RESULT_H_
#include "db/drivers/mysql/common.h"
#include "db/backend/db_result.h"

#include "base/numbers.h"

namespace db {

class MysqlDirectResult : public DBResult {
 public:

  MysqlDirectResult(MYSQL* connection)
    : native_result_(nullptr),
      columns_(0),
      current_row_(0),
      native_row_(nullptr) {
    native_result_ = ::mysql_store_result(connection);
    if (!native_result_) {
      columns_ = ::mysql_field_count(connection);
      DCHECK(columns_ != 0) << "query does not produce any result";
    } else {
      columns_ = ::mysql_num_fields(native_result_);
    }
  }
      
  const char* At(int col) {
    DCHECK(native_result_);
    DCHECK(col >= 0 && col < columns_);
    return native_row_[col];
  }

  const char* At(int col, size_t& len) {
    DCHECK(native_result_);
    DCHECK(col > 0 && col < columns_);
    uint64_t* lengths = ::mysql_fetch_lengths(native_result_);
    DCHECK(lengths);
    len = lengths[col];
    return native_row_[col];
  }

  template<typename T>
  bool DoFetch(int col, T& v) {
    size_t len;
    const char* str = At(col, len);
    if (!str) {
      return false;
    }
    T t;
    bool ret = base::StringAsValue<T>(std::string(str, len), &t);
    v = t;
    return ret;
  }

  virtual NextRowStatus HasNext() override;
  virtual bool Next() override;

  virtual bool Fetch(int col, int16_t& v) override; 
  virtual bool Fetch(int col, uint16_t& v) override; 
  virtual bool Fetch(int col, int32_t& v) override;  
  virtual bool Fetch(int col, uint32_t& v) override; 
  virtual bool Fetch(int col, int64_t& v) override; 
  virtual bool Fetch(int col, uint64_t& v) override; 
  virtual bool Fetch(int col, float& v) override; 
  virtual bool Fetch(int col, double& v) override; 
  virtual bool Fetch(int col, std::string& v) override;
  virtual bool Fetch(int col, base::Time* v) override;
  virtual bool Fetch(int col, std::ostream& v) override;

  virtual bool IsNull(int col) override;
  virtual int Columns() override;
  virtual std::string ColumnToName(int col) override;
  virtual int NameToColumn(const std::string& name) override;

 private:
  ~MysqlDirectResult() {
    if (native_result_) {
      ::mysql_free_result(native_result_);
    }
  }

  MYSQL_RES* native_result_;
  int columns_;
  uint32_t current_row_;
  MYSQL_ROW native_row_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_DIRECT_RESULT_H_
