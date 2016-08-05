#ifndef DB_COMMON_SHARED_OBJECT_H_
#define DB_COMMON_SHARED_OBJECT_H_
#include "base/macros.h"
#include "db/common/simple_ref_counted.h"

#include <string>

namespace db {

class SharedObject : public SimpleRefCounted {
 public:
  ~SharedObject();
  static SimpleRefPtr<SharedObject> Open(const std::string& name);
  void* SafeSym(const std::string& name);
  void* Sym(const std::string& name);

  template<typename T>
  bool Resolve(const std::string& s, T*& v) {
    void* p = Sym(s);
    if (!p) {
      return false;
    }
    v = (T*)(p);
    return true;
  }

  template<typename T>
  void SafeResolve(const std::string& s, T*& v) {
    v = (T*)(Sym(s));
  }

 private:
  std::string dlname_;
  void* handle_;

  SharedObject() : handle_(nullptr) {}
  SharedObject(std::string name, void* handler);

  DISALLOW_COPY_AND_ASSIGN(SharedObject);
};

} // namespace db
#endif // DB_COMMON_SHARED_OBJECT_H_
