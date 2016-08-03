#include "base/macros.h"
#include "base/location.h"
#include "base/string_printf.h"

#include <string>

namespace base {

Location::Location(const char* function_name,
                   const char* file_name,
                   int line_number)
    : function_name_(function_name),
      file_name_(file_name),
      line_number_(line_number) {}

Location::Location() 
    : function_name_("Unknown"),
      file_name_("Unknown"),
      line_number_(-1) {}

Location::Location(const Location& other) 
    : function_name_ (other.function_name_),
      file_name_(other.file_name_),
      line_number_(other.line_number_) {}


std::string Location::ToString() const {
  return std::string(function_name_) + "@" + file_name_ + ":" + std::to_string(line_number_);
}

void Location::Write(bool display_filename,
                     bool display_function_name,
                     std::string* output) const {
  base::StringAppendF(output, "%s[%d]", display_filename ? file_name_ : "line", line_number_);
  if (display_function_name) {
    WriteFunctionName(output);
    output->push_back(' ');
  }
}

void Location::WriteFunctionName(std::string* output) const {
  for (const char *p = function_name_; *p; p++) {
    switch (*p) {
      case '<': 
        output->append("&lt;");
        break;
      case '>':
        output->append("&gt;");
        break;
      default:
        output->push_back(*p);
        break;
    }
  }
}

} // namespace base
