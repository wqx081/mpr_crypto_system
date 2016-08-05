#include "db/frontend/statement.h"

namespace db {

Statement::Statement()
  : placeholder_(1) {}

Statement::Statement(const Statement& other)
  : placeholder_(other.placeholder_),
    statement_(other.statement_),
    connection_(other.connection_) {}

const Statement& Statement::operator=(const Statement& other) {
  if (this != &other) {
    placeholder_ = other.placeholder_;
    statement_ = other.statement_;
    connection_ = other.connection_;
  }
  return *this;
}

Statement::Statement(SimpleRefPtr<backend::Statement> statement,
                     SimpleRefPtr<backend::Connection> connection)
    : placeholder_(1),
      statement_(statement),
      connection_(connection) {}

Statement::~Statement() {
  statement_.Reset();
  connection_.Reset();
}

bool Statement::Empty() const {
  return !statement_;
}

void Statement::Clear() {
  statement_.Reset();
  connection_.Reset();
}

void Statement::Reset() {
  ConnectionThrowGuard g(connection_);
  placeholder_ = 1;
  statement_->Reset();
}

Statement& Statement::operator<<(const std::string& v) {
  return Bind(v);
}

Statement& Statement::operator<<(const char* s) {
  return Bind(s);
}

Statement& Statement::operator<<(const base::Time& v) {
  return Bind(v);
}

Statement& Statement::operator<<(std::istream& v) {
  return Bind(v);
}

Statement& Statement::operator<<(void (*manipulator)(Statement& statement)) {
  manipulator(*this);
  return *this;
}

Result Statement::operator<<(Result(*manipulator)(Statement& statement)) {
  return manipulator(*this);
}

Statement& Statement::Bind(int v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(unsigned int v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(long v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(unsigned long v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(long long v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(unsigned long long v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(double v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(long double v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(const base::StringPiece& v) {
  statement_->Bind(placeholder_++, v.as_string());
  return *this;
}

Statement& Statement::Bind(const char* b, const char* e) {
  statement_->Bind(placeholder_++, b, e);
  return *this;
}

Statement& Statement::Bind(const base::Time& v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(std::istream& v) {
  statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::BindNull() {
  statement_->BindNull(placeholder_++);
  return *this;
}

//
void Statement::Bind(int col, base::StringPiece& v) {
  statement_->Bind(col, v.as_string());
}

void Statement::Bind(int col, const char* b, const char* e) {
  statement_->Bind(col, b, e);
}

void Statement::Bind(int col, const base::Time& v) {
  statement_->Bind(col, v);
}

void Statement::Bind(int col, std::istream& v) {
  statement_->Bind(col, v);
}

void Statement::Bind(int col, int v) {
  statement_->Bind(col, v);
}
void Statement::Bind(int col, unsigned v) {
  statement_->Bind(col, v);
}
void Statement::Bind(int col, long v) {
  statement_->Bind(col, v);
}
void Statement::Bind(int col, unsigned long v) {
  statement_->Bind(col, v);
}
void Statement::Bind(int col, long long v) {
  statement_->Bind(col, v);
}
void Statement::Bind(int col, unsigned long long v) {
  statement_->Bind(col, v);
}
void Statement::Bind(int col, double v) {
  statement_->Bind(col, v);
}
void Statement::Bind(int col, long double v) {
  statement_->Bind(col, v);
}

void Statement::BindNull(int col) {
  statement_->BindNull(col);
}

long long Statement::LastInsertId() {
  ConnectionThrowGuard g(connection_);
  return statement_->SequenceLast(std::string());
}

long long Statement::SequenceLast(const std::string& seq) {
  ConnectionThrowGuard g(connection_);
  return statement_->SequenceLast(seq);
}

unsigned long long Statement::Affected() {
  ConnectionThrowGuard g(connection_);
  return statement_->Affected();
}

Result Statement::Row() {
  ConnectionThrowGuard g(connection_);
  SimpleRefPtr<backend::Result> result_ptr = statement_->Query();
  Result result(result_ptr, statement_, connection_);
  while (result.Next()) {
    if (result.result_->HasNext() == backend::Result::kNextRowExists) {
      g.Done();
      throw MultipleRowsQuery();
    }
  }
  return result;
}

Result Statement::Query() {
  ConnectionThrowGuard g(connection_);
  SimpleRefPtr<backend::Result> res(statement_->Query());
  return Result(res, statement_, connection_);
}

Statement::operator Result() {
  return Query();
}

void Statement::Execute() {
  ConnectionThrowGuard g(connection_);
  statement_->Execute();
}

} // namespace db
