#ifndef SPARROW_FILES_FILE_H_
#define SPARROW_FILES_FILE_H_

#include <stdint.h>
#include <string>
#include "base/macros.h"
#include "base/file_path.h"
#include "base/scoped_file.h"
#include "base/time.h"
#include <sys/stat.h>

namespace base {

typedef int PlatformFile;
typedef struct stat64 stat_wrapper_t;

class File {
 public:
  enum Flags {
    FLAG_OPEN	= 1 << 0,
    FLAG_CREATE	= 1 << 1,

    FLAG_OPEN_ALWAYS = 1 << 2,
    FLAG_CREATE_ALWAYS = 1 << 3,
    FLAG_OPEN_TRUNCATED = 1 << 4,

    FLAG_READ = 1 << 5,
    FLAG_WRITE = 1 << 6,
    FLAG_APPEND = 1 << 7,
    FLAG_EXCLUSIVE_READ = 1 << 8,
    FLAG_EXCLUSIVE_WRITE= 1 << 9,
    FLAG_ASYNC = 1 << 10,
    FLAG_TEMPORARY = 1 << 11,
    FLAG_HIDDEN = 1 << 12,
    FLAG_DELETE_ON_CLOSE = 1 << 13,
    FLAG_WRITE_ATTRIBUTES = 1 << 14,
    FLAG_SHARE_DELETE = 1 << 15,
    FLAG_TERMINAL_DEVICE = 1 << 16,
    FLAG_BACKUP_SEMANTICS = 1 << 17,
    FLAG_EXECUTE = 1 << 18,
    FLAG_SEQUENTIAL_SCAN = 1 << 19,
  };

  enum Error {
    FILE_OK	= 0,
    FILE_ERROR_FAILED	= -1,
    FILE_ERROR_IN_USE	= -2,
    FILE_ERROR_EXISTS	= -3,
    FILE_ERROR_NOT_FOUND= -4,
    FILE_ERROR_ACCESS_DENIED = -5,
    FILE_ERROR_TOO_MANY_OPENED = -6,
    FILE_ERROR_NO_MEMORY = -7,
    FILE_ERROR_NO_SPACE = -8,
    FILE_ERROR_NOT_A_DIRECTORY = -9,
    FILE_ERROR_INVALID_OPERATION = -10,
    FILE_ERROR_SECURITY = -11,
    FILE_ERROR_ABORT = -12,
    FILE_ERROR_NOT_A_FILE = -13,
    FILE_ERROR_NOT_EMPTY = -14,
    FILE_ERROR_INVALID_URL = -15,
    FILE_ERROR_IO = -16,
    FILE_ERROR_MAX = -17
  };

  enum Whence {
    FROM_BEGIN   = 0,
    FROM_CURRENT = 1,
    FROM_END     = 2
  };

  struct Info {
    Info();
    ~Info();
    void FromStat(const stat_wrapper_t& stat_info);
    int64_t size;
    bool is_directory;
    bool is_symbolic_link;
    Time last_modified;
    Time last_accessed;
    Time creation_time;
  };

  File();
  File(const FilePath& path, uint32_t flags);
  explicit File(PlatformFile file);
  explicit File(Error error_details);
  File(File&& other);
  ~File();

  static File CreateForAsyncHandle(PlatformFile file);
  File& operator=(File&& other);
  void Initialize(const FilePath& path, uint32_t flags);
  bool IsValid() const;
  bool created() const { return created_; }
  Error error_details() const { return error_details_; }
  
  PlatformFile GetPlatformFile() const;
  PlatformFile TakePlatformFile();

  void Close();
  int64_t Seek(Whence whence, int64_t offset);
  int Read(int64_t offset, char* data, int size);
  int ReadAtCurrentPos(char* data, int size);
  int ReadNoBestEffort(int64_t offset, char* data, int size);
  int ReadAtCurrentPosNoBestEffort(char* data, int size);

  int Write(int64_t offset, const char* data, int size);
  int WriteAtCurrentPos(const char* data, int size);
  int WriteAtCurrentPosNoBestEffort(const char* data, int size);

  int64_t GetLength();
  bool SetLength(int64_t length);
  bool Flush();
  bool SetTimes(Time last_access_time, Time last_modified_time);
  bool GetInfo(Info* info);

  Error Lock();
  Error Unlock();

  File Duplicate();
  bool async() const { return async_; }
  static Error OSErrorToFileError(int saved_errno);
  static std::string ErrorToString(Error error);

 private:
  void DoInitialize(const FilePath& path, uint32_t flags);
  bool DoFlush();
  void SetPlatformFile(PlatformFile file);
  ScopedFD file_;

  Error error_details_;
  bool created_;
  bool async_;
};


} // namespace base
#endif // SPARROW_FILES_FILE_H_
