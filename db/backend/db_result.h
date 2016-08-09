#ifndef DB_BACKEND_DB_RESULT_H_
#define DB_BACKEND_DB_RESULT_H_
#include "db/backend/common.h"
#include "db/common/connection_info.h"

#include "base/ref_counted.h"
#include "base/macros.h"
#include "base/time.h"

namespace db {

class DBResult : public base::RefCountedThreadSafe<DBResult> {
 public:
  DBResult() {}

  enum NextRowStatus {
    kLastRowReached,
    kNextRowExists,
    kNextRowUnknown,
  };

  virtual NextRowStatus HasNext() = 0;
  virtual bool Next() = 0;
  virtual bool Fetch(int col, int16_t& v) = 0;
  virtual bool Fetch(int col, uint16_t& v) = 0;
  virtual bool Fetch(int col, int32_t& v) = 0;
  virtual bool Fetch(int col, uint32_t& v) = 0;
  virtual bool Fetch(int col, int64_t& v) = 0;
  virtual bool Fetch(int col, uint64_t& v) = 0;
  virtual bool Fetch(int col, float& v) = 0;
  virtual bool Fetch(int col, double& v) = 0;
  virtual bool Fetch(int col, std::string& v) = 0; 
  virtual bool Fetch(int col, base::Time* v) = 0;
  virtual bool Fetch(int col, std::ostream& v) = 0;
  
  virtual bool IsNull(int col) = 0;
  virtual int Columns() = 0;
  virtual int NameToColumn(const std::string& name) = 0;
  virtual std::string ColumnToName(int col) = 0;

 protected:
  virtual ~DBResult(){}

 private:
  friend class base::RefCountedThreadSafe<DBResult>;
};

} // namespace db
#endif // DB_BACKEND_DB_RESULT_H_
