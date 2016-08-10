#ifndef DB_FRONTEND_RESULT_H_
#define DB_FRONTEND_RESULT_H_
#include "db/frontend/common.h"

namespace db {

class Result {
 public:
  Result();
  ~Result();
  Result(const Result&);
  const Result& operator=(const Result&);
  
  int Columns();
  bool Next();
  int Index(const std::string& name) const;
  int FindColumn(const std::string& name);
  std::string Name(int col);

  bool IsNull(int col) const ;
  bool IsNull(const std::string& name) const;
  void Clear();
  void RewindColumn();
  bool Empty() const;

  bool Fetch(int col, int16_t& v);
  bool Fetch(int col, uint16_t& v);
  bool Fetch(int col, int32_t& v);
  bool Fetch(int col, uint32_t& v);
  bool Fetch(int col, int64_t& v);
  bool Fetch(int col, uint64_t& v);
  bool Fetch(int col, float& v);
  bool Fetch(int col, double& v);
  bool Fetch(int col, std::string& v);
  bool Fetch(int col, base::Time& v);
  bool Fetch(int col, std::ostream& v);

  bool Fetch(const std::string& name, int16_t& v);
  bool Fetch(const std::string& name, uint16_t& v);
  bool Fetch(const std::string& name, int32_t& v);
  bool Fetch(const std::string& name, uint32_t& v);
  bool Fetch(const std::string& name, int64_t& v);
  bool Fetch(const std::string& name, uint64_t& v);
  bool Fetch(const std::string& name, float& v);
  bool Fetch(const std::string& name, double& v);
  bool Fetch(const std::string& name, std::string& v);
  bool Fetch(const std::string& name, base::Time& v);
  bool Fetch(const std::string& name, std::ostream& v);

  bool Fetch(int16_t& v);
  bool Fetch(uint16_t& v);
  bool Fetch(int32_t& v);
  bool Fetch(uint32_t& v);
  bool Fetch(int64_t& v);
  bool Fetch(uint64_t& v);
  bool Fetch(float& v);
  bool Fetch(double& v);
  bool Fetch(std::string& v);
  bool Fetch(base::Time& v);
  bool Fetch(std::ostream& v);

  template<typename T>
  T get(const std::string& name) {
    T v = T();
    if (!Fetch(name, v)) {
      throw NullValueFetch();
    }
    return v;
  }
  
  template<typename T>
  T get(const std::string& name, const T& def) {
    T v = T();
    if (!Fetch(name, v)) {
      return def;
    }
    return v;
  }

  template<typename T>
  T get(int col) {
    T v = T();
    if (!Fetch(col, v)) {
      return NullValueFetch();
    }
    return v;
  }

  template<typename T>
  T get(int col, const T& def) {
    T v = T();
    if (!Fetch(col, v)) {
      return def;
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

  Result(scoped_ref_ptr<DBResult> res,
         scoped_ref_ptr<DBStatement> statement,
         scoped_ref_ptr<DBConnection> conn);

  void Check();
  friend class Statement;

  bool eof_;
  bool fetched_;
  int current_column_;
  scoped_ref_ptr<DBResult> db_result_;
  scoped_ref_ptr<DBStatement> db_statement_;
  scoped_ref_ptr<DBConnection> db_connection_;
};

} // namespace db
#endif // DB_FRONTEND_RESULT_H_
