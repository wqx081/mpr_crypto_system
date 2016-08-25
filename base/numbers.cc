#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <climits>

#include <assert.h>

#include <float.h>          // for DBL_DIG and FLT_DIG
#include <math.h>           // for HUGE_VAL

#include "base/numbers.h"
#include "base/ascii_ctype.h"

using std::string;

namespace base {


int32_t strto32_adapter(const char *nptr, char **endptr, int base) {
  const int saved_errno = errno; 
  errno = 0;
  const long result = strtol(nptr, endptr, base);
  if (errno == ERANGE && result == std::numeric_limits<long>::min()) {
    return std::numeric_limits<int32_t>::min();
  } else if (errno == ERANGE && result == std::numeric_limits<long>::max()) {
    return std::numeric_limits<int32_t>::max();
  } else if (errno == 0 && result < std::numeric_limits<int32_t>::min()) {
    errno = ERANGE;
    return std::numeric_limits<int32_t>::min();
  } else if (errno == 0 && result > std::numeric_limits<int32_t>::max()) {
    errno = ERANGE;
    return std::numeric_limits<int32_t>::max();
  } 
  if (errno == 0)
    errno = saved_errno;
  return static_cast<int32_t>(result);
} 

uint32_t strtou32_adapter(const char *nptr, char **endptr, int base) {
  const int saved_errno = errno;
  errno = 0;
  const unsigned long result = strtoul(nptr, endptr, base);
  if (errno == ERANGE && result == std::numeric_limits<unsigned long>::max()) {
    return std::numeric_limits<uint32_t>::max();
  } else if (errno == 0 && result > std::numeric_limits<uint32_t>::max()) {
    errno = ERANGE;
    return std::numeric_limits<uint32_t>::max();
  } 
  if (errno == 0)
    errno = saved_errno;
  return static_cast<uint32_t>(result);
} 

//////
static inline bool EatADouble(const char** text, int* len, bool allow_question,
                              double* val, bool* initial_minus,
                              bool* final_period) {
  const char* pos = *text;
  int rem = *len;  // remaining length, or -1 if null-terminated
  
  if (pos == NULL || rem == 0)
    return false;
  
  if (allow_question && (*pos == '?')) {
    *text = pos + 1;
    if (rem != -1)
      *len = rem - 1;
    return true;
  }
  
  if (initial_minus) {
    if ((*initial_minus = (*pos == '-'))) {  // Yes, we want assignment.
      if (rem == 1)
        return false;
      ++pos;
      if (rem != -1)
        --rem;
     }
  }
  
  if (!strchr("-+.0123456789", *pos))
    return false;
      
  char* end_nonconst;
  double retval;
  if (rem == -1) {
    retval = strtod(pos, &end_nonconst);
  } else { 
    std::unique_ptr<char[]> buf(new char[rem + 1]);
    memcpy(buf.get(), pos, rem);
    buf[rem] = '\0';
    retval = strtod(buf.get(), &end_nonconst);
    end_nonconst = const_cast<char*>(pos) + (end_nonconst - buf.get());
  } 
    
  if (pos == end_nonconst)
    return false;
      
  if (final_period) {
    *final_period = (end_nonconst[-1] == '.');
    if (*final_period) {
      --end_nonconst;
    } 
  } 
    
  *text = end_nonconst;
  *val = retval;
  if (rem != -1)
    *len = rem - (end_nonconst - pos);
  return true;
}

static inline char EatAChar(const char** text, int* len,
                            const char* acceptable_chars,
                            bool update, bool null_ok) {
  assert(!(update && null_ok));
  if ((*len == 0) || (**text == '\0'))
    return (null_ok ? '\1' : '\0');  // if null_ok, we're in predicate mode.
  
  if (strchr(acceptable_chars, **text)) {
    char result = **text;
    if (update) {
      ++(*text);
      if (*len != -1)
        --(*len);
    }
    return result;
  }
  
  return '\0';  // no match; no update
}

bool ParseDoubleRange(const char* text, int len, const char** end,
                      double* from, double* to, bool* is_currency,
                      const DoubleRangeOptions& opts) {
  const double from_default = opts.dont_modify_unbounded ? *from : -HUGE_VAL;
  
  if (!opts.dont_modify_unbounded) {
    *from = -HUGE_VAL;
    *to = HUGE_VAL;
  }
  if (opts.allow_currency && (is_currency != NULL))
    *is_currency = false;
  
  assert(len >= -1);
  assert(opts.separators && (*opts.separators != '\0'));
  assert(strlen(opts.separators) ==
    strcspn(opts.separators, "+0123456789eE$"));
  assert(opts.num_required_bounds <= 2);
  
  if (opts.allow_comparators) {
    char comparator = EatAChar(&text, &len, "<>", true, false);
    if (comparator) {
      double* dest = (comparator == '>') ? from : to;
      EatAChar(&text, &len, "=", true, false);
      if (opts.allow_currency && EatAChar(&text, &len, "$", true, false))
        if (is_currency != NULL)
          *is_currency = true;
      if (!EatADouble(&text, &len, opts.allow_unbounded_markers, dest, NULL,
                                                NULL))
        return false;
      *end = text;
      return EatAChar(&text, &len, opts.acceptable_terminators, false,
                                                opts.null_terminator_ok);
    }
  }
  bool seen_dollar = (opts.allow_currency &&
                      EatAChar(&text, &len, "$", true, false));
  bool initial_minus_sign = false;
  bool final_period = false;
  bool* check_initial_minus = (strchr(opts.separators, '-') && !seen_dollar
                                 && (opts.num_required_bounds < 2)) ?
                                (&initial_minus_sign) : NULL;
  bool* check_final_period = strchr(opts.separators, '.') ? (&final_period)
                               : NULL;
  bool double_seen = EatADouble(&text, &len, opts.allow_unbounded_markers,
                                  from, check_initial_minus, check_final_period);

    if ((opts.num_required_bounds == 2) && !double_seen) return false;
  
    if (seen_dollar && !double_seen) {
        --text;
        if (len != -1)
          ++len;
        seen_dollar = false;
    }
   char separator = EatAChar(&text, &len, opts.separators, true, false);
    if (separator == '.') {
      if (EatAChar(&text, &len, ".", true, false)) {
        if (final_period) {
          EatAChar(&text, &len, ".", true, false);
        } 
      } else if (!EatAChar(&text, &len, opts.separators, true, false)) {
        --text;
        if (len != -1)
          ++len;
        separator = '\0';
      } 
    } 
    if (!separator) {
      if (final_period)  // final period now considered part of first double
        EatAChar(&text, &len, ".", true, false);
      if (initial_minus_sign && double_seen) {
        *to = *from;
        *from = from_default;
      } else if (opts.require_separator ||
                                  (opts.num_required_bounds > 0 && !double_seen) ||
                                  (opts.num_required_bounds > 1) ) {
        return false;
      } 
    } else {
      if (initial_minus_sign && double_seen)
        *from = -(*from);
      bool second_dollar_seen = (seen_dollar
                                 || (opts.allow_currency && !double_seen))
                                && EatAChar(&text, &len, "$", true, false);
      bool second_double_seen = EatADouble(
        &text, &len, opts.allow_unbounded_markers, to, NULL, NULL);
      if (opts.num_required_bounds > double_seen + second_double_seen)
        return false;
      if (second_dollar_seen && !second_double_seen) {
        --text;
        if (len != -1)
          ++len;
        second_dollar_seen = false;
      } 
      seen_dollar = seen_dollar || second_dollar_seen;
    } 
    
    if (seen_dollar && (is_currency != NULL))
      *is_currency = true;
    *end = text;
    char terminator = EatAChar(&text, &len, opts.acceptable_terminators, false,
                               opts.null_terminator_ok);
    if (terminator == '.')     
      --(*end);
    return terminator;
} 

void ConsumeStrayLeadingZeroes(string *const str) {
  const string::size_type len(str->size());
    if (len > 1 && (*str)[0] == '0') {
      const char
        *const begin(str->c_str()),
        *const end(begin + len),
        *ptr(begin + 1);
      while (ptr != end && *ptr == '0') {
        ++ptr;
      }
      string::size_type remove(ptr - begin);
      DCHECK_GT(ptr, begin);
      if (remove == len) {
        --remove;  // if they are all zero, leave one...
      }
      str->erase(0, remove);
    }
}

