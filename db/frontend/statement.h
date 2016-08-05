#ifndef DB_FRONTEND_STATEMENT_H_
#define DB_FRONTEND_STATEMENT_H_
#include "base/time.h"
#include "base/string_piece.h"
#include "db/frontend/frontend.h"
#include "db/frontend/result.h"

namespace db {

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
  Statement& Bind(const char* b, const char* e);
  Statement& Bind(const base::Time& v);
  Statement& Bind(std::istream& v);
  Statement& BindNull();
  
  template<typename T>
  Statement& Bind(const tags::UseTag<T>& val) {
    if (val.tag == kNullValue) {
      return BindNull();
    } else {
      return Bind(val.value);
    }
  }

  void Bind(int col, int v);
  void Bind(int col, unsigned v);
  void Bind(int col, long v);
  void Bind(int col, unsigned long v);
  void Bind(int col, long long v);
  void Bind(int col, unsigned long long v);
  void Bind(int col, double v);
  void Bind(int col, long double v);
  void Bind(int col, base::StringPiece& str);
  void Bind(int col, const char* b, const char* e);
  void Bind(int col, const base::Time& v);
  void Bind(int col, std::istream& v);
  void BindNull(int col);
  
  long long LastInsertId();
  long long SequenceLast(const std::string& seq);
  unsigned long long Affected();

  Result Row();
  Result Query();
  operator Result();

  void Execute();

  Statement& operator<<(const std::string& v);
  Statement& operator<<(const char* s);
  Statement& operator<<(const base::Time& v);
  Statement& operator<<(std::istream& v);
  Statement& operator<<(void (*manipulator)(Statement& statement));
  Result operator<<(Result  (*manipulator)(Statement& statement));
  
  template<typename T>
  Statement& operator<<(const tags::UseTag<T>& value) {
    if (value.tag == kNullValue) {
      return BindNull();
    } else {
      return Bind(value.value);
    }
  }

  template<typename T>
  Statement& operator<<(T v) {
    return Bind(v);
  }

 private:
  Statement(SimpleRefPtr<backend::Statement> statement,
            SimpleRefPtr<backend::Connection> connection);

  int placeholder_;
  SimpleRefPtr<backend::Statement> statement_;
  SimpleRefPtr<backend::Connection> connection_;
 private:
  friend class Session;
};

//TODO
#if 0
inline void Execute(Statement& s) {
  s.Execute();
}

inline void Null(Statement& s) {
  s.BindNull();
}

inline Result Row(Statement& s) {
  return s.Row();
}
#endif



} // namespace db
#endif // DB_FRONTEND_STATEMENT_H_
