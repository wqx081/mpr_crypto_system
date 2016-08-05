#include "db/common/shared_object.h"
#include "db/common/exception.h"

#include <string>
#include <dlfcn.h>

namespace db {

SharedObject::SharedObject(std::string name, void* handle)
    : dlname_(name),
      handle_(handle) {}

SharedObject::~SharedObject() {
  dlclose(handle_);
}

SimpleRefPtr<SharedObject> SharedObject::Open(const std::string& name) {

  SimpleRefPtr<SharedObject> dl;
  void* handle = dlopen(name.c_str(), RTLD_LAZY);
  if (!handle) {
    return dl;
  }
  try {
    dl.Reset(new SharedObject(name, handle));
    handle = nullptr;
    return dl;
  } catch (...) {
    if (handle) {
      dlclose(handle);
    }
    throw;
  }
}

void* SharedObject::Sym(const std::string& s) {
  return ::dlsym(handle_, s.c_str());
}

void* SharedObject::SafeSym(const std::string& s) {
  void* ptr = Sym(s);
  if (!ptr) {
    throw DBException("db::SharedObject failed to resolve symbol [" + s + "] in " + dlname_);
  }
  return ptr;
}

} // namespace db
