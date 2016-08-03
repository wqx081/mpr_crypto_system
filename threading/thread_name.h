#pragma once

#include "base/string_piece.h"
#include <pthread.h>

namespace threading {


inline bool SetThreadName(pthread_t id, 
		          const base::StringPiece& name) {
  return 0 == pthread_setname_np(id, name.substr(0, 15).data());
}

inline bool SetThreadName(const base::StringPiece& name) {
  return SetThreadName(pthread_self(), name);
}

}
