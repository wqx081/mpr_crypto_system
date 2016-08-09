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

bool MysqlDirectResult::Fetch(int col, int16_t& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, uint16_t& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, int32_t& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, uint32_t& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, int64_t& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, uint64_t& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, float& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, double& v) {
  return DoFetch(col, v);
}

bool MysqlDirectResult::Fetch(int col, std::string& v) {
  size_t len;
  const char* str = At(col, len);
  if (!str) {
    return false;
  }
  v.assign(str, len);
  return true;
}

bool MysqlDirectResult::Fetch(int col, std::ostream& out) {
  size_t len;
  const char* str = At(col, len);
  if (!str) {
    return false;
  }
  out.write(str, len);
  return true;
}

bool MysqlDirectResult::Fetch(int col, base::Time* v) {
  size_t len;
  const char* str = At(col, len);
  if (!str) {
    return false;
  }
  std::string t(str, len);
  return base::Time::FromString(t.c_str(), v);
}

bool MysqlDirectResult::IsNull(int col) {
  return At(col) == nullptr;
}

int MysqlDirectResult::Columns() {
  return columns_;
}

std::string MysqlDirectResult::ColumnToName(int col) {
  DCHECK(col >= 0 && col <= columns_);  
  DCHECK(native_result_);
  
  MYSQL_FIELD* fields = ::mysql_fetch_fields(native_result_);
  DCHECK(fields);
  return fields[col].name;
}

int MysqlDirectResult::NameToColumn(const std::string& name) {
  DCHECK(native_result_);
  MYSQL_FIELD* fields = ::mysql_fetch_fields(native_result_);
  DCHECK(fields);
  for (int i=0; i < columns_; ++i) {
    if (name == fields[i].name) {
      return i;
    }
  }
  return -1;
}

} //
