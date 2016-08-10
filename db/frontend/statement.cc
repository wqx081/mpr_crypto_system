#include "db/frontend/common.h"
#include "db/frontend/statement.h"

namespace db {

Statement::Statement()
  : placeholder_(1) {}

//TODO
Statement::~Statement() {
  db_statement_ = nullptr;
  db_connection_ = nullptr;
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

Statement::Statement(scoped_ref_ptr<DBStatement> db_statement,
                     scoped_ref_ptr<DBConnection> db_connection)
    : placeholder_(1),
      db_statement_(db_statement),
      db_connection_(db_connection) {}

bool Statement::Empty() const {
  return db_statement_ != nullptr;
}

void Statement::Clear() {
  db_statement_ = nullptr;
  db_connection_ = nullptr;
}

void Statement::Reset() {
  DBConnectionThrowGuard g(db_connection_);
  placeholder_ = 1;
  db_statement_->Reset();
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

Statement& Statement::operator<<(void (*manipulator)(Statement &statement)) {
  manipulator(*this);
  return *this;
}

Result Statement::operator<<(Result (*manipulator)(Statement &st)) {
  return manipulator(*this);
}

Statement& Statement::Bind(int32_t v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(uint32_t v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(int64_t v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(uint64_t v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(double v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(const std::string& v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}
Statement& Statement::Bind(const char* v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(const char* begin, const char* end) {
  db_statement_->Bind(placeholder_++, begin, end);
  return *this;
}

Statement& Statement::Bind(std::istream& v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::Bind(const base::Time& v) {
  db_statement_->Bind(placeholder_++, v);
  return *this;
}

Statement& Statement::BindNull() {
  db_statement_->BindNull(placeholder_++);
  return *this;
}

////////
void Statement::Bind(int col, const std::string& v) {
  db_statement_->Bind(col, v);
}

void Statement::Bind(int col, const char* s) {
  db_statement_->Bind(col, s);
}

void Statement::Bind(int col, const char* begin, const char* end) {
  db_statement_->Bind(col, begin, end);
}

void Statement::Bind(int col, std::istream& v) {
  db_statement_->Bind(col, v);
}

void Statement::Bind(int col, const base::Time& v) {
  db_statement_->Bind(col, v);
}

void Statement::Bind(int col, int32_t v) { db_statement_->Bind(col, v); }
void Statement::Bind(int col, uint32_t v) { db_statement_->Bind(col, v); }
void Statement::Bind(int col, int64_t v) { db_statement_->Bind(col, v); }
void Statement::Bind(int col, uint64_t v) { db_statement_->Bind(col, v); }
void Statement::Bind(int col, double v) { db_statement_->Bind(col, v); }

void Statement::BindNull(int col) {
  db_statement_->BindNull(col);
}

int64_t Statement::LastInsertId() {
  DBConnectionThrowGuard g(db_connection_);
  return db_statement_->SequenceLast(std::string());
}

int64_t Statement::SequenceLast(const std::string& seq) {
  DBConnectionThrowGuard g(db_connection_);
  return db_statement_->SequenceLast(seq);
}

uint64_t Statement::Affected() {
  DBConnectionThrowGuard g(db_connection_);
  return db_statement_->Affected();
}

Result Statement::Row() {
  DBConnectionThrowGuard g(db_connection_);

  scoped_ref_ptr<DBResult> res = db_statement_->Query();
  Result result(res, db_statement_, db_connection_);
  if (result.Next()) {
    if (result.db_result_->HasNext() == DBResult::NextRowStatus::kNextRowExists) {
      g.Done();
      throw MultipleRowsQuery();
    }
  }
  return result;
}

Result Statement::Query() {
  DBConnectionThrowGuard g(db_connection_);
  scoped_ref_ptr<DBResult> res(db_statement_->Query());
  return Result(res, db_statement_, db_connection_);
}

Statement::operator Result() {
  return Query();
}

void Statement::Execute() {
  DBConnectionThrowGuard g(db_connection_);
  db_statement_->Execute();
}

} // namespace db
