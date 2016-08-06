#ifndef DB_COMMON_DRIVER_MANAGER_H_
#define DB_COMMON_DRIVER_MANAGER_H_
#include "base/macros.h"

#include "db/common/simple_ref_counted.h"
#include "db/common/connection_info.h"

#include "db/backend/db_connection.h"
#include "db/backend/db_driver.h"

#include "threading/mutex.h"
#include <map>
#include <string>
#include <vector>

namespace db {

class DriverManager {
 public:
  static DriverManager& GetInstance();

 private:
  using DBDriversType = std::map<std::string, DBDriverPtr>;

  DriverManager();
  ~DriverManager();

  DBDriverPtr LoadDriver(const ConnectionInfo& info);
  std::vector<std::string> search_paths_;
  bool no_default_directory_;

  threading::Mutex lock_;
  DBDriversType db_drivers_;

  DISALLOW_COPY_AND_ASSIGN(DriverManager);
};

} // namespace db
#endif // DB_COMMON_DRIVER_MANAGER_H_
