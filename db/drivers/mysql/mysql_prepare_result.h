#ifndef DB_DRIVERS_MYSQL_MYSQL_PREPARE_RESULT_H_
#define DB_DRIVERS_MYSQL_MYSQL_PREPARE_RESULT_H_
#include "db/drivers/mysql/common.h"
#include "db/backend/db_result.h"

namespace db {

class MysqlPrepareResult : public DBResult {
 private:

  struct BindData {
    BindData() : ptr(nullptr), length(0), is_null(0), error(0) {
      memset(&buf, 0, sizeof(buf));
    }

    const static int kMaxBufferLength = 128;
    char buf[kMaxBufferLength];
    std::vector<char> vbuf;
    char* ptr;
    unsigned long length;
    my_bool is_null;
    my_bool error;
  };

 public:
  virtual NextRowStatus HasNext() override;
  virtual bool Next() override;
  BindData& At(int col);

  template<typename T>
  bool DoFetch(int col, T& v) {
    BindData& data = At(col);
    if (data.is_null) {
      return false;
    }
    v = ParseNumber<T>(std::string(data.ptr, data.length), fmt_);
    return true;
  }

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
  
  MysqlPrepareResult(MYSQL_STMT* statement);
  ~MysqlPrepareResult();
  void Reset();

 private:
  std::istringstream fmt_;
  int columns_;
  MYSQL_STMT* native_statement_;
  unsigned current_row_;
  MYSQL_RES* meta_;
  std::vector<MYSQL_BIND> native_bind_;
  std::vector<BindData> bind_data_;
};

} // namespace db
#endif // DB_DRIVERS_MYSQL_MYSQL_PREPARE_RESULT_H_
