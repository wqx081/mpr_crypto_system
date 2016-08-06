#include "db/frontend/transaction.h"

namespace db {

Transaction::Transaction(Session& session)
    : session_(&session), commited_(false) {
  session_->Begin();
}

void Transaction::Commit() {
  session_->Commit();
  commited_ = true;
}

void Transaction::Rollback() {
  if (!commited_) {
    session_->Rollback();
  }
  commited_ = true;
}

Transaction::~Transaction() {
  try {
    Rollback();
  } catch (...) {
  }
}

} // namespace db
