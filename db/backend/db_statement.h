#ifndef DB_BACKEND_DB_STATEMENT_H_
#define DB_BACKEND_DB_STATEMENT_H_
#include "db/backend/common.h"
#include "db/backend/db_result.h"
#include "base/ref_counted.h"
#include "base/time.h"
#include "base/macros.h"

namespace db {

class DBStatementCache;

class DBStatement : public base::RefCountedThreadSafe<DBStatement> {
 public:
  DBStatement();

  virtual void Reset() = 0;
  virtual const std::string& SqlQuery() = 0;
  virtual void Bind(int col, const std::string& v) = 0;
  virtual void Bind(int col, const char* v) = 0;
  virtual void Bind(int col, const char* begin, const char* end) = 0;
  virtual void Bind(int col, const base::Time& v) = 0;
  virtual void Bind(int col, std::istream& in) = 0;

  virtual void Bind(int col, int32_t v) = 0;
  virtual void Bind(int col, uint32_t v) = 0;
  virtual void Bind(int col, int64_t v) = 0;
  virtual void Bind(int col, uint64_t v) = 0;
  virtual void Bind(int col, double v) = 0;
  virtual void BindNull(int col) = 0;

  virtual int64_t SequenceLast(const std::string& sequence) = 0;
  virtual uint64_t Affected() = 0;
  
  virtual DBResult* Query() = 0;
  virtual void Execute() = 0;
  
  void Cache(DBStatementCache* cache) {
    cache_ = cache;
  }
  
 protected:
  virtual ~DBStatement();

 private:
  DBStatementCache* cache_;
  friend base::RefCountedThreadSafe<DBStatement>;

};

class DBStatementCache {
 public:
  DBStatementCache();
  bool IsActive();
  void SetSize(size_t n);
  void Put(scoped_ref_ptr<DBStatement> statement);
  void Clear();
  scoped_ref_ptr<DBStatement> Fetch(const std::string& query);
  ~DBStatementCache();

 private:
  struct Data;
  std::unique_ptr<Data> data_;

  DISALLOW_COPY_AND_ASSIGN(DBStatementCache);
};

} // namespace db
#endif // DB_BACKEND_DB_STATEMENT_H_
