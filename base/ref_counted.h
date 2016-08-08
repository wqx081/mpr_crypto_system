#ifndef BASE_REF_COUNTED_H_
#define BASE_REF_COUNTED_H_

#include <stddef.h>
#include <cassert>
#include <iosfwd>
#include <type_traits>

#include "base/atomic_ref_count.h"
#include "base/macros.h"

#include <glog/logging.h>

namespace base {

class RefCountedBase {
 public:
  bool HasOneRef() const {
    return ref_count_ == 1;
  }
 protected:
  RefCountedBase()
    : ref_count_(0) {}
  ~RefCountedBase() {}
  void AddRef() const {
    ++ref_count_;
  }
  bool Release() const {
    if (--ref_count_ == 0) {
      return true;
    }
    return false;
  }

 private:
  mutable int ref_count_;

  DISALLOW_COPY_AND_ASSIGN(RefCountedBase);
};

class RefCountedThreadSafeBase {
 public:
  bool HasOneRef() const;

 protected:
  RefCountedThreadSafeBase();
  ~RefCountedThreadSafeBase();
  void AddRef() const;
  bool Release() const;

 private:
  mutable AtomicRefCount ref_count_;

  DISALLOW_COPY_AND_ASSIGN(RefCountedThreadSafeBase);
};

template<typename T>
class RefCounted : public RefCountedBase {
 public:
  RefCounted() {}

  void AddRef() const {
    RefCountedBase::AddRef();
  }

  void Release() const {
    if (RefCountedBase::Release()) {
      delete static_cast<const T*>(this);
    }
  }

 protected:
  ~RefCounted() {}
 private:
  DISALLOW_COPY_AND_ASSIGN(RefCounted<T>);
};

template<typename T, typename Traits>
class RefCountedThreadSafe;

template<typename T>
struct DefaultRefCountedThreadSafeTraits {
  static void Destruct(const T* x) {
    RefCountedThreadSafe<T,
                         DefaultRefCountedThreadSafeTraits>::DeleteInternal(x);
  }
};

// A thread-safe variant of RefCounted<T>
//
// class MyFoo : public base::RefCountedThreadSafe<MyFoo> {
//  ...
// };
//
// If you're using the default trait, then you shuld add compile time
// asserts that no one else is deleting your object. i.e.
//   private:
//    friend class base::RefCountedThreadSafe<MyFoo>;
//    ~MyFoo();
template<typename T, typename Traits = DefaultRefCountedThreadSafeTraits<T>>
class RefCountedThreadSafe : public RefCountedThreadSafeBase {
 public:
  RefCountedThreadSafe() {}

  void AddRef() const {
    RefCountedThreadSafeBase::AddRef(); 
  }
  void Release() const {
    if (RefCountedThreadSafeBase::Release()) {
      Traits::Destruct(static_cast<const T*>(this));
    }
  }
 protected:
  ~RefCountedThreadSafe() {}

 private:
  friend struct DefaultRefCountedThreadSafeTraits<T>;
  static void DeleteInternal(const T* x) { delete x; }
  
  DISALLOW_COPY_AND_ASSIGN(RefCountedThreadSafe);
};

template <typename T>
class RefCountedData
    : public RefCountedThreadSafe<RefCountedData<T>> {
 public:
  RefCountedData() : data() {}
  RefCountedData(const T& in_value) : data(in_value) {}

  T data;

 private:
  friend class RefCountedThreadSafe<RefCountedData<T>>;
  ~RefCountedData() {}
};

} // namespace base

//////////
//
// A smart pointer class for reference counted objects.
// Use this class instead of calling AddRef and Release manually on a 
// reference counted object to avoid common leaks caused by forgetting
// to Release an object reference.
//
// Sample Usage:
//
// class MyFoo : public RefCounted<MyFoo> {
//  ...
// };
//
// void some_func() {
//  scoped_ptr<MyFoo> foo = new MyFoo();
//  foo->Method(param);
//  // |foo| is released when this function returns
// }
//
template<typename T>
class scoped_ref_ptr {
 public:
  typedef T element_type;

  scoped_ref_ptr() : ptr_(nullptr) {}

  scoped_ref_ptr(T* p) : ptr_(p) {
    if (ptr_) {
      AddRef(ptr_);
    }
  }
  
  scoped_ref_ptr(const scoped_ref_ptr<T>& r) : ptr_(r.ptr_) {
    if (ptr_) {
      AddRef(ptr_);
    }
  }

