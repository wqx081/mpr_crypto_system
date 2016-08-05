#ifndef DB_BACKEND_DB_STATEMENT_H_
#define DB_BACKEND_DB_STATEMENT_H_
#include "base/macros.h"
#include "base/string_piece.h"
#include "base/time.h"

#include "db/backend/common.h"
#include "db/backend/db_result.h"

namespace db {

class DBStatementsCache;
class DBStatement;

using DBStatementPtr = SimpleRefPtr<DBStatement>;


class DBStatement : public SimpleRefCounted {
 public:

  virtual void Reset() = 0;
  virtual const std::string& SqlQuery() = 0;

  virtual void Bind(int col, const base::StringPiece& s) = 0;
  virtual void Bind(int col, const char* begin, const char* end) = 0;
  virtual void Bind(int col, const base::Time& v) = 0;
  virtual void Bind(int col, std::istream& v) = 0;
  virtual void Bind(int col, int v) = 0;
  virtual void Bind(int col, unsigned int v) = 0;
  virtual void Bind(int col, long v) = 0;
  virtual void Bind(int col, unsigned long v) = 0;
  virtual void Bind(int col, long long v) = 0;
  virtual void Bind(int col, unsigned long long v) = 0;
  virtual void Bind(int col, double v) = 0;
  virtual void Bind(int col, long double v) = 0;

  virtual void BindNull(int col) = 0;
  virtual long long SequenceLast(const base::StringPiece& sequence) = 0;
  virtual unsigned long long Affected() = 0;
  virtual DBResult* Query() = 0;
  virtual void Execute() = 0;

  static void Dispose(DBStatement* self);

  void SetCache(DBStatementsCache* cache);

  DBStatement();
  virtual ~DBStatement() {};

 private:
  DBStatementsCache* cache_;
};

class DBStatementsCache {
 public:
  DBStatementsCache();
  bool IsActive();
  void SetSize(size_t n);
  void Put(DBStatement* statement);
  void Clear();
  DBStatementPtr Fetch(const std::string& query);
  ~DBStatementsCache();

 private:
  struct Data;
  std::unique_ptr<Data> data_;

  DISALLOW_COPY_AND_ASSIGN(DBStatementsCache);
};

} // namespace db
#endif // DB_BACKEND_DB_STATEMENT_H_
