#include "base/file_util.h"

#include <stdio.h>
#include <fstream>
#include <limits>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


#include "base/file_path.h"
#include "base/file_enumerator.h"
#include "base/eintr_wrapper.h"

#include <glog/logging.h>

namespace base {

static int CallStat(const char *path, stat_wrapper_t *sb) {
  return stat64(path, sb);
}

static int CallLstat(const char *path, stat_wrapper_t *sb) {
  return lstat64(path, sb);
}


int64_t ComputeDirectorySize(const FilePath& root_path) {
  int64_t running_size = 0;
  FileEnumerator file_iter(root_path, true, FileEnumerator::FILES);
  while (!file_iter.Next().empty()) {
    running_size += file_iter.GetInfo().GetSize();
  }
  return running_size;
}

bool ContentsEqual(const FilePath& filename1, const FilePath& filename2) {
  std::ifstream file1(filename1.value().c_str(),
                      std::ios::in | std::ios::binary);
  std::ifstream file2(filename2.value().c_str(),
                      std::ios::in | std::ios::binary);
  if (!file1.is_open() || !file2.is_open()) {
    return false;
  }
  
  const int BUFFER_SIZE = 2056;
  char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
  do {
    file1.read(buffer1, BUFFER_SIZE);
    file2.read(buffer2, BUFFER_SIZE);

    if ((file1.eof() != file2.eof()) ||
        (file1.gcount() != file2.gcount()) ||
        (memcmp(buffer1, buffer2, static_cast<size_t>(file1.gcount())))) {
      file1.close();
      file2.close();
      return false;
    }
  } while (!file1.eof() || !file2.eof());
  
  file1.close();
  file2.close();
  return true;
}

bool TextContentsEqual(const FilePath& filename1, const FilePath& filename2) {
  std::ifstream file1(filename1.value().c_str(), std::ios::in);
  std::ifstream file2(filename2.value().c_str(), std::ios::in);

  if (!file1.is_open() || !file2.is_open()) {
    return false;
  }

  do {
    std::string line1, line2;
    getline(file1, line1);
    getline(file2, line2);

    if ((file1.eof() != file2.eof()) ||
        file1.bad() || file2.bad()) {
      return false;
    }

    std::string::size_type end1 = line1.find_last_not_of("\r\n");
    if (end1 == std::string::npos) {
      line1.clear();
    } else if (end1 + 1 < line1.length()) {
      line1.erase(end1 + 1);
    }

    std::string::size_type end2 = line1.find_last_not_of("\r\n");
    if (end2 == std::string::npos) {
      line2.clear();
    } else if (end2 + 1 < line2.length()) {
      line2.erase(end2 + 1);
    }

    if (line1 != line2) {
      return false;
    }

  } while (!file1.eof() || !file2.eof());

  return true;
}

bool ReadFileToStringWithMaxSize(const FilePath& path,
                                 std::string* contents,
                                 size_t max_size) {
  if (contents) {
    contents->clear();
  }
  if (path.ReferencesParent()) {
    return false;
  }
  FILE* file = OpenFile(path, "rb");
  if (!file) {
    return false;
  }

  const size_t kBufferSize = 1 << 16;
  std::unique_ptr<char[]> buf(new char[kBufferSize]);
  size_t len;
  size_t size = 0;
  bool read_status = true;

  while ((len = fread(buf.get(), 1, kBufferSize, file)) > 0) {
    if (contents) {
      contents->append(buf.get(), std::min(len, max_size - size));
    }
    if ((max_size - size) < len) {
      read_status = false;
      break;
    }

    size += len;
  }

  read_status = read_status && !ferror(file);
  CloseFile(file);

  return read_status;
}

bool ReadFileToString(const FilePath& path,
                      std::string* contents) {
  return ReadFileToStringWithMaxSize(path,
                                     contents,
                                     std::numeric_limits<size_t>::max());
}

bool DirectoryExists(const FilePath& path) {
  stat_wrapper_t file_info;
  if (CallStat(path.value().c_str(), &file_info) == 0)
    return S_ISDIR(file_info.st_mode);
  return false;
}

std::string TempFileName() {
  return std::string(".org.ilikereader.com.XXXXXX");
}

int CreateAndOpenFdForTemporaryFile(FilePath directory, FilePath* path) {
  *path = directory.Append(base::TempFileName());
  const std::string& tmpdir_string = path->value();
  char* buffer = const_cast<char*>(tmpdir_string.c_str());

  return HANDLE_EINTR(mkstemp(buffer));
}

FilePath MakeAbsoluteFilePath(const FilePath& input) {
  char full_path[PATH_MAX];
  if (realpath(input.value().c_str(), full_path) == nullptr) {
    return FilePath();
  }
  return FilePath(full_path);
}

bool DeleteFile(const FilePath& path, bool recursive) {
  const char* path_str = path.value().c_str();
  stat_wrapper_t file_info;
  int test = CallLstat(path_str, &file_info);
  if (test != 0) {
    bool ret = (errno == ENOENT || errno == ENOTDIR);
    return ret;
  }
  if (!S_ISDIR(file_info.st_mode))
    return (unlink(path_str) == 0);
  if (!recursive)
    return (rmdir(path_str) == 0);

  bool success = true;
  std::stack<std::string> directories;
  directories.push(path.value());
  FileEnumerator traversal(path, true,
                           FileEnumerator::FILES | FileEnumerator::DIRECTORIES |
                           FileEnumerator::SHOW_SYM_LINKS);
  for (FilePath current = traversal.Next(); success && !current.empty();
              current = traversal.Next()) {
    if (traversal.GetInfo().IsDirectory())
      directories.push(current.value());
    else
      success = (unlink(current.value().c_str()) == 0);
  }

  while (success && !directories.empty()) {
    FilePath dir = FilePath(directories.top());
    directories.pop();
    success = (rmdir(dir.value().c_str()) == 0);
  }
  return success;
}

bool CreateTemporaryFileInDir(const FilePath& dir, FilePath* temp_file) {
  int fd = CreateAndOpenFdForTemporaryFile(dir, temp_file);
  return ((fd >= 0) && !IGNORE_EINTR(close(fd)));
}

bool GetTempDir(FilePath* path) {
  const char* tmp = getenv("TMPDIR");
  if (tmp) {
    *path = FilePath(tmp);
  } else {
    *path = FilePath("/tmp");
  }
  return true;
}

static bool CreateTemporaryDirInDirImpl(const FilePath& base_dir,
                                        const FilePath::StringType& name_tmpl,
                                        FilePath* new_dir) {
  DCHECK(name_tmpl.find("XXXXXX") != FilePath::StringType::npos)
      << "Directory name template must contain \"XXXXXX\".";

  FilePath sub_dir = base_dir.Append(name_tmpl);
  std::string sub_dir_string = sub_dir.value();

  char* buffer = const_cast<char*>(sub_dir_string.c_str());
  char* dtemp = mkdtemp(buffer);
  if (!dtemp) {
    DLOG(ERROR) << "mkdtemp";
    return false;
  }
  *new_dir = FilePath(dtemp);
  return true;
}

bool CreateTemporaryDirInDir(const FilePath& base_dir,
                             const FilePath::StringType& prefix,
                             FilePath* new_dir) {
  FilePath::StringType mkdtemp_template = prefix;
  mkdtemp_template.append(FILE_PATH_LITERAL("XXXXXX"));
  return CreateTemporaryDirInDirImpl(base_dir, mkdtemp_template, new_dir);
}

bool CreateNewTempDirectory(const FilePath::StringType& prefix,
                            FilePath* new_temp_path) {
  (void) prefix;
  FilePath tmpdir;
  if (!GetTempDir(&tmpdir))
    return false;

  return CreateTemporaryDirInDirImpl(tmpdir, TempFileName(), new_temp_path);
}



bool CreateDirectoryAndGetError(const FilePath& full_path,
		                File::Error* error) {
  std::vector<FilePath> subpaths;

  FilePath last_path = full_path;
  subpaths.push_back(full_path);
  for (FilePath path = full_path.DirName();
       path.value() != last_path.value();
       path = path.DirName()) {
    subpaths.push_back(path);
    last_path = path;
  }

  for (std::vector<FilePath>::reverse_iterator i = subpaths.rbegin();
       i != subpaths.rend(); ++i) {
    if (DirectoryExists(*i))
      continue;
    if (mkdir(i->value().c_str(), 0700) == 0)
      continue;

    int saved_errno = errno;
    if (!DirectoryExists(*i)) {
      if (error)
        *error = File::OSErrorToFileError(saved_errno);
      return false;
    }
  }
  return true;
}

bool CreateDirectory(const FilePath& full_path) {
  return CreateDirectoryAndGetError(full_path, NULL);
}

bool GetFileInfo(const FilePath& file_path, File::Info* results) {
  stat_wrapper_t file_info;
  if (CallStat(file_path.value().c_str(), &file_info) != 0)
    return false;
		     
  results->FromStat(file_info);
  return true;
} 

bool GetFileSize(const FilePath& file_path, int64_t* file_size) {
  File::Info info;
  if (!GetFileInfo(file_path, &info)) {
    return false;
  }
  *file_size = info.size;
  return true;
}

bool TouchFile(const FilePath& path,
               const Time& last_accessed,
               const Time& last_modified) {

  int flags = File::FLAG_OPEN | File::FLAG_WRITE_ATTRIBUTES;
  File file(path, flags);
  if (!file.IsValid()) {
    return false;
  }
  return file.SetTimes(last_accessed, last_modified);
}

bool PathExists(const FilePath& path) {
  return ::access(path.value().c_str(), F_OK) == 0;
}

bool TruncateFile(FILE* file) {
  if (file == nullptr) {
    return false;
  }
  long current_offset = ::ftell(file);
  if (current_offset == -1) {
    return false;
  }
  int fd = ::fileno(file);
  if (::ftruncate(fd, current_offset) != 0) {
    return false;
  }
  return true;
}

///// POSIX
FILE* OpenFile(const FilePath& filename, const char* mode) {
  FILE* result = nullptr;
  do {
    result = ::fopen(filename.value().c_str(), mode);
  } while (!result && errno == EINTR);
  return result;
}

bool CloseFile(FILE* file) {
  if (file == nullptr) {
    return true;
  }
  return ::fclose(file) == 0;
}

FILE* FileToFILE(File file, const char* mode) {
  FILE* stream = fdopen(file.GetPlatformFile(), mode);
  if (stream) {
    file.TakePlatformFile();
  }
  return stream;
}

int ReadFile(const FilePath& filename, char* data, int max_size) {
  int fd = HANDLE_EINTR(open(filename.value().c_str(), O_RDONLY));
  if (fd < 0) {
    return -1;
  }

  ssize_t bytes_read = HANDLE_EINTR(read(fd, data, max_size));
  if (IGNORE_EINTR(close(fd)) < 0) {
    return -1;
  }
  return bytes_read;       
}

bool WriteFileDecriptor(const int fd, const char* data, int size) {
  ssize_t bytes_written_total = 0;
  for (ssize_t bytes_written_partial = 0;
       bytes_written_total < size;
       bytes_written_total += bytes_written_partial) {
    bytes_written_partial = HANDLE_EINTR(write(fd,
                                               data + bytes_written_total,
                                               size - bytes_written_total));
    if (bytes_written_partial < 0) {
      return false;
    }
  }
  return true;
}

int WriteFile(const FilePath& filename, const char* data, int size) {
  int fd = HANDLE_EINTR(::creat(filename.value().c_str(), 0666));
  if (fd < 0) {
    return -1;
  }
  
  int bytes_written = WriteFileDecriptor(fd, data, size) ? size : -1;
  if (IGNORE_EINTR(close(fd)) < 0) {
    return -1;
  }
  return bytes_written;
}

bool AppendToFile(const FilePath& filename, const char* data, int size) {
  bool ret = true;
  int fd = HANDLE_EINTR(::open(filename.value().c_str(), O_WRONLY | O_APPEND));
  if (fd < 0) {
    return false;
  }

  if (!WriteFileDecriptor(fd, data, size)) {
    ret = false;
  }

  if (IGNORE_EINTR(close(fd)) < 0) {
    ret = false;
  }

  return ret;
}

bool CopyFile(const FilePath& from_path,
              const FilePath& to_path) {

  File infile = File(from_path, File::FLAG_OPEN | File::FLAG_READ);
  if (!infile.IsValid()) {
    return false;
  }
  
  File outfile(to_path, File::FLAG_WRITE | File::FLAG_CREATE_ALWAYS);
  if (!outfile.IsValid()) {
    return false;
  }

  const size_t kBufferSize = 32768;
  std::vector<char> buffer(kBufferSize);
  bool result = true;

  while (result) {
    ssize_t bytes_read = infile.ReadAtCurrentPos(&buffer[0],
                                                 buffer.size());
    if (bytes_read < 0) {
      result = false;
      break;
    }
    if (bytes_read == 0) {
      break;
    }
    ssize_t bytes_written_per_read = 0;
    do {
      ssize_t bytes_written_partial = outfile.WriteAtCurrentPos(
        &buffer[bytes_written_per_read], 
        bytes_read - bytes_written_per_read);
      if (bytes_written_partial < 0) {
        result = false;
        break;
      }
      bytes_written_per_read += bytes_written_partial;
    } while (bytes_written_per_read < bytes_read);
  }

  return result;
}

//bool CopyDirectory(const FilePath& from_path, const FilePath& to_path,
//                   bool recursive);
//
} // namespace base
