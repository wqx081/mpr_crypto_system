#ifndef DB_SIMPLE_REF_COUNTED_H_
#define DB_SIMPLE_REF_COUNTED_H_
#include "base/atomic_ref_count.h"
#include <exception>
#include <stdexcept>

class SimpleRefCounted {
 public:
  bool HasOneRef() const {
    return base::AtomicRefCountIsOne(
      &const_cast<SimpleRefCounted*>(this)->ref_count_);
  }

// protected: 
  SimpleRefCounted() : ref_count_(0) {}
  virtual ~SimpleRefCounted() {}
  
  void AddRef() const {
    base::AtomicRefCountInc(&ref_count_);
  }
 
  // Return true if the object should self-delete
  bool Release() const {
    if (!base::AtomicRefCountDec(&ref_count_)) {
      return true;
    }
    return false;
  }

  static void Dispose(SimpleRefCounted* ptr) {
    delete ptr;
  }

 private:
  mutable base::AtomicRefCount ref_count_;
};

template<typename T>
class SimpleRefPtr {
 public:
  SimpleRefPtr(T* ptr=nullptr) {
    Reset(ptr);
  }

  ~SimpleRefPtr() {
    Reset();
  }

  SimpleRefPtr(const SimpleRefPtr& other) {
    Reset(other.ptr_);
  }

  const SimpleRefPtr& operator=(const SimpleRefPtr& other) {
    if (this != &other) {
      Reset(other.ptr_);
    }
    return *this;
  }

  T* get() const {
    return ptr_;
  }

  operator bool() const {
    return ptr_ != nullptr;
  }

  T* operator->() const {
    if (ptr_ == nullptr) {
      throw std::runtime_error("attempt to access an empty object");
    }
    return ptr_;
  }
  
  T& operator*() const {
    if (ptr_ == nullptr) {
      throw std::runtime_error("attempt to access an empty object");
    }
    return *ptr_;
  }
  
  void Reset(T* ptr=nullptr) {
    if (ptr == ptr_) {
      return;
    }    
    if (ptr_ != nullptr) {
      if (ptr_->Release()) {
        T::Dispose(ptr_);
      }
      ptr_ = nullptr;
    }
    if (ptr != nullptr) {
      ptr->AddRef();
    }
    ptr_ = ptr;
  }
 private:
  T* ptr_;
};

#endif // DB_SIMPLE_REF_COUNTED_H_
