#ifndef THREADING_EXCEPTION_H_
#define THREADING_EXCEPTION_H_
#include "base/exception.h"
#include <exception>

namespace threading {

class NoSuchTaskException : public base::TLibraryException {};
  
class UncancellableTaskException : public base::TLibraryException {};
  
class InvalidArgumentException : public base::TLibraryException {};
  
class IllegalStateException : public base::TLibraryException {
 public:
  IllegalStateException() {}
  IllegalStateException(const std::string& message) : TLibraryException(message) {}
};
  
class TimedOutException : public base::TLibraryException {
 public:
  TimedOutException():TLibraryException("TimedOutException"){};
  TimedOutException(const std::string& message ) :
  TLibraryException(message) {}
};
  
class TooManyPendingTasksException : public base::TLibraryException {
 public:
  TooManyPendingTasksException():TLibraryException("TooManyPendingTasksException"){};
  TooManyPendingTasksException(const std::string& message ) :
    TLibraryException(message) {}
};
  
class SystemResourceException : public base::TLibraryException {
 public:
  SystemResourceException() {}
  
  SystemResourceException(const std::string& message) :
     TLibraryException(message) {}
};

} // namespace threading
#endif // THREADING_EXCEPTION_H_
