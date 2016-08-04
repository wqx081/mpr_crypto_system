#include "db/driver_manager.h"
#include "db/shared_object.h"
#include "db/backend/connection.h"
#include "db/exception.h"

#include "threading/mutex.h"

#include <vector>
#include <list>

extern "C" {

db::backend::Connection* DbMysqlGetConnection(const db::ConnectionInfo& cs);

} // extern "C"

namespace db {

typedef backend::StaticDriver::ConnectFunctionType ConnectFunctionType;

class SoDriver : public backend::LoadableDriver {
 public:
  SoDriver(const std::string& name, const std::vector<std::string>& so_list) {
    std::string symbol_name = "Db" + name + "GetConnection";
    for (size_t i = 0; i < so_list.size(); ++i) {
      so_ = SharedObject::Open(so_list[i]);
      if (so_) {
       so_->SafeResolve(symbol_name, connect_);
      }
    }
    if (!so_ || connect_) {
      throw DBException("db::Driver failed to load driver " + name + " - no module found");
    } 
  }

  virtual backend::Connection* Open(const ConnectionInfo& connection_info) override {
    return connect_(connection_info);
  }
    
 private:
  ConnectFunctionType connect_;
  SimpleRefPtr<SharedObject> so_;
};

backend::Connection* DriverManager::Connect(const std::string& str) {
  ConnectionInfo conn(str);
  return Connect(conn);
}

backend::Connection* DriverManager::Connect(const ConnectionInfo& connection_info) {
  SimpleRefPtr<backend::Driver> driver;
  DriversType::iterator it;
  {
    threading::Guard g(lock_);
    it = drivers_.find(connection_info.driver);
    if (it != drivers_.end()) {
      driver = it->second;
    } else {
      driver = LoadDriver(connection_info);
      drivers_[connection_info.driver] = driver;
    }
  }
  return driver->Connect(connection_info);
}

void DriverManager::CollectUnused() {
  std::list<SimpleRefPtr<backend::Driver>> garbage;
  {
    threading::Guard g(lock_);
    DriversType::iterator it = drivers_.begin();
    DriversType::iterator tmp;
    while (it != drivers_.end()) {
      if (!it->second->InUse()) {
        garbage.push_back(it->second);
        tmp = it;
        ++it;
        drivers_.erase(tmp);
      } else {
        ++it;
      }
    }
  }
  garbage.clear();
}

SimpleRefPtr<backend::Driver> 
DriverManager::LoadDriver(const ConnectionInfo& connection_info) {
  std::vector<std::string> so_names;
  std::string module;
  std::vector<std::string> search_paths = search_paths_;
  std::string mpath = connection_info.Get("@modules_path");
  if (!mpath.empty()) {
    size_t sep = mpath.find(":");
    search_paths.push_back(mpath.substr(0, sep));
    while (sep < mpath.size()) {
      size_t next = mpath.find(":", sep + 1);
      search_paths.push_back(mpath.substr(sep + 1, next - sep + 1));
      sep = next;
    }
  }
  if (!(module = connection_info.Get("@module")).empty()) {
    so_names.push_back(module);
  } else {
    //TODO
    std::string so_name1; // = "libdb_" + connection_info.driver + ".so";
    std::string so_name2;
    for (size_t i = 0; i < search_paths.size(); ++i) {
      so_names.push_back(search_paths[i] + "/" + so_name1);
      so_names.push_back(search_paths[i] + "/" + so_name2);
    }
    if (!no_default_directory_) {
      so_names.push_back(so_name1);
      so_names.push_back(so_name2);
    }
  }
  SimpleRefPtr<backend::Driver> driver = new SoDriver(connection_info.driver, so_names);  
  return driver;
}

void DriverManager::InstallDriver(const std::string& name, SimpleRefPtr<backend::Driver> driver) {
  if (!driver) {
    throw DBException("db::DriverManager::InstallDriver: Can't install empty driver");
  }
  threading::Guard g(lock_);
  drivers_[name] = driver;
}

DriverManager::DriverManager()
    : no_default_directory_(false) {}

DriverManager::~DriverManager() {}

void DriverManager::AddSearchPath(const std::string& path) {
  threading::Guard g(lock_);
  search_paths_.push_back(path);
}

void DriverManager::ClearSearchPaths() {
  threading::Guard g(lock_);
  search_paths_.clear();
}

void DriverManager::UseDefaultSearchPath(bool v) {
  threading::Guard g(lock_);
  no_default_directory_ = ! v;
}

// static
DriverManager& DriverManager::GetInstance() {
  static DriverManager instance;
  return instance;
}

namespace {

struct Initializer {
 Initializer() {
   DriverManager::GetInstance();
   DriverManager::GetInstance().
     InstallDriver("mysql", new backend::StaticDriver(DbMysqlGetConnection));
 }
} g_init;

} // namespace

} // namespace db