  template<typename U,
           typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
  scoped_ref_ptr(const scoped_ref_ptr<U>& r) : ptr_(r.get()) {
    if (ptr_) {
      AddRef(ptr_);
    }
  }

  scoped_ref_ptr(scoped_ref_ptr&& r) : ptr_(r.get()) {
    r.ptr_ = nullptr;
  }

  template<typename U,
           typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
  scoped_ref_ptr(scoped_ref_ptr<U>&& r) : ptr_(r.get()) {
    r.ptr_ = nullptr;
  }

  ~scoped_ref_ptr() {
    if (ptr_) {
      Release(ptr_);
    }
  }

  T* get() const {
    return ptr_;
  }

  T& operator*() const {
    assert(ptr_ != nullptr);
    return *ptr_;
  }

  T* operator->() const {
    assert(ptr_ != nullptr);
    return ptr_;
  }
  
  scoped_ref_ptr<T>& operator=(T* p) {
    if (p) {
      AddRef(p);
    }
    T* old_ptr = ptr_;
    ptr_ = p;
    if (old_ptr) {
      Release(old_ptr);
    }
    return *this;
  }

  scoped_ref_ptr<T>& operator=(const scoped_ref_ptr<T>& r) {
    return *this = r.ptr_;
  }

  template <typename U>
  scoped_ref_ptr<T>& operator=(const scoped_ref_ptr<U>& r) {
    return *this = r.get();
  }

  scoped_ref_ptr<T>& operator=(scoped_ref_ptr<T>&& r) {
    scoped_ref_ptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  template <typename U>
  scoped_ref_ptr<T>& operator=(scoped_ref_ptr<U>&& r) {
    scoped_ref_ptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  void swap(T** pp) {
    T* p = ptr_;
    ptr_ = *pp;
    *pp = p;
  }

  void swap(scoped_ref_ptr<T>& r) {
    swap(&r.ptr_);
  }

  explicit operator bool() const { return ptr_ != nullptr; }

  template <typename U>
  bool operator==(const scoped_ref_ptr<U>& rhs) const {
    return ptr_ == rhs.get();
  }

  template <typename U>
  bool operator!=(const scoped_ref_ptr<U>& rhs) const {
    return !operator==(rhs);
  }

  template <typename U>
  bool operator<(const scoped_ref_ptr<U>& rhs) const {
    return ptr_ < rhs.get();
  }

 protected:
  T* ptr_;
 private:
  template<typename U>
  friend class scoped_ref_ptr;

  static void AddRef(T* ptr);
  static void Release(T* ptr);
};

template <typename T>
void scoped_ref_ptr<T>::AddRef(T* ptr) {
  ptr->AddRef();
}

template <typename T>
void scoped_ref_ptr<T>::Release(T* ptr) {
  ptr->Release();
}

template <typename T>
scoped_ref_ptr<T> make_scoped_ref_ptr(T* t) {
  return scoped_ref_ptr<T>(t);
}

template <typename T, typename U>
bool operator==(const scoped_ref_ptr<T>& lhs, const U* rhs) {
  return lhs.get() == rhs;
}

template <typename T, typename U>
bool operator==(const T* lhs, const scoped_ref_ptr<U>& rhs) {
  return lhs == rhs.get();
}

template <typename T>
bool operator==(const scoped_ref_ptr<T>& lhs, std::nullptr_t null) {
  (void) null;
  return !static_cast<bool>(lhs);
}

template <typename T>
bool operator==(std::nullptr_t null, const scoped_ref_ptr<T>& rhs) {
  (void) null;
  return !static_cast<bool>(rhs);
}

template <typename T, typename U>
bool operator!=(const scoped_ref_ptr<T>& lhs, const U* rhs) {
  return !operator==(lhs, rhs);
}

template <typename T, typename U>
bool operator!=(const T* lhs, const scoped_ref_ptr<U>& rhs) {
  return !operator==(lhs, rhs);
}

template <typename T>
bool operator!=(const scoped_ref_ptr<T>& lhs, std::nullptr_t null) {
  return !operator==(lhs, null);
}

template <typename T>
bool operator!=(std::nullptr_t null, const scoped_ref_ptr<T>& rhs) {
  return !operator==(null, rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const scoped_ref_ptr<T>& p) {
  return out << p.get();
}

#endif // BASE_REF_COUNTED_H_
