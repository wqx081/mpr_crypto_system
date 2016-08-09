#include "db/common/connection_info.h"

#include "base/string_piece.h"
#include "base/string_util.h"

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <locale>

#include <glog/logging.h>

namespace db {

namespace {
static const base::StringPiece kTrimWhiteSpace = " \t\r\n\f";
size_t knpos = std::string::npos;

} // namespace 

void ConnectionInfo::Init() {
  properties.clear();

  // get dirver
  size_t pos = connection_string.find(':');
  CHECK_NE(pos, knpos) << "db:Invalid connection string - no dirver";
  driver = connection_string.substr(0, pos);  
  pos++;

  while (pos < connection_string.size()) {
    size_t n = connection_string.find('=', pos);
    CHECK_NE(n, knpos) << "db:Invalid connection string - invalid property";
    std::string key;
    base::TrimString(connection_string.substr(pos, n - pos),
                     kTrimWhiteSpace,
                     &key);
    pos = n + 1;
    std::string value;
    while (pos < connection_string.size() && 
           kTrimWhiteSpace.find(connection_string[pos]) != knpos) {
      pos++;
    }
    if (pos >= connection_string.size()) {
    // nothing
    } else if (connection_string[pos] == '\'') {
      pos++;
      while (true) {
        DCHECK(pos < connection_string.size()) << "db:Invalid connection string "
                    "- unterminated string";
        if (connection_string[pos] == '\'') {
          if (pos + 1 < connection_string.size() &&
              connection_string[pos + 1] == '\'') {
            value += '\'';
            pos += 2;
          } else {
            pos++;
            break;
          } 
        } else {
          value += connection_string[pos];
          pos++;
        }
      }
    } else {
      size_t n = connection_string.find(';', pos);
      if (n == knpos) {
        base::TrimString(connection_string.substr(pos),
                         kTrimWhiteSpace,
                         &value);
        pos = connection_string.size();
      } else {
        base::TrimString(connection_string.substr(pos, n - pos),
                         kTrimWhiteSpace,
                         &value);
        pos = n;
      }
    }
    // if
    DCHECK(properties.find(key) == properties.end()) << "db:invalid connection string - "
           "duplicate key";
    properties[key] = value;
    while (pos < connection_string.size()) {
      char c = connection_string[pos];
      if (kTrimWhiteSpace.find(c) != knpos) {
        ++pos;
      } else {
        ++pos;
        break;
      }
    }
  }
}

std::string ConnectionInfo::Get(const std::string& key, 
                                const std::string& default_value) const {
  PropertiesType::const_iterator it = properties.find(key);
  if (it == properties.end()) {
    return default_value;
  } else {
    return it->second;
  }
}

int ConnectionInfo::Get(const std::string& key, int default_value) const {
  PropertiesType::const_iterator it = properties.find(key);  
  if (it == properties.end()) {
    return default_value;
  }
  std::istringstream ss;
  ss.imbue(std::locale::classic());
  ss.str(it->second);
  int value;
  ss >> value;
  //TODO
  return value;  
}

bool ConnectionInfo::Has(const std::string& key) const {
  return properties.find(key) != properties.end();
}

} // namespace db

