#ifndef BASE_EXCEPTION_H_
#define BASE_EXCEPTION_H_
#include <exception>
#include <string>

namespace base {

class TException : public std::exception {
 public:
  TException() {}
  TException(TException&&) noexcept {}
  TException(const TException&) {}
  TException& operator=(const TException&) {
    return *this;
  }
  TException& operator=(TException&&) { return *this; }
};

template<class Self>
struct TExceptionType : TException {
  const Self& self() const {
    return static_cast<const Self&>(*this);
  }
  
  Self& self() {
    return static_cast<Self&>(*this);
  }
};


class TLibraryException : public TException {
 public:
  TLibraryException() {}
  
  explicit TLibraryException(const std::string& message) :
    message_(message) {}
  
  TLibraryException(const char* message, int errnoValue);
  
  ~TLibraryException() throw() override {}
  
  const char* what() const throw() override {
    if (message_.empty()) {
      return "Default TLibraryException.";
    } else {
      return message_.c_str();
    }
  }
  
 protected:
  std::string message_;
};


} // namespace base

#endif // BASE_EXCEPTION_H_
