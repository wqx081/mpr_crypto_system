#ifndef DB_FRONTEND_TRANSACTION_H_
#define DB_FRONTEND_TRANSACTION_H_
#include "db/frontend/common.h"
#include "db/frontend/session.h"


namespace db {

class Transaction {
 public:
  explicit Transaction(Session& session);
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
