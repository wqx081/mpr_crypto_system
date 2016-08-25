#ifndef BASE_STATUS_H_
#define BASE_STATUS_H_

#include <functional>
#include <iosfwd>
#include <string>
#include "base/string_piece.h"

#include <glog/logging.h>

namespace base {

enum Code {
  OK = 0,

  CANCELLED = 1,
  UNKNOWN = 2,
  INVALID_ARGUMENT = 3,
  DEADLINE_EXCEEDED = 4,
  NOT_FOUND = 5,
  ALREADY_EXISTS = 6,
  PERMISSION_DENIED = 7,
  UNAUTHENTICATED = 16,
  RESOURCE_EXHAUSTED = 8,
  FAILED_PRECONDITION = 9,
  ABORTED = 10,
  OUT_OF_RANGE = 11,
  UNIMPLEMENTED = 12,
  INTERNAL = 13,
  UNAVAILABLE = 14,
  DATA_LOSS = 15,
  DO_NOT_USE_RESERVED_FOR_FUTURE_EXPANSION_USE_DEFAULT_IN_SWITCH_INSTEAD_ = 20,
};

class Status {
 public:
  Status() : state_(nullptr) {}
  ~Status() { delete state_; }
  Status(Code code, StringPiece msg);

  Status(const Status& other);
  void operator=(const Status& other);

  static Status OK() { return Status(); }

  bool ok() const { return state_ == nullptr; }

  Code code() const { return ok() ? Code::OK : state_->code; }

  const std::string error_message() const {
    return ok() ? empty_string() : state_->msg;
  }

  bool operator==(const Status& x) const;
  bool operator!=(const Status& x) const;

  void Update(const Status& new_status);

  std::string ToString() const;

 private:
  static const std::string& empty_string();
  struct State {
    Code code;
    std::string msg;
  };
  State* state_;
  void SlowCopyFrom(const State* src);
};

inline Status::Status(const Status& s)
  : state_((s.state_ == nullptr) ? nullptr : new State(*s.state_)) {}
  
inline void Status::operator=(const Status& s) {
  if (state_ != s.state_) {
    SlowCopyFrom(s.state_);
  }
}
  
inline bool Status::operator==(const Status& x) const {
  return (this->state_ == x.state_) || (ToString() == x.ToString());
}
  
inline bool Status::operator!=(const Status& x) const { return !(*this == x); }
  
std::ostream& operator<<(std::ostream& os, const Status& x);
  
typedef std::function<void(const Status&)> StatusCallback;
  
#define STATUS_CHECK_OK(val) CHECK_EQ(::base::Status::OK(), (val))
#define STATUS_QCHECK_OK(val) QCHECK_EQ(::base::Status::OK(), (val))

} // namesapce base

#define RETURN_IF_ERROR(expr)                        \
  do {                                               \
    const base::Status _status = (expr);             \
    if (PREDICT_FALSE(!_status.ok())) return _status; \
  } while (0)     


#endif // BASE_STATUS_H_
