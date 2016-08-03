#include "base/file.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/time.h>

#include <glog/logging.h>

#include "base/eintr_wrapper.h"

namespace base {

File::Info::Info()
  : size(0),
    is_directory(false),
    is_symbolic_link(false) {
}

File::Info::~Info() {
}

File::File()
  : error_details_(FILE_ERROR_FAILED),
    created_(false),
    async_(false) {
}

File::File(const FilePath& path, uint32_t flags)
  : error_details_(FILE_OK),
    created_(false),
    async_(false) {
  Initialize(path, flags);
}

File::File(PlatformFile file)
  : file_(file),
    error_details_(FILE_OK),
    created_(false),
    async_(false) {
  DCHECK_GE(file, -1);
}

File::File(Error error_details)
  : error_details_(error_details),
    created_(false),
    async_(false) {
}

File::File(File&& other)
  : file_(other.TakePlatformFile()),
    error_details_(other.error_details()),
    created_(other.created()),
    async_(other.async()) {
}

File::~File() {
  Close();
}

// static
File File::CreateForAsyncHandle(PlatformFile platform_file) {
  File file(platform_file);
  file.async_ = true;
  return file;
}

File& File::operator=(File&& other) {
  DCHECK_NE(this, &other);
  Close();
  SetPlatformFile(other.TakePlatformFile());
  error_details_ = other.error_details();
  created_ = other.created();
  async_ = other.async();
  return *this;
}

void File::Initialize(const FilePath& path, uint32_t flags) {
  if (path.ReferencesParent()) {
    error_details_ = FILE_ERROR_ACCESS_DENIED;
    return;
  }
  DoInitialize(path, flags);
}

std::string File::ErrorToString(Error error) {
  switch (error) {
    case FILE_OK: return "FILE_OK";
    case FILE_ERROR_FAILED: return "FILE_ERROR_FAILED";
    case FILE_ERROR_IN_USE: return "FILE_ERROR_IN_USE";
    case FILE_ERROR_EXISTS: return "FILE_ERROR_EXISTS";
    case FILE_ERROR_NOT_FOUND: return "FILE_ERROR_NOT_FOUND";
    case FILE_ERROR_ACCESS_DENIED: return "FILE_ERROR_ACCESS_DENIED";
    case FILE_ERROR_TOO_MANY_OPENED: return "FILE_ERROR_TOO_MANY_OPENED";
    case FILE_ERROR_NO_MEMORY: return "FILE_ERROR_NO_MEMORY";
    case FILE_ERROR_NO_SPACE: return "FILE_ERROR_NO_SPACE";
    case FILE_ERROR_NOT_A_DIRECTORY: return "FILE_ERROR_NOT_A_DIRECTORY";
    case FILE_ERROR_INVALID_OPERATION: return "FILE_ERROR_INVALID_OPERATION";
    case FILE_ERROR_SECURITY: return "FILE_ERROR_SECURITY";
    case FILE_ERROR_ABORT: return "FILE_ERROR_ABORT";
    case FILE_ERROR_NOT_A_FILE: return "FILE_ERROR_NOT_A_FILE";
    case FILE_ERROR_NOT_EMPTY: return "FILE_ERROR_NOT_EMPTY";
    case FILE_ERROR_INVALID_URL: return "FILE_ERROR_INVALID_URL";
    case FILE_ERROR_IO: return "FILE_ERROR_IO";
    case FILE_ERROR_MAX: break;
  }
  return "";
}

bool File::Flush() {
  bool ret = DoFlush();
  return ret;
}

namespace {

int CallFstat(int fd, stat_wrapper_t* sb) {
  return fstat64(fd, sb);
}

bool IsOpenAppend(PlatformFile file) {
  return (fcntl(file, F_GETFL) & O_APPEND) != 0;
}

int CallFtruncate(PlatformFile file, int64_t length) {
  return HANDLE_EINTR(ftruncate(file, length));
}

int CallFutimes(PlatformFile file, const struct timeval times[2]) {
  return futimes(file, times);
}

File::Error CallFcntlFlock(PlatformFile file, bool do_lock) {
  struct flock lock;
  lock.l_type = do_lock ? F_WRLCK : F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  if (HANDLE_EINTR(fcntl(file, F_SETLK, &lock)) == -1)
    return File::OSErrorToFileError(errno);
  return File::FILE_OK;
}

} // namespace

void File::Info::FromStat(const stat_wrapper_t& stat_info) {
  is_directory = S_ISDIR(stat_info.st_mode);
  is_symbolic_link = S_ISLNK(stat_info.st_mode);
  size = stat_info.st_size;
  
  time_t last_modified_sec = stat_info.st_mtim.tv_sec;
  int64_t last_modified_nsec = stat_info.st_mtim.tv_nsec;
  time_t last_accessed_sec = stat_info.st_atim.tv_sec;
  int64_t last_accessed_nsec = stat_info.st_atim.tv_nsec;
  time_t creation_time_sec = stat_info.st_ctim.tv_sec;
  int64_t creation_time_nsec = stat_info.st_ctim.tv_nsec;

  last_modified = Time::FromTimeT(last_modified_sec) +
	TimeDelta::FromMicroseconds(last_modified_nsec /
				    Time::kNanosecondsPerMicrosecond);

  last_accessed = Time::FromTimeT(last_accessed_sec) +
        TimeDelta::FromMicroseconds(last_accessed_nsec /
                              Time::kNanosecondsPerMicrosecond);
    
  creation_time = Time::FromTimeT(creation_time_sec) +
	TimeDelta::FromMicroseconds(creation_time_nsec /
                 Time::kNanosecondsPerMicrosecond);
}


bool File::IsValid() const {
  return file_.is_valid();
}

PlatformFile File::GetPlatformFile() const {
  return file_.get();
}

PlatformFile File::TakePlatformFile() {
  return file_.release();
}

void File::Close() {
  if (!IsValid())
    return;
  file_.reset();
}

int64_t File::Seek(Whence whence, int64_t offset) {
  return lseek(file_.get(), static_cast<off_t>(offset),
		            static_cast<int>(whence));
}

int File::Read(int64_t offset, char* data, int size) {
  DCHECK(IsValid());
  if (size < 0)
    return -1;

  int bytes_read = 0;
  int rv;
  do {
    rv = HANDLE_EINTR(pread(file_.get(), data + bytes_read,
			    size - bytes_read, offset + bytes_read));
    if (rv <= 0)
      break;
    bytes_read += rv;
  } while (bytes_read < size);
  return bytes_read ? bytes_read : rv;
}

int File::ReadAtCurrentPos(char* data, int size) {
  DCHECK(IsValid());
  if (size < 0)
    return -1;

  int bytes_read = 0;
  int rv;
  do {
    rv = HANDLE_EINTR(read(file_.get(), data + bytes_read,
			    size - bytes_read));
    if (rv <= 0)  
      break;
    bytes_read += rv;
  } while (bytes_read < size);

  return bytes_read ? bytes_read : rv;
}

int File::ReadNoBestEffort(int64_t offset, char* data, int size) {
  DCHECK(IsValid());
  return HANDLE_EINTR(pread(file_.get(), data, size, offset));
}

int File::ReadAtCurrentPosNoBestEffort(char* data, int size) {
  DCHECK(IsValid());
  if (size < 0)
    return -1;
  return HANDLE_EINTR(read(file_.get(), data, size));
}

int File::Write(int64_t offset, const char* data, int size) {
  if (IsOpenAppend(file_.get())) {
    return WriteAtCurrentPos(data, size);
  }
  DCHECK(IsValid());
  if (size < 0)
    return -1;

  int bytes_written = 0;
  int rv;
  do {
    rv = HANDLE_EINTR(pwrite(file_.get(), data + bytes_written,
			     size - bytes_written, 
			     offset + bytes_written)) ;
    if (rv <= 0)
      break;
    bytes_written += rv;
  } while (bytes_written < size);
  return bytes_written ? bytes_written : rv;
}

int File::WriteAtCurrentPos(const char* data, int size) {
  DCHECK(IsValid());
  if (size < 0)
    return -1;
  int bytes_written = 0;
  int rv;
  do {
    rv = HANDLE_EINTR(write(file_.get(), data + bytes_written,
			    size - bytes_written));
    if (rv <= 0) 
      break;

    bytes_written += rv;
  } while (bytes_written < size);

  return bytes_written ? bytes_written : rv;
}

int File::WriteAtCurrentPosNoBestEffort(const char* data, int size) {
  DCHECK(IsValid());
  if (size < 0)
    return -1;
  return HANDLE_EINTR(write(file_.get(), data, size));
}

int64_t File::GetLength() {
  DCHECK(IsValid());
  stat_wrapper_t file_info;
  if (CallFstat(file_.get(), &file_info))
    return false;
  return file_info.st_size;
}

bool File::SetLength(int64_t length) {
  DCHECK(IsValid());
  return !CallFtruncate(file_.get(), length);
}

bool File::SetTimes(Time last_access_time, Time last_modified_time) {
  DCHECK(IsValid());

  timeval times[2];
  times[0] = last_access_time.ToTimeval();
  times[1] = last_modified_time.ToTimeval();

  return !CallFutimes(file_.get(), times);
}

bool File::GetInfo(Info* info) {
  DCHECK(IsValid());

  stat_wrapper_t file_info;
  if (CallFstat(file_.get(), &file_info))
    return false;
  info->FromStat(file_info);
  return true;
}


File::Error File::Lock() {
  return CallFcntlFlock(file_.get(), true);
}

File::Error File::Unlock() {
  return CallFcntlFlock(file_.get(), false);
}

File File::Duplicate() {
  if (!IsValid()) {
    return File();
  }

  PlatformFile fd = dup(GetPlatformFile());
  if (fd == -1) {
    return File(OSErrorToFileError(errno));
  }
  File other(fd);
  if (async()) {
    other.async_ = true;
  }
  return other;
}


// static
File::Error File::OSErrorToFileError(int saved_errno) {
  switch (saved_errno) {
    case EACCES:
    case EISDIR:
    case EROFS:
    case EPERM:
      return FILE_ERROR_ACCESS_DENIED;
    case EBUSY:
    case ETXTBSY:
      return FILE_ERROR_IN_USE;
    case EEXIST:
      return FILE_ERROR_EXISTS;
    case EIO:
      return FILE_ERROR_IO;
    case ENOENT:
      return FILE_ERROR_NOT_FOUND;
    case EMFILE:
      return FILE_ERROR_TOO_MANY_OPENED;
    case ENOMEM:
      return FILE_ERROR_NO_MEMORY;
    case ENOSPC:
      return FILE_ERROR_NO_SPACE;
    case ENOTDIR:
      return FILE_ERROR_NOT_A_DIRECTORY;
    default:
      return FILE_ERROR_FAILED;
  }
}

void File::DoInitialize(const FilePath& path, uint32_t flags) {
  DCHECK(!IsValid());

  int open_flags = 0;
  if (flags & FLAG_CREATE)
    open_flags = O_CREAT | O_EXCL;

  created_ = false;

  if (flags & FLAG_CREATE_ALWAYS) {
    DCHECK(!open_flags);
    DCHECK(flags & FLAG_WRITE);
    open_flags = O_CREAT | O_TRUNC;
  }

  if (flags & FLAG_OPEN_TRUNCATED) {
    DCHECK(!open_flags);
    DCHECK(flags & FLAG_WRITE);
    open_flags = O_CREAT | O_TRUNC;
  }

  if (!open_flags && !(flags & FLAG_OPEN) 
		  && !(flags & FLAG_OPEN_ALWAYS)) {
    errno = EOPNOTSUPP;
    error_details_ = FILE_ERROR_FAILED;
    return;
  }

  if (flags & FLAG_WRITE && flags & FLAG_READ) {
    open_flags |= O_RDWR;
  } else if (flags & FLAG_WRITE) {
    open_flags |= O_WRONLY;
  } else if (!(flags & FLAG_READ) && 
             !(flags & FLAG_WRITE_ATTRIBUTES) &&
	     !(flags & FLAG_APPEND) &&
	     !(flags & FLAG_OPEN_ALWAYS)) {
    ; // never come here???
  }

  if (flags & FLAG_TERMINAL_DEVICE) {
    open_flags |= O_NOCTTY | O_NDELAY;
  }

  if (flags & FLAG_APPEND && flags & FLAG_READ) {
    open_flags |= O_APPEND | O_RDWR;
  } else if (flags & FLAG_APPEND) {
    open_flags |= O_APPEND | O_WRONLY;
  }

  int mode = S_IRUSR | S_IWUSR;
  int descriptor = HANDLE_EINTR(open(path.value().c_str(),
			             open_flags, mode));
  if (flags & FLAG_OPEN_ALWAYS) {
    if (descriptor < 0) {
      open_flags |= O_CREAT;
      if (flags & FLAG_EXCLUSIVE_READ || flags & FLAG_EXCLUSIVE_WRITE)
        open_flags |= O_EXCL;

      descriptor = HANDLE_EINTR(open(path.value().c_str(),
			             open_flags,
				     mode));
      if (descriptor >= 0)
        created_ = true;
    }
  }

  if (descriptor < 0) {
    error_details_ = File::OSErrorToFileError(errno);
    return;
  }

  if (flags & (FLAG_CREATE_ALWAYS | FLAG_CREATE)) {
    created_ = true;
  }

  if (flags & FLAG_DELETE_ON_CLOSE) {
    unlink(path.value().c_str());
  }

  async_ = ((flags & FLAG_ASYNC) == FLAG_ASYNC);
  error_details_ = FILE_OK;
  file_.reset(descriptor);
}

bool File::DoFlush() {
  DCHECK(IsValid());

  return !HANDLE_EINTR(fdatasync(file_.get()));
}

void File::SetPlatformFile(PlatformFile file) {
  DCHECK(!file_.is_valid());
  file_.reset(file);
}

/////////////

} // namespace base
