#ifndef DB_DRIVERS_MYSQL_RESULT_H_
#define DB_DRIVERS_MYSQL_RESULT_H_

#include "db/drivers/mysql/common.h"

namespace db {
namespace mysql_backend {

class UnPreMysqlResult : public backend::Result {
 public:
  virtual NextRowStatus HasNext() override {
    if (!res_) {
      return kLastRowReached;
    }
    if (current_row_ >= ::mysql_num_rows(res_)) {
      return kLastRowReached;
    } else {
      return kNextRowExists;
    }
  }
  virtual bool Next() override {
    if (!res_) {
      return false;
    }
    current_row_++;
    row_ = ::mysql_fetch_row(res_);
    if (!row_) {
      return false;
    }
    return true;
  }

  const char* At(int col) {
    if (!res_) {
      throw EmptyRowAccess();
    }
    if (col < 0 || col >= columns_) {
      throw InvalidColumn();
    }
    return row_[col];
  }

  const char* At(int col, size_t& len) {
    if (!res_) {
      throw EmptyRowAccess();
    }
    if (col < 0 || col >= columns_) {
      throw InvalidColumn();
    }
    unsigned long* lengths = ::mysql_fetch_lengths(res_);
    if (lengths == nullptr) {
      throw MyException("Can't get length of column");
    }
    len = lengths[col];
    return row_[col];
  }

  template<typename T>
  bool DoFetch(int col, T& v) {
    size_t len;
    const char* s = At(col, len);
    if (!s) {
      return false;
    }
    v = ParseNumber<T>(std::string(s, len), fmt_);
    return true;
  }

  virtual bool Fetch(int col, short& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, unsigned short& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, int& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, unsigned& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, long& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, unsigned long& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, long long& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, unsigned long long& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, float& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, double& v) override {
    return DoFetch(col, v);
  }
  virtual bool Fetch(int col, long double& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, std::string& v) override {
    size_t len;
    const char* s = At(col, len);
    if (!s) {
      return false;
    }
    v.assign(s, len);
    return true;
  }

  virtual bool Fetch(int col, std::ostream& v) override {
    size_t len;
    const char* s = At(col, len);
    if (!s) {
      return false;
    }
    v.write(s, len);
    return true;
  }

  virtual bool Fetch(int col, base::Time* v) override {
    size_t len;
    const char* s = At(col, len);
    std::string tmp(s, len);
    return base::Time::FromString(tmp.c_str(), v);
  }

  virtual bool IsNull(int col) override {
    return At(col) == nullptr;
  }

  virtual int Columns() override {
    return columns_;
  }

  virtual std::string ColumnToName(int col) override {
    if (col < 0 || col >= columns_) {
      throw InvalidColumn();
    }
    if (!res_) {
      throw EmptyRowAccess();
    }

    MYSQL_FIELD* fields = ::mysql_fetch_fields(res_);
    if (!fields) {
      throw MyException("Internal error empty fields");
    }
    return fields[col].name;
  }
  
  virtual int NameToColumn(const std::string& name) override {
    if (!res_) {
      throw EmptyRowAccess();   
    }

    MYSQL_FIELD* fields = ::mysql_fetch_fields(res_);
    if (!fields) {
      throw MyException("Internal error empty fields");
    }
    for (int i = 0; i < columns_; ++i) {
      if (name == fields[i].name) {
        return i;
      }
    }
    return -1;
  }


  // internal
  UnPreMysqlResult(MYSQL* connection) :
    res_(nullptr),
    columns_(0),
    current_row_(0),
    row_(nullptr) {

   fmt_.imbue(std::locale::classic()); 
   res_ = ::mysql_store_result(connection);
   if (!res_) {
     columns_ = ::mysql_field_count(connection);
     if (columns_ == 0) {
       throw MyException("query does not produce any result");
     }
   } else {
     columns_ = ::mysql_num_fields(res_);
   }
 }

  ~UnPreMysqlResult() {
    if (res_) {
      ::mysql_free_result(res_);
    }
  }  

 private:
  std::istringstream fmt_;
  MYSQL_RES* res_;
  int columns_;
  unsigned current_row_;
  MYSQL_ROW row_;
};

///

class PreMysqlResult : public backend::Result {
 private:
  struct BindData {
    BindData() 
      : ptr(nullptr), length(0), is_null(false), error(false) {}
		  
    char buf[128];
    std::vector<char> vbuf;
    char* ptr;
    unsigned long length;
    my_bool is_null;
    my_bool error;
  };

 public:

  virtual NextRowStatus HasNext() override {
    if (current_row_ >= ::mysql_stmt_num_rows(stmt_)) {
      return kLastRowReached;
    } else {
      return kNextRowExists;
    } 
  }  

