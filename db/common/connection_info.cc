#include "db/common/connection_info.h"
#include "db/common/exception.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <locale>

namespace db {

namespace {

static inline std::string& ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

static inline std::string& rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

static inline std::string trim(std::string s) {
    return ltrim(rtrim(s));
}

} // namespace

// static
void ConnectionInfo::ParseConnectionString(const std::string& cs,
                                           std::string& driver_name,
                                           std::map<std::string, std::string>& params) {
  params.clear();
  auto p = cs.find(':'); 
  if (p == std::string::npos) {
    throw DBException("db::Invalid connection string - no dirver given");
  }
  driver_name = cs.substr(0, p);
  p++;
  while (p < cs.size()) {
    size_t n = cs.find('=', p);
    if (n == std::string::npos) {
      throw DBException("db::Invalid connection string - invalid property");
    }

    std::string key = trim(cs.substr(p, n-p));
    p = n + 1;
    std::string value;
    while (p < cs.size() && ::isspace(cs[p])) {
      ++p;
    }
    if (p >= cs.size()) {
      // nothing to do
    } else if (cs[p] == '\'') {
      p++;
      while (true) {
        if (p >= cs.size()) {
          throw DBException("db::Invalid connection string unterminated string");
        }
        if (cs[p] == '\'') {
          if (p + 1 < cs.size() && cs[p + 1] == '\'') {
            value += '\'';
            p += 2;
          } else {
            p++;
            break;
          }
        } else {
          value += cs[p];
          p++;
        }
      }
    } else {
      size_t n = cs.find(';', p);
      if (n == std::string::npos) {
        value = trim(cs.substr(p));
        p = cs.size();
      } else {
        value = trim(cs.substr(n, n - p));
        p = n;
      }
    }
    if (params.find(key) != params.end()) {
      throw DBException("db::Invalid connection string duplicate key");
    }
    params[key] = value;
    while (p < cs.size()) {
      char c = cs[p];
      if (::isspace(c)) {
        ++p;
      } else if (c == ';') {
        ++p;
        break;
      }
    }
  }
}

std::string ConnectionInfo::Get(const std::string& prop,
                                const std::string& default_value) const {
  properties_type::const_iterator it = properties.find(prop);
  if (it == properties.end()) {
    return default_value;
  }
  return it->second;
}

int ConnectionInfo::Get(const std::string& prop, int default_value) const {
  properties_type::const_iterator it = properties.find(prop);
  if (it == properties.end()) {
    return default_value;
  }

  std::istringstream ss;
  ss.imbue(std::locale::classic());
  ss.str(it->second);
  int val;
  ss >> val;
  if (!ss || !ss.eof()) {
    throw DBException("db::connection_info property " + prop + " expected to be integer value");
  } 
  return val;
}

bool ConnectionInfo::Has(const std::string& prop) const {
  return properties.find(prop) != properties.end();
}

} // namespace db
