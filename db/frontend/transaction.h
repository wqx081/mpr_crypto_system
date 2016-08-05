#ifndef DB_FRONTEND_TRANSACTION_H_
#define DB_FRONTEND_TRANSACTION_H_
#include "base/macros.h"
#include "db/frontend/frontend.h"
#include "db/frontend/session.h"

namespace db {

class Transaction final {
 public:
  Transaction(Session& session);
  ~Transaction();

  void Commit();
  void Rollback();

 private:
  Session* session_;
  bool commited_;

  DISALLOW_COPY_AND_ASSIGN(Transaction);
};

} // namespace db
#endif // DB_FRONTEND_TRANSACTION_H_
