#ifndef BASE_PICKLE_H_
#define BASE_PICKLE_H_
#include <stddef.h>
#include <stdint.h>
#include <string>

#include "base/ref_counted.h"
#include "base/string_piece.h"
#include "base/file.h"

#include <glog/logging.h>

namespace base {

class Pickle;

class PickleIterator {
 public:
  PickleIterator() : payload_(nullptr), read_index_(0), end_index_(0) {}
  explicit PickleIterator(const Pickle& pickle);
  
  bool ReadBool(bool* result);
  bool ReadInt16(int16_t* result);
  bool ReadInt(int* result) { return ReadInt32(result); }
  bool ReadInt32(int32_t* result);
  bool ReadLong(long* result) { return ReadInt64(result); }
  bool ReadInt64(int64_t* result);
  bool ReadUInt16(uint16_t* result);
  bool ReadUInt32(uint32_t* result);
  bool ReadUInt64(uint64_t* result);
 
  bool ReadFloat(float* result);
  bool ReadDouble(double* result);
  bool ReadString(std::string* result);
  bool ReadStringPiece(base::StringPiece* result);
  bool ReadData(const char** data, int* len);
  bool ReadBytes(const char** data, int len);

  bool ReadLength(int* result) {
    return ReadInt32(result) && *result >= 0;
  }
  
  bool SkipBytes(int num_bytes) {
    return !!GetReadPointerAndAdvance(num_bytes);
  }

 private:
  template<typename T>
  bool ReadBuiltinType(T* result);
  
  void Advance(size_t size);

  template<typename T>
  const char* GetReadPointerAndAdvance();

  const char* GetReadPointerAndAdvance(int num_bytes);
  const char* GetReadPointerAndAdvance(int num_bytes,
                                       size_t size_element);
  const char* payload_;
  size_t read_index_;
  size_t end_index_;
};

class PickleSizer {
 public:
  PickleSizer();
  ~PickleSizer();

  size_t payload_size() const { return payload_size_; }

  void AddBool()  { return AddInt32(); }
  void AddInt16() { return AddPOD<int16_t>(); }
  void AddInt32() { return AddPOD<int32_t>(); }
  void AddInt64() { return AddPOD<int64_t>(); }
  void AddUInt16(){ return AddPOD<uint16_t>(); }
  void AddUInt32(){ return AddPOD<uint32_t>(); }
  void AddUInt64(){ return AddPOD<uint64_t>(); }
  void AddFloat() { return AddPOD<float>(); }
  void AddDouble(){ return AddPOD<double>(); }
  void AddString(const StringPiece&);
  void AddData(int len);
  void AddBytes(int len);

 private:
  template<size_t length> void AddBytesStatic();
  template<typename T>
  void AddPOD() { AddBytesStatic<sizeof(T)>(); }
  size_t payload_size_ = 0;
};

class Pickle {
 public:

  struct Header {
    uint32_t payload_size;
  };

  Pickle();
  explicit Pickle(int header_size);
  Pickle(const char* data, int data_len);
  Pickle(const Pickle& other);
  virtual ~Pickle();
  Pickle& operator=(const Pickle& other);
  
  size_t size() const {
    return header_size_ + header_->payload_size;
  }
  const void* data() const {
    return header_;
  }

  size_t GetTotalAllocatedSize() const;

  bool WriteBool(bool value) {
    return WriteInt32(value ? 1 : 0);
  }
  bool WriteInt16(int16_t value) {
    return WritePOD(value);
  }

  bool WriteInt(int value) {
    return WriteInt32(value);
  }

  bool WriteInt32(int32_t value) {
    return WritePOD(value);
  }

  bool WriteLong(long value) {
    return WriteInt64(value);
  }

  bool WriteInt64(int64_t value) {
    return WritePOD(value);
  }
  bool WriteUInt16(uint16_t value) { return WritePOD(value); }
  bool WriteUInt32(uint32_t value) { return WritePOD(value); }
  bool WriteUInt64(uint64_t value) { return WritePOD(value); }
  bool WriteFloat(float value) { return WritePOD(value); }
  bool WriteDouble(double value) { return WritePOD(value); }
  
  bool WriteString(const StringPiece& value);
  bool WriteData(const char* data, int len);
  bool WriteBytes(const void* data, int len);
  void Reserve(size_t len);

  template<class T>
  T* headerT() {
    DCHECK_EQ(header_size_, sizeof(T));
    return static_cast<T *>(header_);
  }

  template<class T>
  const T* headerT() const {
    DCHECK_EQ(header_size_, sizeof(T));
    return static_cast<const T *>(header_);
  }

  size_t payload_size() const {
    return header_ ? header_->payload_size : 0;
  }

  const char* payload() const {
    return reinterpret_cast<const char*>(header_) + header_size_;
  }
  
  const char* end_of_payload() const {
    return header_ ? payload() + payload_size() : nullptr;
  }

 protected:
  char* mutable_payload() {
    return reinterpret_cast<char *>(header_) + header_size_;
  }
  size_t capacity_after_header() const {
    return capacity_after_header_;
  }

  void Resize(size_t new_capacity);

  void* ClaimBytes(size_t num_bytes);

  static const char* FindNext(size_t header_size,
                              const char* range_start,
                              const char* range_end);
  static bool PeekNext(size_t header_size,
                       const char* range_start,
                       const char* range_end,
                       size_t* pickle_size);

  static const int kPayloadUnit;

 private:
  friend class PickleIterator;

  Header* header_;
  size_t header_size_;
  size_t capacity_after_header_;
  size_t write_offset_;
  
  template<size_t length> void WriteBytesStatic(const void* data);

  template<typename T> bool WritePOD(const T& data) {
    WriteBytesStatic<sizeof(data)>(&data);
    return true;
  }

  inline void* ClaimUninitializedBytesInternal(size_t num_bytes);
  inline void WriteBytesCommon(const void* data, size_t length);

};

} // namespace base
#endif // BASE_PICKLE_H_
