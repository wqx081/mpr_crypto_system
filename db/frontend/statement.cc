#include "db/frontend/common.h"
#include "db/frontend/result.h"
#include "db/frontend/statement.h"

#include "db/backend/db_statement.h"
#include "db/backend/db_connection.h"

namespace db {

Statement::Statement()
  : placeholder_(1) {}

Statement::~Statement() {
  db_statement_.Reset();
  db_connection_.Reset();
}

Statement::Statement(const Statement& other)
  : placeholder_(other.placeholder_),
    db_statement_(other.db_statement_),
    db_connection_(other.db_connection_) {}

const Statement& Statement::operator=(const Statement& other) {
  if (this != &other) {
    placeholder_ = other.placeholder_;
    db_statement_ = other.db_statement_;
    db_connection_ = other.db_connection_;
  }
  return *this;
}

Statement::Statement(DBStatementPtr statement,
                     DBConnectionPtr connection)
  : placeholder_(1),
    db_statement_(statement),
    db_connection_(connection) {}


bool Statement::Empty() const {
  return !db_statement_;
}

void Statement::Clear() {
  db_statement_.Reset();
  db_connection_.Reset();
}

void Statement::Reset() {
  DBConnectionThrowGuard g(db_connection_);
  placeholder_ = 1;
  db_statement_->Reset();
}

Statement& Statement::operator<<(const base::StringPiece& str) {
  return Bind(str);
}

Statement& Statement::operator<<(const base::Time& v) {
  return Bind(v);
}

Statement& Statement::operator<<(std::istream& v) {
  return Bind(v);
}

Statement& Statement::operator<<(void (*manipulator)(Statement &st)) {
  manipulator(*this);
  return *this;
}

Result Statement::operator<<(Result (*manipulator)(Statement &st)) {
  return manipulator(*this);
}

Statement& Statement::Bind(int v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(unsigned int v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(long v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(unsigned long v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(long long v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(unsigned long long v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(double v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(long double v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(const base::StringPiece& v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(const char* begin, const char* end) {
  db_statement_->Bind(placeholder_++, begin, end);
  return *this;
}

Statement& Statement::Bind(const base::Time& v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(std::istream& v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::BindNull() {
  db_statement_->BindNull(placeholder_++);
  return *this;
}

///////////////
//
void Statement::Bind(int col, const base::StringPiece& v) {
  db_statement_->Bind(col, v);
}

void Statement::Bind(int col, const char* begin, const char* end) {
  db_statement_->Bind(col, begin, end);
}

void Statement::Bind(int col, const base::Time& v) {
  db_statement_->Bind(col, v);
}

void Statement::Bind(int col, std::istream& v) {
  db_statement_->Bind(col, v);
}

void Statement::Bind(int col, int v) {
  db_statement_->Bind(col, v);
}
void Statement::Bind(int col, unsigned int v) {
  db_statement_->Bind(col, v);
}
void Statement::Bind(int col, long v) {
  db_statement_->Bind(col, v);
}
void Statement::Bind(int col, unsigned long v) {
  db_statement_->Bind(col, v);
}
void Statement::Bind(int col, long long v) {
  db_statement_->Bind(col, v);
}
void Statement::Bind(int col, unsigned long long v) {
  db_statement_->Bind(col, v);
}
void Statement::Bind(int col, double v) {
  db_statement_->Bind(col, v);
}
void Statement::Bind(int col, long double v) {
  db_statement_->Bind(col, v);
}

void Statement::BindNull(int col) {
  db_statement_->BindNull(col);
}

long long Statement::LastInsertId() {
  DBConnectionThrowGuard g(db_connection_);
  return db_statement_->SequenceLast(std::string());
}

long long Statement::SequenceLast(const base::StringPiece& str) {
  DBConnectionThrowGuard g(db_connection_);
  return db_statement_->SequenceLast(str); 
}

unsigned long long Statement::Affected() {
  DBConnectionThrowGuard g(db_connection_);
  return db_statement_->Affected();
}

Result Statement::Row() {
  DBConnectionThrowGuard g(db_connection_);
  DBResultPtr db_result = db_statement_->Query();
  Result result(db_result, db_statement_, db_connection_);
  if (result.Next()) {
    if (result.db_result_->HasNext() == DBResult::kNextRowExists) {
      g.Done();
      throw MultipleRowsQuery();
    }
  }
  return result;
}

Result Statement::Query() {
  DBConnectionThrowGuard g(db_connection_);
  DBResultPtr result(db_statement_->Query());
  return Result(result, db_statement_, db_connection_);
}

Statement::operator Result() {
  return Query();
}

void Statement::Execute() {
  DBConnectionThrowGuard g(db_connection_);
  db_statement_->Execute();
}


} // namespace db
