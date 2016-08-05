#include "db/drivers/mysql/mysql_direct_result.h"

namespace db {

DBResult::NextRowStatus MysqlDirectResult::HasNext() {
  if (!native_result_) {
    return kLastRowReached;
  }
  if (current_row_ >= ::mysql_num_rows(native_result_)) {
    return kLastRowReached;
  } else {
    return kNextRowExists;
  }
}

bool MysqlDirectResult::Next() {
  if (!native_result_) {
    return false;
  }
  current_row_++;
  native_row_ = ::mysql_fetch_row(native_result_);
  if (!native_row_) {
    return false;
  }
  return true;
}

const char* MysqlDirectResult::At(int col) {
  if (!native_result_) {
    throw EmptyRowAccess();
  }
  if (col < 0 || col >= columns_) {
    throw InvalidColumn();
  }
  return native_row_[col];
}

const char* MysqlDirectResult::At(int col, size_t& len) {
  if (!native_result_) {
    throw EmptyRowAccess();
  }
  if (col < 0 || col >= columns_) {
    throw InvalidColumn();
  }
  unsigned long* lengths = ::mysql_fetch_lengths(native_result_);
  if (lengths == nullptr) {
    throw mysql_backend::MyException("Can't get length of column");
  }
  len = lengths[col];
  return native_row_[col];
}

bool MysqlDirectResult::Fetch(int col, short& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, unsigned short& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, int& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, unsigned int& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, long& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, unsigned long& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, long long& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, unsigned long long& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, float& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, double& v) {
  return DoFetch(col, v);
}
bool MysqlDirectResult::Fetch(int col, long double& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, std::string& v) {
  size_t len;
  const char* sp = At(col, len);
  if (!sp) {
    return false;
  }
  v.assign(sp, len);
  return true;
}

bool MysqlDirectResult::Fetch(int col, std::ostream& v) {
  size_t len;
  const char* sp = At(col, len);
  if (!sp) {
    return false;
  }
  v.write(sp, len);
  return true;
}

bool MysqlDirectResult::Fetch(int col, base::Time* v) {
  size_t len;
  const char* sp = At(col, len);
  if (!sp) {
    return false;
  }
  std::string t(sp, len);
  return base::Time::FromString(t.c_str(), v);
}

bool MysqlDirectResult::IsNull(int col) {
  return At(col) == 0;
}

int MysqlDirectResult::Columns() {
  return columns_;
}

std::string MysqlDirectResult::ColumnToName(int col) {
  if (col < 0 || col >= columns_) {
    throw InvalidColumn();
  }
  if (!native_result_) {
    throw EmptyRowAccess();
  }
  MYSQL_FIELD* fields = ::mysql_fetch_fields(native_result_);
  if (!fields) {
    throw mysql_backend::MyException("Internal error empty fields");
  }
  return fields[col].name;
}

int MysqlDirectResult::NameToColumn(const base::StringPiece& name) {
  if (!native_result_) {
    throw EmptyRowAccess();
  }
  MYSQL_FIELD* fields = ::mysql_fetch_fields(native_result_);
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