  int32_t ParseLeadingInt32Value(const char *str, int32_t deflt) {
    using std::numeric_limits;
  
    char *error = NULL;
    long value = strtol(str, &error, 0);
    if (value > numeric_limits<int32_t>::max()) {
      value = numeric_limits<int32_t>::max();
    } else if (value < numeric_limits<int32_t>::min()) {
      value = numeric_limits<int32_t>::min();
    }
    return (error == str) ? deflt : value;
  }
  
uint32_t ParseLeadingUInt32Value(const char *str, uint32_t deflt) {
    using std::numeric_limits;
  
    if (numeric_limits<unsigned long>::max() == numeric_limits<uint32_t>::max()) {
      char *error = NULL;
      const uint32_t value = strtoul(str, &error, 0);
      return (error == str) ? deflt : value;
    } else {
      char *error = NULL;
      int64_t value = strto64(str, &error, 0);
      if (value > numeric_limits<uint32_t>::max() ||
                    value < -static_cast<int64_t>(numeric_limits<uint32_t>::max())) {
        value = numeric_limits<uint32_t>::max();
      } 
      return (error == str) ? deflt : value;
    } 
} 

int32_t ParseLeadingDec32Value(const char *str, int32_t deflt) {
    using std::numeric_limits;
    
    char *error = NULL;
    long value = strtol(str, &error, 10);
    if (value > numeric_limits<int32_t>::max()) {
      value = numeric_limits<int32_t>::max();
    } else if (value < numeric_limits<int32_t>::min()) {
      value = numeric_limits<int32_t>::min();
    } 
    return (error == str) ? deflt : value;
}
  
uint32_t ParseLeadingUDec32Value(const char *str, uint32_t deflt) {
    using std::numeric_limits;
  
    if (numeric_limits<unsigned long>::max() == numeric_limits<uint32_t>::max()) {
      char *error = NULL;
      const uint32_t value = strtoul(str, &error, 10);
      return (error == str) ? deflt : value;
    } else {
      char *error = NULL;
      int64_t value = strto64(str, &error, 10);
      if (value > numeric_limits<uint32_t>::max() ||
                    value < -static_cast<int64_t>(numeric_limits<uint32_t>::max())) {
        value = numeric_limits<uint32_t>::max();
      } 
      return (error == str) ? deflt : value;
    } 
} 

