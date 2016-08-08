#include "db/common/driver_manager.h"
#include "db/common/shared_object.h"
#include "db/common/connection_info.h"
#include "threading/mutex.h"

#include <vector>
#include <list>

extern "C" {
db::DBConnection* db_mysql_get_connection(const db::ConnectionInfo& info);
} // extern "C"

namespace db {

using ConnectFunctionType = DBStaticDriver::ConnectFunctionType;

class DBSoDriver : public DBLoadableDriver {
 public:
  DBSoDriver(const std::string& name,
             const std::vector<std::string>& so_list) : connect_fn_(nullptr) {
    std::string symbol_name = "db_" + name + "_get_connection";
    for (size_t i = 0; i < so_list.size(); ++i) {
      so_ = SharedObject::Open(so_list[i]);
      if (so_) {
        so_->SafeResolve(symbol_name, connect_fn_);
	break;
      }
    }
    if (!so_ || !connect_fn_) {
      throw DBException("db::driver failed to load driver "
		      + name + " - no module found");
    }
  }

  virtual DBConnection* Open(const ConnectionInfo& info) override {
    return connect_fn_(info);
  }

 private:
  ConnectFunctionType connect_fn_;
  SharedObjectPtr so_;  
};

DriverManager::DriverManager()
  : no_default_directory_(false) {}

DBConnection* DriverManager::Connect(const std::string& str) {
  ConnectionInfo info(str);
  return Connect(info);
}

DBConnection* DriverManager::Connect(const ConnectionInfo& info) {
  DBDriverPtr driver;
  DBDriversType::iterator it;
  {
    threading::Guard l(lock_);
    it = db_drivers_.find(info.driver);
    if (it != db_drivers_.end()) {
      driver = it->second;
    } else {
      driver = LoadDriver(info);
      db_drivers_[info.driver] = driver;
    }
  }
  return driver->Connect(info);
}

void DriverManager::CollectUnused() {
  std::list<DBDriverPtr> garbage;
  {
    threading::Guard l(lock_);
    DBDriversType::iterator it = db_drivers_.begin();
    DBDriversType::iterator t;
    while (it != db_drivers_.end()) {
      if (!it->second->InUse()) {
        garbage.push_back(it->second);
	t = it;
	++it;
	db_drivers_.erase(t);
      } else {
        ++it;
      }
    }
  }
  garbage.clear();
}

DBDriverPtr DriverManager::LoadDriver(const ConnectionInfo& info) {
  std::vector<std::string> so_names;
  std::string module;
  std::vector<std::string> search_paths = search_paths_;
  std::string mpath = info.Get("@modules_path");

  if (!mpath.empty()) {
    size_t sep = mpath.find(";"); // TODO
    search_paths.push_back(mpath.substr(0, sep));
    while (sep < mpath.size()) {
      size_t next = mpath.find(";", sep + 1);
      search_paths.push_back(mpath.substr(sep + 1, next - sep + 1));
      sep = next;
    }
  }

  if (!(module = info.Get("@module")).empty()) {
    so_names.push_back(module);
  } else {
    std::string so_name1 = "libdb_" + info.driver + ".so";
    std::string so_name2 = "libdb_" + info.driver + ".so";

    for (size_t i = 0; i < search_paths.size(); ++i) {
      so_names.push_back(search_paths[i] + "/" + so_name1);
      so_names.push_back(search_paths[i] + "/" + so_name2);
    }

    if (!no_default_directory_) {
      so_names.push_back(so_name1);
      so_names.push_back(so_name2);
    }
  }

  DBDriverPtr driver = new DBSoDriver(info.driver, so_names);
  return driver;
}

void DriverManager::InstallDriver(const base::StringPiece& name,
		                  DBDriverPtr driver) {
  if (!driver) {
    throw DBException("db::DriverManager::InstallDriver: Can't "
		    "install empty driver");
  }
  threading::Guard l(lock_);
  db_drivers_[name.as_string()] = driver;
}

DriverManager::~DriverManager() {}

void DriverManager::AddSearchPath(const std::string& path) {
  threading::Guard l(lock_);
  search_paths_.push_back(path);
}

void DriverManager::ClearSearchPaths() {
  threading::Guard l(lock_);
  search_paths_.clear();
}

void DriverManager::UseDefaultSearchPath(bool v) {
  threading::Guard l(lock_);
  no_default_directory_ = !v;
}

DriverManager& DriverManager::GetInstance() {
  static DriverManager instance;
  return instance;
}

namespace {

struct Initializer {
  Initializer() {
    DriverManager::GetInstance();
    DriverManager::GetInstance().InstallDriver("mysql", 
		    new DBStaticDriver(db_mysql_get_connection));
  }
} init_;

} // namespace

} // namespace db