  virtual bool Next() override {
    current_row_++;
    Reset();
    if (columns_ > 0) {
      if (::mysql_stmt_bind_result(stmt_, &bind_[0])) {
        throw MyException(::mysql_stmt_error(stmt_));
      }
    }

    int ret = ::mysql_stmt_fetch(stmt_);
    if (ret == MYSQL_NO_DATA) {
      return false;
    }
    if (ret == MYSQL_DATA_TRUNCATED) {
      for (int i = 0; i < columns_; ++i) {
        if (bind_data_[i].error    &&
            !bind_data_[i].is_null &&
	    bind_data_[i].length >= sizeof(bind_data_[i].buf)) {

	  bind_data_[i].vbuf.resize(bind_data_[i].length);
	  bind_[i].buffer = &bind_data_[i].vbuf.front();
	  bind_[i].buffer_length = bind_data_[i].length;
	  if (::mysql_stmt_fetch_column(stmt_, &bind_[i], i, 0)) {
	    throw MyException(::mysql_stmt_error(stmt_));
	  }
	  bind_data_[i].ptr = &bind_data_[i].vbuf.front();
	}
      }  
    }
    return true;
  }

  BindData& At(int col) {
    if (col < 0 || col >= columns_) {
      throw InvalidColumn();
    }  
    if (bind_data_.empty()) {
      throw MyException("Attempt to access data without fetching it first");
    }
    return bind_data_.at(col);
  }

  
  template<typename T>
  bool DoFetch(int col, T& v) {
    BindData& bind_data = At(col);
    if (bind_data.is_null) {
      return false;
    }

    v = ParseNumber<T>(std::string(bind_data.ptr, bind_data.length), fmt_);
    return true;
  }

  virtual bool Fetch(int col, short& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, unsigned short& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, int& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, unsigned& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, long& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, unsigned long& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, long long& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, unsigned long long& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, float& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, double& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, long double& v) override {
    return DoFetch(col, v);
  }

  virtual bool Fetch(int col, std::string& v) override {
    BindData& bind_data = At(col);
    if (bind_data.is_null) {
      return false;
    }
    v.assign(bind_data.ptr, bind_data.length);
    return true;
  }

  virtual bool Fetch(int col, std::ostream& v) override {
    BindData& bind_data = At(col);
    if (bind_data.is_null) {
      return false;
    }
    v.write(bind_data.ptr, bind_data.length);
    return true;
  }

  virtual bool Fetch(int col, base::Time* v) override {
    std::string str;
    if (!Fetch(col, str)) {
      return false;
    }  
    return base::Time::FromString(str.c_str(), v);
  }

  virtual bool IsNull(int col) override {
    return At(col).is_null;
  }

  virtual int Columns() override {
    return columns_;
  }

  virtual std::string ColumnToName(int col) override {
    if (col < 0 || col >= columns_) {
      throw InvalidColumn();
    }
    MYSQL_FIELD* fields = ::mysql_fetch_fields(meta_);
    if (!fields) {
      throw MyException("Internal error empty fields");
    }
    return fields[col].name;
  }

  virtual int NameToColumn(const std::string& name) override {
    MYSQL_FIELD* fields = ::mysql_fetch_fields(meta_);
    if (!fields) {
      throw MyException("Internal error empty fields");
    }
    for (int i = 0; i < columns_; ++i) {
      if (name == fields[i].name) {
        return i;
      }
    }
    return -1;
  }

  // Internal
  PreMysqlResult(MYSQL_STMT* stmt)
      : stmt_(stmt), current_row_(0), meta_(nullptr) {

    fmt_.imbue(std::locale::classic());  
    columns_ = ::mysql_stmt_field_count(stmt_);
    if (::mysql_stmt_store_result(stmt_)) {
      throw MyException(::mysql_stmt_error(stmt_));
    }
    meta_ = ::mysql_stmt_result_metadata(stmt_);
    if (!meta_) {
      throw MyException("seems that the query does not produce any result");
    }
  }

  ~PreMysqlResult() {
    ::mysql_free_result(meta_);
  }

  void Reset() {
    bind_.resize(0);
    bind_data_.resize(0);
    bind_.resize(columns_, MYSQL_BIND());
    bind_data_.resize(columns_, BindData());
    for (int i = 0; i < columns_; ++i) {
      bind_[i].buffer_type = MYSQL_TYPE_STRING;
      bind_[i].buffer = bind_data_[i].buf;
      bind_[i].buffer_length = sizeof(bind_data_[i].buf);
      bind_[i].length = &bind_data_[i].length;
      bind_[i].is_null = &bind_data_[i].is_null;
      bind_[i].error = &bind_data_[i].error;
      bind_data_[i].ptr = bind_data_[i].buf;
    }
  }

 private:
  std::istringstream fmt_;
  int columns_;
  MYSQL_STMT* stmt_;
  unsigned current_row_;
  MYSQL_RES* meta_;
  std::vector<MYSQL_BIND> bind_;
  std::vector<BindData> bind_data_;
};

} // namespace mysql_backend
} // db

#endif
