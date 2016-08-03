#ifndef BASE_LOCATION_H_
#define BASE_LOCATION_H_
#include <stddef.h>
#include <cassert>
#include <string>

namespace base {

class Location {
 public:
  Location(const char* function_name,
           const char* file_name,
           int line_number);
  Location();
  Location(const Location& other);

  bool operator==(const Location& other) const {
    return line_number_ == other.line_number_ &&
           file_name_ == other.file_name_;
  }

  const char* function_name() const { return function_name_; }
  const char* file_name() const { return file_name_; }
  int line_number() const { return line_number_; }

  std::string ToString() const;

  void Write(bool display_filename, bool display_function_name, std::string* output) const;
  void WriteFunctionName(std::string* output) const;

 private:
  const char* function_name_;
  const char* file_name_;
  int line_number_;
};

#if 0
struct LocationSnapshot {
  LocationSnapshot();
  explicit LocationSnapshot(const base::Location& location);
  ~LocationSnapshot();

  std::string file_name;
  std::string function_name;
  int line_number;
};
#endif

#define FROM_HERE_WITH_FUNCTION(function_name) \
  ::base::Location(function_name,              \
                   __FILE__,                   \
                   __LINE__)

#define FROM_HERE FROM_HERE_WITH_FUNCTION(__func__)

} // namespace base
#endif // BASE_LOCATION_H_
