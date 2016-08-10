#include "db/frontend/session.h"
#include "db/common/connection_info.h"
#include "db/common/connection_manager.h"

namespace db {

Session::Session() {}
Session::~Session() {}


Session::Session(const Session& other)
  : db_connection_(other.db_connection_) {}

const Session& Session::operator=(const Session& other) {
  if (this != &other) {
    db_connection_ = other.db_connection_;
  }
  return *this;
}

Session::Session(scoped_ref_ptr<DBConnection> db_connection)
  : db_connection_(db_connection) {}

Session::Session(scoped_ref_ptr<DBConnection> db_connection,
                 const OnceFunctor& f)
  : db_connection_(db_connection) {
  Once(f);
}
    
Session::Session(const ConnectionInfo& info) {
  Open(info);
}

Session::Session(const std::string& str) {
  Open(str);
}

Session::Session(const ConnectionInfo& info, const OnceFunctor& f) {
  Open(info);
  Once(f);
}

Session::Session(const std::string& str, const OnceFunctor& f) {
  Open(str);
  Once(f);
}

void Session::Open(const std::string& str) {
  db_connection_ = ConnectionManager::GetInstance().Open(str);
}

void Session::Open(const ConnectionInfo& info) {
  db_connection_ = ConnectionManager::GetInstance().Open(info);
}

void Session::Close() {
  db_connection_ = nullptr;
}

bool Session::IsOpen() {
  return db_connection_ != nullptr;
}

Statement Session::Prepare(const std::string& query) {
  DBConnectionThrowGuard g(db_connection_);

  scoped_ref_ptr<DBStatement> db_statement(db_connection_->Prepare(query));
  Statement result(db_statement, db_connection_);
  return result;
}

Statement Session::NewDirectStatement(const std::string& query) {
  DBConnectionThrowGuard g(db_connection_);

  scoped_ref_ptr<DBStatement> db_statement(db_connection_->GetDirectStatement(query));
  Statement result(db_statement, db_connection_);
  return result;
}

Statement Session::NewPreparedStatement(const std::string& query) {
  DBConnectionThrowGuard g(db_connection_);

  scoped_ref_ptr<DBStatement> db_statement(db_connection_->GetPreparedStatement(query));
  Statement result(db_statement, db_connection_);
  return result;
}

Statement Session::NewPreparedUncachedStatement(const std::string& query) {
  DBConnectionThrowGuard g(db_connection_);

  scoped_ref_ptr<DBStatement> db_statement(db_connection_->GetPreparedUncahcedStatement(query));
  Statement result(db_statement, db_connection_);
  return result;
}

Statement Session::operator<<(const std::string& query) {
  return Prepare(query);
}

Statement Session::operator<<(const char* query) {
  return Prepare(query);
}

void Session::Begin() {
  DBConnectionThrowGuard g(db_connection_);
  db_connection_->Begin();
}

void Session::Commit() {
  DBConnectionThrowGuard g(db_connection_);
  db_connection_->Commit();
}

void Session::Rollback() {
  DBConnectionThrowGuard g(db_connection_);
  db_connection_->Rollback();
}

std::string Session::Escape(const char* begin, const char* end) {
  return db_connection_->Escape(begin, end);
}

std::string Session::Escape(const char* str) {
  return db_connection_->Escape(str);
}

std::string Session::Escape(const std::string& str) {
  return db_connection_->Escape(str);
}

std::string Session::Driver() {
  return db_connection_->Driver();
}

std::string Session::Engine() {
  return db_connection_->Engine();
}

bool Session::once_called() const {
  return db_connection_->once_called();
}

void Session::set_once_called(bool v) {
  db_connection_->set_once_called(v);  
}

bool Session::recyclable() const {
  return db_connection_->recyclable();
}

void Session::set_recyclable(bool v) {
  db_connection_->set_recyclable(v);
}

void Session::Once(const OnceFunctor& f) {
  if (!once_called()) {
    f(*this);
    set_once_called(true);
  }
}

void Session::ClearCache() {
  db_connection_->ClearCache();
}


} // namespace db
