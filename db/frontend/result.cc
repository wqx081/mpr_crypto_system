#include "db/frontend/common.h"
#include "db/frontend/result.h"

namespace db {

Result::Result()
  : eof_(false), fetched_(false), current_column_(0) {}

Result::Result(DBResultPtr result,
               DBStatementPtr statement,
               DBConnectionPtr connection)
  : eof_(false), fetched_(false), current_column_(0),
    db_result_(result),
    db_statement_(statement),
    db_connection_(connection) {}

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
  DBConnectionThrowGuard g(db_connection_);

  if (eof_) {
    return false;
  }

  fetched_ = true;
  eof_ = db_result_->Next() == false;
  current_column_ = 0;
  return !eof_;
}

int Result::Index(const base::StringPiece& str) {
  int col = db_result_->NameToColumn(str);
  if (col < 0) {
    throw InvalidColumn();
  }
  return col;
}

std::string Result::Name(int col) {
  if (col < 0 || col >= Columns()) {
    throw InvalidColumn();
  }
  return db_result_->ColumnToName(col);
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
  db_result_.Reset();
  db_statement_.Reset();
  db_connection_.Reset();
}

void Result::Check() {
  if (Empty()) {
    throw EmptyRowAccess();
  }
}

bool Result::IsNull(int col) {
  return db_result_->IsNull(col);
}

bool Result::IsNull(const base::StringPiece& str) {
  return IsNull(Index(str));
}

bool Result::Fetch(int col, short& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned short& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, int& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned int& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, long& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned long& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, long long& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, unsigned long long& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, float& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, double& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, long double& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, std::string& v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, base::Time* v) { return db_result_->Fetch(col, v); }
bool Result::Fetch(int col, std::ostream& v) { return db_result_->Fetch(col, v); }

bool Result::Fetch(const base::StringPiece& name, 
                   short& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   unsigned short& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   int& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   unsigned int& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   long& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   unsigned long& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   long long& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   unsigned long long& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   float& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   double& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   long double& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   std::string& v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   base::Time* v) { 
  return db_result_->Fetch(Index(name), v); 
}

bool Result::Fetch(const base::StringPiece& name, 
                   std::ostream& v) { 
  return db_result_->Fetch(Index(name), v); 
}


bool Result::Fetch(short& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned short& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(int& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned int& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(long& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned long& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(long long& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(unsigned long long& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(float& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(double& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(long double& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(std::string& v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(base::Time* v) { return db_result_->Fetch(current_column_++, v); }
bool Result::Fetch(std::ostream& v) { return db_result_->Fetch(current_column_++, v); }


} // namespace db
