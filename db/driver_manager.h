#ifndef DB_DRIVER_MANAGER_H_
#define DB_DRIVER_MANAGER_H_
#include "base/macros.h"
#include "threading/mutex.h"

#include "db/simple_ref_counted.h"
#include "db/connection_info.h"

#include <map>
#include <string>
#include <vector>

namespace db {

namespace backend {

class Connection;
class Driver;

} // namespace backend

class DriverManager {
 public:
  static DriverManager& GetInstance();
  void InstallDriver(const std::string& name, SimpleRefPtr<backend::Driver> driver);
  void CollectUnused();
  void AddSearchPath(const std::string& path);
  void ClearSearchPaths();
  void UseDefaultSearchPath(bool v);
  
  backend::Connection* Connect(const ConnectionInfo& connection_info);
  backend::Connection* Connect(const std::string& connection_string);

 private:
  DriverManager();
  ~DriverManager();
  
  SimpleRefPtr<backend::Driver> LoadDriver(const ConnectionInfo& connection_info);
  
  typedef std::map<std::string, SimpleRefPtr<backend::Driver>> DriversType;
  std::vector<std::string> search_paths_;
  bool no_default_directory_;
  DriversType drivers_;
  threading::Mutex lock_;

  DISALLOW_COPY_AND_ASSIGN(DriverManager);
};

} // namespace db
#endif // DB_DRIVER_MANAGER_H_
