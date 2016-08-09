#ifndef DB_DRIVERS_MYSQL_MYSQL_PREPARED_RESULT_H_
#define DB_DRIVERS_MYSQL_MYSQL_PREPARED_RESULT_H_
#include "db/drivers/mysql/common.h"
#include "db/backend/db_result.h"
#include "base/numbers.h"

namespace db {

class MysqlPreparedResult : public DBResult {
 private:
  struct BindData {
    static const int kBindDataBufferLength = 128;
    char buf[kBindDataBufferLength];
    std::vector<char> vbuf;
    char* ptr;
    uint64_t length;
    my_bool is_null;
    my_bool error;

    BindData() : ptr(nullptr), length(0), is_null(false), error(false) {}
  };

 public:
  MysqlPreparedResult(MYSQL_STMT* statement);
  ~MysqlPreparedResult();
  void Reset();

  virtual NextRowStatus HasNext() override;
  virtual bool Next() override;

  BindData& At(int col) {
    DCHECK(col >= 0 && col < columns_);
    DCHECK(!bind_data_.empty());
    return bind_data_.at(col);
  }

  template<typename T>
  bool DoFetch(int col, T& v) {
    T t;
    bool ret;
    BindData& bind_data = At(col);
    if (bind_data.is_null) {
      return false;
    }
    ret = base::StringAsValue<T>(std::string(bind_data.ptr, bind_data.length),
		                 &t);
    v = t;
    return ret;
  }

  virtual bool Fetch(int col, int16_t& v) override;
  virtual bool Fetch(int col, uint16_t& v) override;
  virtual bool Fetch(int col, int32_t& v) override;
  virtual bool Fetch(int col, uint32_t& v) override;
  virtual bool Fetch(int col, int64_t& v) override;
  virtual bool Fetch(int col, uint64_t& v) override;
  virtual bool Fetch(int col, float& v) override;
  virtual bool Fetch(int col, double& v) override;
  virtual bool Fetch(int col, std::string& v) override;
  virtual bool Fetch(int col, std::ostream& v) override;
  virtual bool Fetch(int col, base::Time* v) override;

  virtual bool IsNull(int col) override;
  virtual int Columns() override;
  virtual std::string ColumnToName(int col) override;
  virtual int NameToColumn(const std::string& name) override;

 private:
  int columns_;
  MYSQL_STMT* native_statement_;
  uint32_t current_row_;
  MYSQL_RES* meta_;
  std::vector<MYSQL_BIND> bind_;
  std::vector<BindData> bind_data_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_PREPARED_RESULT_H_
