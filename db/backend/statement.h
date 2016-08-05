#ifndef DB_BACKEND_STATEMENT_H_
#define DB_BACKEND_STATEMENT_H_
#include "db/simple_ref_counted.h"
#include "base/time.h"

#include <memory>

namespace db {
namespace backend {

class Result;
class StatementsCache;

class Statement : public SimpleRefCounted {
 public:
  Statement();
  virtual ~Statement();
  
  virtual void Reset() = 0;
  virtual const std::string& SqlQuery() = 0; 
  virtual void Bind(int col, const std::string&) = 0;
  virtual void Bind(int col, const char* s) = 0;
  virtual void Bind(int col, const char* b, const char* e) = 0;
  virtual void Bind(int col, const base::Time&) = 0;
  virtual void Bind(int col, std::istream& ) = 0;
  virtual void Bind(int col, int v) = 0;
  virtual void Bind(int col, unsigned v) = 0;
  virtual void Bind(int col, long v) = 0;
  virtual void Bind(int col, unsigned long v) = 0;
  virtual void Bind(int col, long long v) = 0;
  virtual void Bind(int col, unsigned long long v) = 0;
  virtual void Bind(int col, double v) = 0;
  virtual void Bind(int col, long double v) = 0;
  
  virtual void BindNull(int col) = 0;
  virtual long long SequenceLast(const std::string& sequence) = 0;
  virtual unsigned long long Affected() = 0;
  
  virtual Result* Query() = 0;
  virtual void Execute() = 0;

  //Caching
  static void Dispose(Statement* self);
  void SetCache(StatementsCache* cache);

 private:
  StatementsCache* cache_;
};

class StatementsCache {
 public:
  StatementsCache();
  ~StatementsCache();

  bool IsActive();
  void SetSize(size_t n);
  void Put(Statement* statement);
  void Clear();
  SimpleRefPtr<Statement> Fetch(const std::string& q);    

 private:
  struct Data;
  std::unique_ptr<Data> data_;

  DISALLOW_COPY_AND_ASSIGN(StatementsCache);
};

} // namespace backend
} // namespace db
#endif // DB_BACKEND_STATEMENT_H_
