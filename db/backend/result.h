#ifndef DB_BACKEND_RESULT_H_
#define DB_BACKEND_RESULT_H_
#include "db/simple_ref_counted.h"
#include "base/time.h"

#include <memory>

namespace db {
namespace backend {

class Result : public SimpleRefCounted {
 public:
  Result();
  virtual ~Result();

  enum NextRowStatus {
    kLastRowReached,
    kNextRowExists,
    kNextRowUnknown,
  };
  
  virtual NextRowStatus HasNext() = 0;
  virtual bool Next() = 0;
  virtual bool Fetch(int col, short&v) = 0;
  virtual bool Fetch(int col, unsigned short& v) = 0;
  virtual bool Fetch(int col, int& v) = 0;
  virtual bool Fetch(int col, unsigned& v) = 0;
  virtual bool Fetch(int col, long& v) = 0;
  virtual bool Fetch(int col, unsigned long& v) = 0;
  virtual bool Fetch(int col, long long& v) = 0;
  virtual bool Fetch(int col, unsigned long long& v) = 0;
  virtual bool Fetch(int col, float& v) = 0;
  virtual bool Fetch(int col, double& v) = 0;
  virtual bool Fetch(int col, long double& v) = 0;
  virtual bool Fetch(int col, std::string& v) = 0;
  virtual bool Fetch(int col, std::ostream& v) = 0;
  virtual bool Fetch(int col, base::Time& v) = 0;
  
  virtual bool IsNull(int col) = 0;
  virtual int Columns() = 0;
  virtual int NameToColumn(const std::string& name) = 0;
  virtual std::string ColumnToName(int col) = 0;

#if 0
 private:
  struct Data;
  std::unique_ptr<Data> data_;
#endif

};

} // namespace backend
} // namespace db
#endif // DB_BACKEND_RESULT_H_
