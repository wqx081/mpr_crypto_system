#include "db/drivers/mysql/mysql_prepared_result.h"

namespace db {

const int MysqlPreparedResult::BindData::kBindDataBufferLength;

MysqlPreparedResult::MysqlPreparedResult(MYSQL_STMT* statement)
    : native_statement_(statement),
      current_row_(0),
      meta_(nullptr) {
  columns_ = ::mysql_stmt_field_count(native_statement_);      
  DCHECK(::mysql_stmt_store_result(native_statement_) == 0);
  meta_ = ::mysql_stmt_result_metadata(native_statement_);
  DCHECK(meta_);
}

MysqlPreparedResult::~MysqlPreparedResult() {
  ::mysql_free_result(meta_);
}

void MysqlPreparedResult::Reset() {
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


DBResult::NextRowStatus
MysqlPreparedResult::HasNext() {
  if (current_row_ >= ::mysql_stmt_num_rows(native_statement_)) {
    return kLastRowReached;
  } else {
    return kNextRowExists;
  }
}

bool MysqlPreparedResult::Next() {
  current_row_++;
  Reset();
  if (columns_ > 0) {
    DCHECK(::mysql_stmt_bind_result(native_statement_, &bind_[0]) == 0);
  }
  int r = ::mysql_stmt_fetch(native_statement_);
  if (r == MYSQL_NO_DATA) {
    return false;
  }
  if (r == MYSQL_DATA_TRUNCATED) {
    for (int i = 0; i < columns_; ++i) {
      if (bind_data_[i].error    &&
          !bind_data_[i].is_null &&
	  bind_data_[i].length >= sizeof(bind_data_[i].buf)) {
        bind_data_[i].vbuf.resize(bind_data_[i].length);
	bind_[i].buffer = &bind_data_[i].vbuf.front();
	bind_[i].buffer_length = bind_data_[i].length;
	DCHECK(::mysql_stmt_fetch_column(native_statement_,
				         &bind_[i],
					 i,
					 0) == 0);
	bind_data_[i].ptr = &bind_data_[i].vbuf.front();
      }
    }
  }
  return true;
}

bool MysqlPreparedResult::Fetch(int col, int16_t& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, uint16_t& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, int32_t& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, uint32_t& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, int64_t& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, uint64_t& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, float& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, double& v) {
  return DoFetch(col, v);
}

bool MysqlPreparedResult::Fetch(int col, std::string& v) {
  BindData& bind_data = At(col);
  if (bind_data.is_null) {
    return false;
  }
  v.assign(bind_data.ptr, bind_data.length);
  return true;
}

bool MysqlPreparedResult::Fetch(int col, std::ostream& out) {
  BindData& bind_data = At(col);
  if (bind_data.is_null) {
    return false;
  }
  out.write(bind_data.ptr, bind_data.length);
  return true;
}

bool MysqlPreparedResult::Fetch(int col, base::Time* v) {
  std::string t;
  if (!Fetch(col, t)) {
    return false;
  }
  return ParseTimeString(t.c_str(), v);
}

bool MysqlPreparedResult::IsNull(int col) {
  return At(col).is_null;
}

int MysqlPreparedResult::Columns() {
  return columns_;
}

std::string MysqlPreparedResult::ColumnToName(int col) {
  DCHECK(col >= 0 && col < columns_);
  MYSQL_FIELD* fields = ::mysql_fetch_fields(meta_);
  DCHECK(fields != nullptr);
  return fields[col].name;
}

int MysqlPreparedResult::NameToColumn(const std::string& name) {
  MYSQL_FIELD* fields = ::mysql_fetch_fields(meta_);
  DCHECK(fields != nullptr);
  for (int i = 0; i < columns_; ++i) {
    if (name == fields[i].name) {
      return i;
    }
  }
  return -1;
}


} // namespace db