  uint64_t ParseLeadingUInt64Value(const char *str, uint64_t deflt) {
    char *error = NULL;
    const uint64_t value = strtou64(str, &error, 0);
    return (error == str) ? deflt : value;
  } 
  
  int64_t ParseLeadingInt64Value(const char *str, int64_t deflt) {
    char *error = NULL;
    const int64_t value = strto64(str, &error, 0);
    return (error == str) ? deflt : value;
  } 
  
  uint64_t ParseLeadingHex64Value(const char *str, uint64_t deflt) {
    char *error = NULL;
    const uint64_t value = strtou64(str, &error, 16);
    return (error == str) ? deflt : value;
  }

  int64_t ParseLeadingDec64Value(const char *str, int64_t deflt) {
    char *error = NULL;
    const int64_t value = strto64(str, &error, 10);
    return (error == str) ? deflt : value;
  }
  
uint64_t ParseLeadingUDec64Value(const char *str, uint64_t deflt) {
  char *error = NULL;
  const uint64_t value = strtou64(str, &error, 10);
  return (error == str) ? deflt : value;
}

  double ParseLeadingDoubleValue(const char *str, double deflt) {
    char *error = NULL;
    errno = 0;
    const double value = strtod(str, &error);
    if (errno != 0 ||  // overflow/underflow happened
                error == str) {  // no valid parse
                                     return deflt;
                                   } else {
      return value;
    }
  }

  bool ParseLeadingBoolValue(const char *str, bool deflt) {
    static const int kMaxLen = 5;
    char value[kMaxLen + 1];
    while (ascii_isspace(*str)) {
      ++str;
    }
    int len = 0;
    for (; len <= kMaxLen && ascii_isalnum(*str); ++str)
      value[len++] = ascii_tolower(*str);
    if (len == 0 || len > kMaxLen)
      return deflt;
    value[len] = '\0';
    switch (len) {
      case 1:
        if (value[0] == '0' || value[0] == 'n')
          return false;
        if (value[0] == '1' || value[0] == 'y')
          return true;
        break;
      case 2:
        if (!strcmp(value, "no"))
          return false;
        break;
      case 3:
        if (!strcmp(value, "yes"))
          return true;
        break;
      case 4:
        if (!strcmp(value, "true"))
          return true;
        break;
      case 5:
        if (!strcmp(value, "false"))
          return false;
        break;
    }
    return deflt;
}

namespace {

