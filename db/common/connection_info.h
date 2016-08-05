#ifndef DB_COMMON_CONNECTION_INFO_H_
#define DB_COMMON_CONNECTION_INFO_H_
#include <string>
#include <map>

#include "base/time.h"

namespace db {

class ConnectionInfo {
 public:
  static void ParseConnectionString(const std::string& cs,
                                    std::string& driver_name,
                                    std::map<std::string, std::string>& props);
  std::string connection_string;
  std::string driver;

  typedef std::map<std::string, std::string> properties_type;
  properties_type properties;
  
  bool Has(const std::string& prop) const;
  std::string Get(const std::string& prop, const std::string& default_value=std::string()) const;
  int Get(const std::string& prop, int default_value) const;
  
  ConnectionInfo() {}
  explicit ConnectionInfo(const std::string& conn_str)
    : connection_string(conn_str) {
    ConnectionInfo::ParseConnectionString(connection_string, driver, properties);
  }
};

} // namespace db
#endif // DB_CONNECTION_INFO_H_
