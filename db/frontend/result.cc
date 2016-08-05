#include "db/frontend/result.h"

namespace db {

Result::Result()
  : eof_(false),
    fetched_(false),
    current_column_(0) {}

Result::Result(SimpleRefPtr<backend::Result> result,
               SimpleRefPtr<backend::Statement> statement,
               SimpleRefPtr<backend::Connection> connection)
    : eof_(false), 
      fetched_(false), 
      current_column_(0),
      result_(result),
      statement_(statement),
      connection_(connection) {}

Result::Result(const Result& other)
  : eof_(other.eof_),
    fetched_(other.fetched_),
    current_column_(other.current_column_),
    result_(other.result_),
    statement_(other.statement_),
    connection_(other.connection_) {}

const Result& Result::operator=(const Result& other) {
  if (this != &other) {
    eof_ = other.eof_;
    fetched_ = other.fetched_;
    current_column_ = other.current_column_;
    result_ = other.result_;
    statement_ = other.statement_;
    connection_ = other.connection_;
  }
  return *this;
}

Result::~Result() {
  Clear();
}

int Result::Columns() {
  return result_->Columns();
}

bool Result::Next() {

  ConnectionThrowGuard g(connection_);  

  if (eof_) {
    return false;
  }
  fetched_ = true;
  eof_ = result_->Next() == false;
  current_column_ = 0;
  return !eof_;
}

int Result::Index(const std::string& name) {
  int col = result_->NameToColumn(name);
  if (col < 0) {
    throw InvalidColumn();
  }
  return col;
}

std::string Result::Name(int col) {
  if (col < 0 || col >= Columns()) {
    throw InvalidColumn();
  }
  return result_->ColumnToName(col);
}

int Result::FindColumn(const std::string& name) {
  int col = result_->NameToColumn(name);
  if (col < 0) {
    return -1;
  }
  return col;
}

void Result::RewindColumn() {
  current_column_ = 0;
}

bool Result::Empty() {
  if (!result_) {
    return true;
  }
  return eof_ || !fetched_;
}

void Result::Clear() {
  eof_ = true;
  fetched_ = true;
  result_.Reset();
  statement_.Reset();
  connection_.Reset();
}

void Result::Check() {
  if (Empty()) {
    throw EmptyRowAccess();
  }
}

bool Result::IsNull(int col) {
  return result_->IsNull(col);
}

bool Result::IsNull(const std::string& name) {
  return IsNull(Index(name));
}

// Fetchs
bool Result::Fetch(int col, short& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned short& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, int& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned int& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, long& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned long& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, long long& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned long long& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, float& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, double& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, long double& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, std::string& v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, base::Time* v) { return result_->Fetch(col, v); }
bool Result::Fetch(int col, std::ostream& v) { return result_->Fetch(col, v); }

bool Result::Fetch(const std::string& name, short& v) { 
  return result_->Fetch(Index(name), v); 
}
bool Result::Fetch(const std::string& name, unsigned short& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, int& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, unsigned int& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, long& v) { return result_->Fetch(Index(name), v); }
bool Result::Fetch(const std::string& name, unsigned long& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, long long& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, unsigned long long& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, float& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, double& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, long double& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, std::string& v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, base::Time* v) { 
  return result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const std::string& name, std::ostream& v) { 
  return result_->Fetch(Index(name), v); 
}


bool Result::Fetch(short& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned short& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(int& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned int& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(long& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned long& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(long long& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned long long& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(float& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(double& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(long double& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(std::string& v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(base::Time* v) { return result_->Fetch(current_column_++, v); }
bool Result::Fetch(std::ostream& v) { return result_->Fetch(current_column_++, v); }


} // namespace db
