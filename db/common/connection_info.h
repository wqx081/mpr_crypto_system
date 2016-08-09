#ifndef DB_COMMON_CONNECTION_INFO_H_
#define DB_COMMON_CONNECTION_INFO_H_
#include <string>
#include <map>
#include <memory>

namespace db {

class ConnectionInfo {
 public:
  ConnectionInfo() {}
  explicit ConnectionInfo(const std::string& str)
    : connection_string(str) {
    Init();
  }

  bool Has(const std::string& key) const;
  std::string Get(const std::string& key, const std::string& default_string=std::string()) const;
  int Get(const std::string& key, int default_value) const;

  using PropertiesType = std::map<std::string, std::string>;
  PropertiesType properties;
  std::string connection_string;
  std::string driver;

 private:
  void Init();

};

} // namespace db
#endif // DB_COMMON_CONNECTION_INFO_H_
