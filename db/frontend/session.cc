#include "db/frontend/session.h"
#include "db/connections_manager.h"
#include "db/connection_pool.h"

namespace db {

Session::Session() {}

Session::Session(const Session& other)
  : connection_(other.connection_) {}

const Session& Session::operator=(const Session& other) {
  if (this != &other) {
    connection_ = other.connection_;
  }
  return *this;
}

Session::Session(SimpleRefPtr<backend::Connection> connection)
  : connection_(connection) {}

Session::Session(SimpleRefPtr<backend::Connection> connection,
                 const OnceFunctor& f)
  : connection_(connection) {
  Once(f);
}

Session::Session(const ConnectionInfo& connection_info) {
  Open(connection_info);
}

Session::Session(const std::string& connection_str) {
  Open(connection_str);
}

Session::Session(const ConnectionInfo& connection_info, const OnceFunctor& f) {
  Open(connection_info);
  Once(f);
}

Session::Session(const std::string& connection_str, const OnceFunctor& f) {
  Open(connection_str);
  Once(f);
}

Session::~Session() {}


void Session::Open(const ConnectionInfo& connection_info) {
  connection_ = ConnectionsManager::GetInstance().Open(connection_info);
}

void Session::Open(const std::string& connection_str) {
  connection_ = ConnectionsManager::GetInstance().Open(connection_str);
}

void Session::Close() {
  connection_.Reset();
}

bool Session::IsOpen() {
  return connection_;
}

Statement Session::Prepare(const std::string& query) {
  ConnectionThrowGuard g(connection_);
  SimpleRefPtr<backend::Statement> statement_ptr(connection_->Prepare(query));
  Statement statement(statement_ptr, connection_);
  return statement;
}

Statement Session::NewStatement(const std::string& query) {
  ConnectionThrowGuard g(connection_);
  SimpleRefPtr<backend::Statement> statement_ptr(connection_->GetStatement(query));
  Statement statement(statement_ptr, connection_);
  return statement;
}

Statement Session::NewPreparedStatement(const std::string& query) {
  ConnectionThrowGuard g(connection_);
  SimpleRefPtr<backend::Statement> statement_ptr(connection_->GetPreparedStatement(query));
  Statement statement(statement_ptr, connection_);
  return statement;
}

Statement Session::NewPreparedUncachedStatement(const std::string& query) {
  ConnectionThrowGuard g(connection_);
  SimpleRefPtr<backend::Statement> statement_ptr(connection_->GetPreparedUncachedStatement(query));
  Statement statement(statement_ptr, connection_);
  return statement;
}

Statement Session::operator<<(const std::string& query) {
  return Prepare(query);
}

Statement Session::operator<<(const char* query) {
  return Prepare(query);
}

void Session::Begin() {
  ConnectionThrowGuard g(connection_);
  connection_->Begin();
}

void Session::Commit() {
  ConnectionThrowGuard g(connection_);
  connection_->Commit();
}

void Session::Rollback() {
  ConnectionThrowGuard g(connection_);
  connection_->Rollback();
}

std::string Session::Escape(const base::StringPiece& s) {
  return connection_->Escape(s);
}

std::string Session::Escape(const char* b, const char* e) {
  return connection_->Escape(b, e);
}

std::string Session::Driver() {
  return connection_->Driver();
}

std::string Session::Engine() {
  return connection_->Engine();
}

bool Session::OnceCalled() {
  return connection_->OnceCalled();
}

void Session::SetOnceCalled(bool v) {
  connection_->SetOnceCalled(v);
}

void Session::Once(const OnceFunctor& f) {
  if (!OnceCalled()) {
    f(*this);
    SetOnceCalled(true);
  }
}

void Session::ClearCache() {
  connection_->ClearCache();
}

void Session::ClearConnectionPool() {
  connection_->ClearCache();
  connection_->SetRecyclable(false);
  connection_->GetPool()->Clear();
}

bool Session::Recyclable() {
  return connection_->Recyclable();
}

void Session::SetRecyclable(bool v) {
  connection_->SetRecyclable(v);
}

ConnectionData* Session::GetConnectionData(const std::type_info& type) {
  return connection_->GetConnectionData(type);
}

ConnectionData* Session::ReleaseConnectionData(const std::type_info& type) {
  return connection_->ReleaseConnectionData(type);
}

void Session::SetConnectionData(const std::type_info& type, ConnectionData* data) {
  connection_->ResetConnectionData(type, data);
}

} // namespace db
