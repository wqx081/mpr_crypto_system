#include "db/frontend/result.h"

namespace db {

Result::Result()
  : eof_(false),
    fetched_(false),
    current_column_(0) {}

Result::Result(scoped_ref_ptr<DBResult> db_result,
               scoped_ref_ptr<DBStatement> db_statement,
	       scoped_ref_ptr<DBConnection> db_connection)
  : eof_(false),
    fetched_(false),
    current_column_(0),
    db_result_(db_result),
    db_statement_(db_statement),
    db_connection_(db_connection) {}

Result::Result(const Result& other) 
  : eof_(other.eof_), 
    fetched_(other.fetched_), 
    current_column_(other.current_column_),
    db_result_(other.db_result_),
    db_statement_(other.db_statement_),
    db_connection_(other.db_connection_) {}

const Result& Result::operator=(const Result& other) {
  if (this != &other) {
    eof_ = other.eof_;
    fetched_ = other.fetched_;
    current_column_ = other.current_column_;
    db_result_ = other.db_result_;
    db_statement_ = other.db_statement_;
    db_connection_ = other.db_connection_;
  }
  return *this;
}

Result::~Result() {
  Clear();
}

int Result::Columns() {
  return db_result_->Columns();
}

bool Result::Next() {
  if (eof_) {
    return false;
  }
  fetched_ = true;
  eof_ = db_result_->Next() == false;
  current_column_ = 0;
  return !eof_;
}


int Result::Index(const std::string& name) {
  int col = db_result_->NameToColumn(name);
  DCHECK(col >= 0);
  return col;
}

std::string Result::Name(int col) {
  DCHECK(col >= 0 && col < Columns());
  return db_result_->ColumnToName(col);
}

int Result::FindColumn(const std::string& name) {
  int c = db_result_->NameToColumn(name);
  if (c < 0) 
    return -1;
  return c;
}

void Result::RewindColumn() {
  current_column_ = 0;
}

bool Result::Empty() {
  if (!db_result_) {
    return true;
  }
  return eof_ || !fetched_;
}

void Result::Clear() {
  eof_ = true;
  fetched_ = true;
  db_result_ = nullptr;
  db_statement_ = nullptr;
  db_connection_ = nullptr;
}

void Result::Check() {
  DCHECK(!Empty());
}

bool Result::IsNull(int col) {
  return db_result_->IsNull(col);
}

bool Result::IsNull(const std::string& name) {
  return IsNull(Index(name));
}

bool Result::Fetch(int col, int16_t& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, uint16_t& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, int32_t& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, uint32_t& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, int64_t& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, uint64_t& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, float& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, double& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, std::string& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, std::ostream& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, base::Time* v) { return db_result_->Fetch(col, v); }



bool Result::Fetch(const std::string& name, int16_t& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, uint16_t& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, int32_t& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, uint32_t& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, int64_t& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, uint64_t& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, float& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, double& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, std::string& v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, base::Time* v) {
  return db_result_->Fetch(Index(name), v);
}
bool Result::Fetch(const std::string& name, std::ostream& v) {
  return db_result_->Fetch(Index(name), v);
}


bool Result::Fetch(int16_t& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(uint16_t& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(int32_t& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(uint32_t& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(int64_t& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(uint64_t& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(std::string& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(std::ostream& v) {
  return db_result_->Fetch(current_column_++, v);
}
bool Result::Fetch(base::Time* v) {
  return db_result_->Fetch(current_column_++, v);
}


} // namespace db
