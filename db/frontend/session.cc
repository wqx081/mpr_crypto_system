#include "db/frontend/common.h"
#include "db/frontend/session.h"
#include "db/common/connection_manager.h"

namespace db {

Session::Session() {}

Session::Session(const Session& other)
    : db_connection_(other.db_connection_) {}

const Session& Session::operator=(const Session& other) {
  if (this != &other) {
    db_connection_ = other.db_connection_;
  }
  return *this;
}

Session::Session(DBConnectionPtr connection)
  : db_connection_(connection) {}

Session::Session(DBConnectionPtr connection, const OnceFunctor& f)
  : db_connection_(connection) {
  Once(f);  
}

Session::~Session() {}

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

void Session::Open(const ConnectionInfo& info) {
  db_connection_ = ConnectionsManager::GetInstance().Open(info);
}

void Session::Open(const std::string& str) {
  db_connection_ = ConnectionsManager::GetInstance().Open(str);
}

void Session::Close() {
  db_connection_.Reset();
}

bool Session::IsOpen() {
  return db_connection_;
}

Statement Session::Prepare(const std::string& query) {
  DBConnectionThrowGuard g(db_connection_);
  DBStatementPtr ptr(db_connection_->Prepare(query));
  Statement statement(ptr, db_connection_);
  return statement;
}

Statement Session::NewDirectStatement(const base::StringPiece& query) {
  DBConnectionThrowGuard g(db_connection_);
  DBStatementPtr statement(db_connection_->GetDirectStatement(query.as_string()));
  Statement result(statement, db_connection_);
  return result;
}

Statement Session::NewPrepareStatement(const base::StringPiece& query) {
  DBConnectionThrowGuard g(db_connection_);
  DBStatementPtr statement(db_connection_->GetPrepareStatement(query.as_string()));
  Statement result(statement, db_connection_);
  return result;
}

Statement Session::NewPrepareUncachedStatement(const base::StringPiece& query) {
  DBConnectionThrowGuard g(db_connection_);
  DBStatementPtr statement(db_connection_->GetPreparedUncachedStatement(query.as_string()));
  Statement result(statement, db_connection_);
  return result;
}

Statement Session::operator<<(const base::StringPiece& query) {
  return Prepare(query.as_string());
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

std::string Session::Escape(const base::StringPiece& str) {
  return db_connection_->Escape(str);
}

std::string Session::Escape(const char* begin, const char* end) {
  return db_connection_->Escape(begin, end);
}

std::string Session::Driver() {
  return db_connection_->Driver();
}

std::string Session::Engine() {
  return db_connection_->Engine();
}

void Session::SetOnceCalled(bool v) {
  db_connection_->SetOnceCalled(v);
}

bool Session::OnceCalled() {
  return db_connection_->OnceCalled();
}

void Session::Once(const OnceFunctor& f) {
  if (!OnceCalled()) {
    f(*this);
    SetOnceCalled(true);
  }
}

void Session::ClearCache() {
  db_connection_->ClearCache();
}

void Session::ClearConnectionPool() {
  db_connection_->ClearCache();
  db_connection_->SetRecyclable(false);
  db_connection_->GetConnectionPool()->Clear();
}

void Session::SetRecyclable(bool v) {
  db_connection_->SetRecyclable(v);  
}

bool Session::Recyclable() {
  return db_connection_->Recyclable();
}

ConnectionData* Session::GetConnectionData(const std::type_info& type) {
  return db_connection_->GetConnectionData(type);
}

ConnectionData* Session::ReleaseConnectionData(const std::type_info& type) {
  return db_connection_->ReleaseConnectionData(type);
}

void Session::ResetConnectionData(const std::type_info& type,
		                  ConnectionData* data) {
  db_connection_->ResetConnectionData(type, data);
}


} // namespace db