  static const int8_t kAsciiToInt[256] = {
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  // 16 36s.
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    36, 36, 36, 36, 36, 36, 36,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 36, 36, 36, 36, 36,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36 };
  
inline bool safe_parse_sign_and_base(StringPiece* text  /*inout*/,
                                     int* base_ptr  /*inout*/,
                                     bool* negative_ptr  /*output*/) {
    const char* start = text->data();
    const char* end = start + text->size();
    int base = *base_ptr;
  
    while (start < end && ascii_isspace(start[0])) {
      ++start;
    }
    while (start < end && ascii_isspace(end[-1])) {
      --end;
    }
    if (start >= end) {
      return false;
    }
  
    *negative_ptr = (start[0] == '-');
    if (*negative_ptr || start[0] == '+') {
      ++start;
      if (start >= end) {
        return false;
      }
    }
    if (base == 0) {
      if (end - start >= 2 && start[0] == '0' &&
                    (start[1] == 'x' || start[1] == 'X')) {
        base = 16;
        start += 2;
        if (start >= end) {
          return false;
        } 
      } else if (end - start >= 1 && start[0] == '0') {
        base = 8;
        start += 1;
      } else {
        base = 10;
      } 
    } else if (base == 16) {
      if (end - start >= 2 && start[0] == '0' &&
                    (start[1] == 'x' || start[1] == 'X')) {
        start += 2; 
        if (start >= end) {
          return false;
        } 
      } 
    } else if (base >= 2 && base <= 36) {
    } else {
      return false;
    } 
    text->set(start, end - start);
    *base_ptr = base;
    return true;
} 


template<typename IntType>
inline bool safe_parse_positive_int(
    StringPiece text, int base, IntType* value_p) {
  IntType value = 0;
  const IntType vmax = std::numeric_limits<IntType>::max();
  assert(vmax > 0);
  //assert(static_cast<int>(vmax) >= base);
  const IntType vmax_over_base = vmax / base;
  const char* start = text.data();
  const char* end = start + text.size();
  for (; start < end; ++start) {
    unsigned char c = static_cast<unsigned char>(start[0]);
      int digit = kAsciiToInt[c];
      if (digit >= base) {
        *value_p = value;
        return false;
      }
      if (value > vmax_over_base) {
        *value_p = vmax;
        return false;
      }
      value *= base;
      if (value > vmax - digit) {
        *value_p = vmax;
        return false;
      }
      value += digit;
    }
    *value_p = value;
    return true;
  }
  

template<typename IntType>
inline bool safe_parse_negative_int(
      StringPiece text, int base, IntType* value_p) {
  IntType value = 0;
  const IntType vmin = std::numeric_limits<IntType>::min();
  assert(vmin < 0);
  assert(vmin <= 0 - base);
  IntType vmin_over_base = vmin / base;
  if (vmin % base > 0) {
    vmin_over_base += 1;
  } 
  const char* start = text.data();
  const char* end = start + text.size();
  for (; start < end; ++start) {
    unsigned char c = static_cast<unsigned char>(start[0]);
    int digit = kAsciiToInt[c];
    if (digit >= base) {
      *value_p = value;
      return false;
    } 
    if (value < vmin_over_base) {
      *value_p = vmin;
      return false;
    } 
    value *= base;
    if (value < vmin + digit) {
      *value_p = vmin; 
      return false;
    } 
    value -= digit;
  } 
  *value_p = value;
  return true;
} 

template<typename IntType>
bool safe_int_internal(StringPiece text, IntType* value_p, int base) {
  *value_p = 0;
  bool negative;
  if (!safe_parse_sign_and_base(&text, &base, &negative)) {
    return false;
  } 
  if (!negative) {
    return safe_parse_positive_int(text, base, value_p);
  } else { 
    return safe_parse_negative_int(text, base, value_p);
  } 
} 
  
template<typename IntType>
inline bool safe_uint_internal(StringPiece text, IntType* value_p, int base) {
  *value_p = 0;
  bool negative;
  if (!safe_parse_sign_and_base(&text, &base, &negative) || negative) {
    return false;
  }
  return safe_parse_positive_int(text, base, value_p);
}
  

} // namespace

bool safe_strto32_base(StringPiece text, int32_t* value, int base) {
  return safe_int_internal<int32_t>(text, value, base);
}
  
bool safe_strto64_base(StringPiece text, int64_t* value, int base) {
  return safe_int_internal<int64_t>(text, value, base);
}

bool safe_strtou32_base(StringPiece text, uint32_t* value, int base) {
  return safe_uint_internal<uint32_t>(text, value, base);
}
  
bool safe_strtou64_base(StringPiece text, uint64_t* value, int base) {
  return safe_uint_internal<uint64_t>(text, value, base);
}
  
bool safe_strtosize_t_base(StringPiece text, size_t* value, int base) {
  return safe_uint_internal<size_t>(text, value, base);
}

bool safe_strtof(const char* str, float* value) {
  char* endptr;
  *value = strtof(str, &endptr);
  if (endptr != str) {
    while (ascii_isspace(*endptr)) ++endptr;
  } 
  return *str != '\0' && *endptr == '\0';
} 
  
bool safe_strtod(const char* str, double* value) {
  char* endptr;
  *value = strtod(str, &endptr);
  if (endptr != str) { 
    while (ascii_isspace(*endptr)) ++endptr;
  } 
  return *str != '\0' && *endptr == '\0';
} 
  
bool safe_strtof(const string& str, float* value) {
  return safe_strtof(str.c_str(), value);
} 
  
bool safe_strtod(const string& str, double* value) {
  return safe_strtod(str.c_str(), value);
} 

} // namespace base
