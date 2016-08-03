#include "base/string_util.h"

namespace base {

template<typename Str>
inline static bool DoLowerCaseEqualsASCII(BasicStringPiece<Str> str,
                                               StringPiece lowercase_ascii) {
  if (str.size() != lowercase_ascii.size())
    return false;
  for (size_t i = 0; i < str.size(); i++) {
    if (ToLowerASCII(str[i]) != lowercase_ascii[i])
      return false;
  }
  return true;
}

bool LowerCaseEqualsASCII(StringPiece str, StringPiece lowercase_ascii) {
  return DoLowerCaseEqualsASCII<std::string>(str, lowercase_ascii);
}


template<class StringType>
int CompareCaseInsensitiveASCIIT(BasicStringPiece<StringType> a,
			         BasicStringPiece<StringType> b) {
  size_t i = 0;
  while (i < a.length() && i < b.length()) {
    typename StringType::value_type lower_a = ToLowerASCII(a[i]);
    typename StringType::value_type lower_b = ToLowerASCII(b[i]);
    if (lower_a < lower_b)
      return -1;
    if (lower_a > lower_b)
      return 1; 
    i++;
  } 
			        
  if (a.length() == b.length())
    return 0;
				      
  if (a.length() < b.length())
    return -1;
  return 1;
} 
    
int CompareCaseInsensitiveASCII(StringPiece a, StringPiece b) {
  return CompareCaseInsensitiveASCIIT<std::string>(a, b);
}

bool StartsWith(StringPiece str,
		StringPiece search_for,
		CompareCase case_sensitivity) {
  if (search_for.size() > str.size()) {
    return false;
  }
  BasicStringPiece<std::string> source = str.substr(0, search_for.size());

  switch (case_sensitivity) {
    case CompareCase::SENSITIVE: 
      return source == search_for;
    case CompareCase::INSENSITIVE_ASCII:
      return std::equal(search_for.begin(), search_for.end(),
		        source.begin(),
         CaseInsensitiveCompareASCII<typename std::string::value_type>());
					     
  default:
    ; 
    return false;
  }
}

bool EndsWith(StringPiece str,
	      StringPiece search_for,
	      CompareCase case_sensitivity) {

  if (search_for.size() > str.size())
    return false;
     
  BasicStringPiece<std::string> source = str.substr(str.size() - 
		  search_for.size(),
		  search_for.size());
         
  switch (case_sensitivity) {
    case CompareCase::SENSITIVE:
      return source == search_for;

    case CompareCase::INSENSITIVE_ASCII:
      return std::equal(
         source.begin(), source.end(),
	search_for.begin(),
CaseInsensitiveCompareASCII<typename std::string::value_type>());
						   
	default:
	; //NOTREACHED();
  return false;
}

}

char* WriteInto(std::string* str, size_t length_with_null) {
  DCHECK_GT(length_with_null, 1u);
  str->reserve(length_with_null);
  str->resize(length_with_null - 1);
  return &((*str)[0]);
}

} // namespace base
