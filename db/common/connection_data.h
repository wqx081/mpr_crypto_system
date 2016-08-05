#ifndef DB_COMMON_CONNECTION_DATA_H_
#define DB_COMMON_CONNECTION_DATA_H_
#include "base/macros.h"
#include <memory>

namespace db {

class ConnectionData {
 public:
  ConnectionData();
  virtual ~ConnectionData();

 private:
  struct Data;
  std::unique_ptr<Data> data_;

  DISALLOW_COPY_AND_ASSIGN(ConnectionData);
};

} // namespace db
#endif // DB_COMMON_CONNECTION_DATA_H_
