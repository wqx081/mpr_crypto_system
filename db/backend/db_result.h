#ifndef DB_BACKEND_DB_RESULT_H_
#define DB_BACKEND_DB_RESULT_H_

#include "base/time.h"
#include "base/string_piece.h"
#include "db/backend/common.h"

namespace db {

class DBResult : public SimpleRefCounted {
 public:
  enum NextRowStatus {
    kLastRowReached,
    kNextRowExists,
    kNextRowUnknown,
  };

  virtual NextRowStatus HasNext() = 0;
  virtual bool Next() = 0;

  virtual bool Fetch(int col, short& v) = 0;
  virtual bool Fetch(int col, unsigned short& v) = 0;
  virtual bool Fetch(int col, int& v) = 0;
  virtual bool Fetch(int col, unsigned int& v) = 0;
  virtual bool Fetch(int col, long& v) = 0;
  virtual bool Fetch(int col, unsigned long& v) = 0;
  virtual bool Fetch(int col, long long& v) = 0;
  virtual bool Fetch(int col, unsigned long long& v) = 0;
  virtual bool Fetch(int col, float& v) = 0;
  virtual bool Fetch(int col, double& v) = 0;
  virtual bool Fetch(int col, long double& v) = 0;
  virtual bool Fetch(int col, std::ostream& v) = 0;
  virtual bool Fetch(int col, base::Time* v) = 0;
  virtual bool Fetch(int col, std::string& v) = 0;

  virtual bool IsNull(int col) = 0;
  virtual int Columns() = 0;
  virtual int NameToColumn(const base::StringPiece& name) = 0;
  virtual std::string ColumnToName(int col) = 0;

  DBResult(); 
  virtual ~DBResult() {}
};

} // namespace db
#endif // DB_BACKEND_DB_RESULT_H_
