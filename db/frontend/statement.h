#ifndef DB_FRONTEND_STATEMENT_H_
#define DB_FRONTEND_STATEMENT_H_
#include "db/frontend/common.h"
#include "db/frontend/result.h"

namespace db {

class Statement {
 public:
  Statement();
  ~Statement();
  Statement(const Statement&);
  const Statement& operator=(const Statement&);

  void Reset();
  void Clear();
  bool Empty() const;

  Statement& Bind(int32_t v);
  Statement& Bind(uint32_t v);
  Statement& Bind(int64_t v);
  Statement& Bind(uint64_t v);
  Statement& Bind(float v);
  Statement& Bind(double v);
  Statement& Bind(const std::string& vv);
  Statement& Bind(const char* str);
  Statement& Bind(const char* begin, const char* end);
  Statement& Bind(const base::Time& v);
  Statement& Bind(std::istream& v);

  Statement& BindNull();

  void Bind(int col, int32_t v);
  void Bind(int col, uint32_t v);
  void Bind(int col, int64_t v);
  void Bind(int col, uint64_t v);
  void Bind(int col, float v);
  void Bind(int col, double v);
  void Bind(int col, const std::string& vv);
  void Bind(int col, const char* v);
  void Bind(int col, const char* begin, const char* end);
  void Bind(int col, std::istream& v);
  void Bind(int col, const base::Time& v);
  void BindNull(int col);

  int64_t LastInsertId();
  int64_t SequenceLast(const std::string& seq);
  uint64_t Affected();

  Result Row();
  Result Query();
  operator Result();

  void Execute();

  Statement& operator<<(const std::string& v);

 private:
  Statement(scoped_ref_ptr<DBStatement> db_statement,
            scoped_ref_ptr<DBConnection> db_connection);

  int placeholder_;
  scoped_ref_ptr<DBStatement> db_statement_;
  scoped_ref_ptr<DBConnection> db_connection_;

 private:
  friend class Session;
};

} // namespace db
#endif // DB_FRONTEND_STATEMENT_H_
