#include "db/drivers/mysql/mysql_prepare_result.h"
#include "db/backend/db_result.h"

namespace db {

MysqlPrepareResult::MysqlPrepareResult(MYSQL_STMT* stmt)
  : native_statement_(stmt),
    current_row_(0),
    meta_(nullptr) {
  fmt_.imbue(std::locale::classic());
  columns_ = ::mysql_stmt_field_count(native_statement_);
  if (::mysql_stmt_store_result(native_statement_)) {
    throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
  }
  meta_ = ::mysql_stmt_result_metadata(native_statement_);
  if (!meta_) {
    throw mysql_backend::MyException("Seems that the query does not produce any result");
  }
}

MysqlPrepareResult::~MysqlPrepareResult() {
  ::mysql_free_result(meta_);
}

void MysqlPrepareResult::Reset() {
  native_bind_.resize(0);
  bind_data_.resize(0);
  native_bind_.resize(columns_, MYSQL_BIND());
  bind_data_.resize(columns_, BindData());
  for (int i = 0; i < columns_; ++i) {
    native_bind_[i].buffer_type = MYSQL_TYPE_STRING;
    native_bind_[i].buffer = bind_data_[i].buf;
    native_bind_[i].buffer_length = sizeof(bind_data_[i].buf);
    native_bind_[i].length = &bind_data_[i].length;
    native_bind_[i].is_null = &bind_data_[i].is_null;
    native_bind_[i].error = &bind_data_[i].error;
    bind_data_[i].ptr = bind_data_[i].buf;
  }
}

DBResult::NextRowStatus MysqlPrepareResult::HasNext() {
  if (current_row_ >= ::mysql_stmt_num_rows(native_statement_)) {
    return kLastRowReached;
  } else {
    return kNextRowExists;
  }
}

bool MysqlPrepareResult::Next() {
  current_row_++;
  Reset();
  if (columns_ > 0) {
    if (::mysql_stmt_bind_result(native_statement_, &native_bind_[0])) {
      throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
    }
  }
  int r = ::mysql_stmt_fetch(native_statement_);
  if (r == MYSQL_NO_DATA) {
    return false;
  }

  if (r == MYSQL_DATA_TRUNCATED) {
    for (int i = 0; i < columns_; ++i) {
      if (bind_data_[i].error &&
          !bind_data_[i].is_null &&
          bind_data_[i].length >= sizeof(!bind_data_[i].buf)) {
 
        bind_data_[i].vbuf.resize(bind_data_[i].length);
        native_bind_[i].buffer = &bind_data_[i].vbuf.front();
        native_bind_[i].buffer_length = bind_data_[i].length;
        if (::mysql_stmt_fetch_column(native_statement_, &native_bind_[i], i, 0)) {
          throw mysql_backend::MyException(::mysql_stmt_error(native_statement_));
        }
        bind_data_[i].ptr = &bind_data_[i].vbuf.front();
      }
    }
  }
  return true;
}

MysqlPrepareResult::BindData& MysqlPrepareResult::At(int col) {
  if (col < 0 || col >= columns_) {
    throw InvalidColumn();
  }
  if (bind_data_.empty()) {
    throw mysql_backend::MyException("Attempt to access data without fetching it first");
  }
  return bind_data_.at(col);
}

bool MysqlPrepareResult::Fetch(int col, short& v) {
  return DoFetch(col, v);
}

bool MysqlPrepareResult::Fetch(int col, unsigned short& v) {
  return DoFetch(col, v);
}

bool MysqlPrepareResult::Fetch(int col, int& v) {
  return DoFetch(col, v);
}
bool MysqlPrepareResult::Fetch(int col, unsigned int& v) {
  return DoFetch(col, v);
}

bool MysqlPrepareResult::Fetch(int col, long& v) {
  return DoFetch(col, v);
}

bool MysqlPrepareResult::Fetch(int col, unsigned long& v) {
  return DoFetch(col, v);
}

bool MysqlPrepareResult::Fetch(int col, long long& v) {
  return DoFetch(col, v);
}
bool MysqlPrepareResult::Fetch(int col, unsigned long long& v) {
  return DoFetch(col, v);
}
bool MysqlPrepareResult::Fetch(int col, float& v) {
  return DoFetch(col, v);
}

bool MysqlPrepareResult::Fetch(int col, double& v) {
  return DoFetch(col, v);
}
bool MysqlPrepareResult::Fetch(int col, long double& v) {
  return DoFetch(col, v);
}

bool MysqlPrepareResult::Fetch(int col, std::string& v) {
  BindData& data = At(col);
  if (data.is_null) {
    return false;
  }
  v.assign(data.ptr, data.length);
  return true;
}

bool MysqlPrepareResult::Fetch(int col, std::ostream& v) {
  BindData& data = At(col);
  if (data.is_null) {
    return false;
  }
  v.write(data.ptr, data.length);
  return true;
}

bool MysqlPrepareResult::Fetch(int col, base::Time* v) {
  std::string str;
  if (!Fetch(col, str)) {
    return false;
  }
  return base::Time::FromString(str.c_str(), v);
}

bool MysqlPrepareResult::IsNull(int col) {
  return At(col).is_null;
}

int MysqlPrepareResult::Columns() {
  return columns_;
}

std::string MysqlPrepareResult::ColumnToName(int col) {
  if (col < 0 || col >= columns_) {
    throw InvalidColumn();
  }
  MYSQL_FIELD* fields = ::mysql_fetch_fields(meta_);
  if (!fields) {
    throw mysql_backend::MyException("Internal error empty fileds");
  }
  return fields[col].name;
}

int MysqlPrepareResult::NameToColumn(const base::StringPiece& name) {
  MYSQL_FIELD* fields = ::mysql_fetch_fields(meta_);  
  if (!fields) {
    throw mysql_backend::MyException("Internal error empty fields");
  }
  for (int i = 0; i < columns_; ++i) {
    if (name == fields[i].name) {
      return i;
    }
  }
  return -1;
}


} // namespace db
