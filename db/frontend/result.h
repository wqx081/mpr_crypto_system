#ifndef DB_FRONTEND_RESULT_H_
#define DB_FRONTEND_RESULT_H_
#include "db/frontend/common.h"
#include "db/backend/db_result.h"
#include "db/backend/db_statement.h"
#include "db/backend/db_connection.h"

namespace db {

class Statement;

class Result final {
 public:
  Result();
  ~Result();
  Result(const Result&);
  const Result& operator=(const Result&);

  int Columns();
  bool Next();
  int Index(const base::StringPiece& name);
  int FindColumn(const base::StringPiece& name);
  std::string Name(int col);
  bool IsNull(int col);
  bool IsNull(const base::StringPiece& name);
  void Clear();
  void RewindColumn();
  bool Empty();
  
  bool Fetch(int col, short& v);  
  bool Fetch(int col, unsigned short& v);  
  bool Fetch(int col, int& v);  
  bool Fetch(int col, unsigned int& v);  
  bool Fetch(int col, long& v);  
  bool Fetch(int col, unsigned long& v);  
  bool Fetch(int col, long long& v);  
  bool Fetch(int col, unsigned long long& v);  
  bool Fetch(int col, float& v);  
  bool Fetch(int col, double& v);  
  bool Fetch(int col, long double& v);  
  bool Fetch(int col, std::string& v);  
  bool Fetch(int col, base::Time* v);  
  bool Fetch(int col, std::ostream& v);  

  bool Fetch(const base::StringPiece& name, short& v);
  bool Fetch(const base::StringPiece& name, unsigned short& v);
  bool Fetch(const base::StringPiece& name, int& v);
  bool Fetch(const base::StringPiece& name, unsigned int& v);
  bool Fetch(const base::StringPiece& name, long& v);
  bool Fetch(const base::StringPiece& name, unsigned long& v);
  bool Fetch(const base::StringPiece& name, long long& v);
  bool Fetch(const base::StringPiece& name, unsigned long long& v);
  bool Fetch(const base::StringPiece& name, float& v);
  bool Fetch(const base::StringPiece& name, double& v);
  bool Fetch(const base::StringPiece& name, long double& v);
  bool Fetch(const base::StringPiece& name, std::string& v);
  bool Fetch(const base::StringPiece& name, base::Time* v);
  bool Fetch(const base::StringPiece& name, std::ostream& v);

  bool Fetch(short& v);
  bool Fetch(unsigned short& v);
  bool Fetch(int& v);
  bool Fetch(unsigned int& v);
  bool Fetch(long& v);
  bool Fetch(unsigned long& v);
  bool Fetch(long long& v);
  bool Fetch(unsigned long long& v);
  bool Fetch(float& v);
  bool Fetch(double& v);
  bool Fetch(long double& v);
  bool Fetch(std::string& v);
  bool Fetch(base::Time* v);
  bool Fetch(std::ostream& v);

  template<typename T>
  T get(const base::StringPiece& name) {
    T v = T();
    if (!Fetch(name, v)) {
      throw NullValueFetch();
    }
    return v;
  }

  template<typename T>
  T get(const base::StringPiece& name, const T& default_value) {
    T v = T();
    if (!Fetch(name, v)) {
      return default_value;
    }
    return v;
  }

  template<typename T>
  T get(int col) {
    T v = T();
    if (!Fetch(col, v)) {
      throw NullValueFetch();
    }
    return v;
  }

  template<typename T>
  T get(int col, const T& default_value) {
    T v = T();
    if (!Fetch(col, v)) {
      return default_value;
    }
    return v;
  }

  template<typename T>
  Result& operator>>(tags::IntoTag<T> ref) {
    if (Fetch(ref.value)) {
      ref.tag = kNotNullValue;
    } else {
      ref.tag = kNullValue;
    }
    return *this;
  }

  template<typename T>
  Result& operator>>(T& value) {
    Fetch(value);
    return *this;
  }

 private:
  Result(DBResultPtr result,
         DBStatementPtr statement,
         DBConnectionPtr connection);
  void Check();

  bool eof_;
  bool fetched_;
  int current_column_;
  DBResultPtr db_result_;
  DBStatementPtr db_statement_;
  DBConnectionPtr db_connection_;

 private:
  friend class Statement;
};

} // namespace db
#endif // DB_FRONTEND_RESULT_H_
