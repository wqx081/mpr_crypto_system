#ifndef DB_FRONTEND_STATEMENT_H_
#define DB_FRONTEND_STATEMENT_H_
#include "db/frontend/common.h"
#include "db/frontend/result.h"
#include "db/backend/db_statement.h"
#include "db/backend/db_connection.h"

namespace db {

class Session;

class Statement final {
 public:
  Statement();
  ~Statement();
  Statement(const Statement&);
  const Statement& operator=(const Statement&);

  void Reset();
  void Clear();
  bool Empty() const;

  Statement& Bind(int v);
  Statement& Bind(unsigned int v);
  Statement& Bind(long v);
  Statement& Bind(unsigned long v);
  Statement& Bind(long long v);
  Statement& Bind(unsigned long long v);
  Statement& Bind(double v);
  Statement& Bind(long double v);
  Statement& Bind(const base::StringPiece& str);
  Statement& Bind(const char* begin, const char* end);
  Statement& Bind(const base::Time& v);
  Statement& Bind(std::istream& v);
  Statement& BindNull();

  void Bind(int col, int v);
  void Bind(int col, unsigned int v);
  void Bind(int col, long v);
  void Bind(int col, unsigned long v);
  void Bind(int col, long long v);
  void Bind(int col, unsigned long long v);
  void Bind(int col, double v);
  void Bind(int col, long double v);
  void Bind(int col, const base::StringPiece& v);
  void Bind(int col, const char* begin, const char* end);
  void Bind(int col, const base::Time& v);
  void Bind(int col, std::istream& v);
  void BindNull(int col);

  long long LastInsertId();
  long long SequenceLast(const base::StringPiece& str);
  unsigned long long Affected();
  
  Result Row();
  Result Query();
  operator Result();

  void Execute();

  Statement& operator<<(const base::StringPiece& str);
  Statement& operator<<(const base::Time& v);
  Statement& operator<<(std::istream& v);
  Statement& operator<<(void (*manipulator)(Statement& statement));
  Result operator<<(Result (*manipulator) (Statement& statement)); 

  template<typename T>
  Statement& operator<<(const tags::UseTag<T>& val) {
    if (val.tag == kNullValue) {
      return BindNull();
    } else {
      return Bind(val.value);
    }
  }

  template<typename T>
  Statement& operator<<(T v) {
    return Bind(v);
  }

 private:
  Statement(DBStatementPtr statement,
            DBConnectionPtr connection);

  int placeholder_;
  DBStatementPtr db_statement_;
  DBConnectionPtr db_connection_;
  
 private:
  friend class Session;
};

inline void Execute(Statement& statement) {
  statement.Execute();
}

inline void Null(Statement& statement) {
  statement.BindNull();
}

inline Result Row(Statement& statement) {
  return statement.Row();
}

} // namespace db
#endif // DB_FRONTEND_STATEMENT_H_
