#ifndef BASE_FILE_UTIL_H_
#define BASE_FILE_UTIL_H_
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <set>
#include <string>
#include <vector>

#include "base/file.h"
#include "base/file_path.h"
#include "base/time.h"

namespace base {

FilePath MakeAbsoluteFilePath(const FilePath& input);
int64_t ComputeDirectorySize(const FilePath& root_path);
bool DeleteFile(const FilePath& path, bool recursive);
bool Move(const FilePath& from_path, const FilePath& to_path);
bool ReplaceFile(const FilePath& from_path,
                 const FilePath& to_path,
                 File::Error* error);
bool CopyFile(const FilePath& from_path, const FilePath& to_path);
bool CopyDirectory(const FilePath& from_path,
                   const FilePath& to_path,
                   bool recursive);
bool PathExists(const FilePath& path);
bool PathIsWritable(const FilePath& path);
bool DirectoryExists(const FilePath& path);
bool ContentsEqual(const FilePath& filename1,
                   const FilePath& filename2);
bool TextContentsEqual(const FilePath& filename1,
                       const FilePath& filename2);
bool ReadFileToString(const FilePath& path, std::string* contents);
bool ReadFileToStringWithMaxSize(const FilePath& path,
                                 std::string* contents,
                                 size_t max_size);
bool ReadFromFD(int fd, char* buffer, size_t bytes);
bool CreateSymbolicLink(const FilePath& target,
                        const FilePath& symlink);
bool ReadSymbolicLink(const FilePath& symlink, FilePath* target);

enum FilePermissionBits {
  FILE_PERMISSION_MASK              = S_IRWXU | S_IRWXG | S_IRWXO,
  FILE_PERMISSION_USER_MASK         = S_IRWXU,
  FILE_PERMISSION_GROUP_MASK        = S_IRWXG,
  FILE_PERMISSION_OTHERS_MASK       = S_IRWXO,

  FILE_PERMISSION_READ_BY_USER      = S_IRUSR,
  FILE_PERMISSION_WRITE_BY_USER     = S_IWUSR,
  FILE_PERMISSION_EXECUTE_BY_USER   = S_IXUSR,
  FILE_PERMISSION_READ_BY_GROUP     = S_IRGRP,
  FILE_PERMISSION_WRITE_BY_GROUP    = S_IWGRP,
  FILE_PERMISSION_EXECUTE_BY_GROUP  = S_IXGRP,
  FILE_PERMISSION_READ_BY_OTHERS    = S_IROTH,
  FILE_PERMISSION_WRITE_BY_OTHERS   = S_IWOTH,
  FILE_PERMISSION_EXECUTE_BY_OTHERS = S_IXOTH,
};

bool GetPosixFilePermissions(const FilePath& path, int* mode);
bool SetPosixFilePermissions(const FilePath& path, int mode);
bool IsDirectoryEmpty(const FilePath& dir_path);
bool GetTempDir(FilePath* path);
FilePath GetHomeDir();
bool CreateTemporaryFile(FilePath* path);
bool CreateTemporaryFileInDir(const FilePath& dir,
                              FilePath* temp_file);
FILE* CreateAndOpenTemporaryFile(FilePath* path);
FILE* CreateAndOpenTemporaryFileInDir(const FilePath& dir,
                                      FilePath* path);
bool CreateNewTempDirectory(const FilePath::StringType& prefix,
                            FilePath* new_temp_path);
bool CreateTemporaryDirInDir(const FilePath& base_dir,
                             const FilePath::StringType& prefix,
                             FilePath* new_dir);
bool CreateDirectoryAndGetError(const FilePath& full_path,
                                File::Error* error);
bool CreateDirectory(const FilePath& full_path);
bool GetFileSize(const FilePath& file_path, int64_t* file_size);
bool NormalizeFilePath(const FilePath& path, FilePath* real_path);
bool IsLink(const FilePath& file_path);
bool GetFileInfo(const FilePath& file_path, File::Info* info);

bool TouchFile(const FilePath& path,
               const Time& last_accessed,
               const Time& last_modified);

FILE* OpenFile(const FilePath& filename, const char* mode);
bool CloseFile(FILE* file);
FILE* FileToFILE(File file, const char* mode);
bool TruncateFile(FILE* file);
int ReadFile(const FilePath& filename, char* data, int max_size);
int WriteFile(const FilePath& filename, const char* data,
              int size);
bool WriteFileDescriptor(const int fd, const char* data, int size);
bool AppendToFile(const FilePath& filename,
                  const char* data,
                  int size);
bool GetCurrentDirectory(FilePath* path);
bool SetCurrentDirectory(const FilePath& path);
int GetUniquePathNumber(const FilePath& path,
                        const FilePath::StringType& suffix);
bool SetNonBlocking(int fd);

bool VerifyPathControlledByUser(const base::FilePath& base,
                                const base::FilePath& path,
                                uid_t owner_uid,
                                const std::set<gid_t>& group_gids);
int GetMaximumPathComponentLength(const base::FilePath& path);

enum FileSystemType {
  FILE_SYSTEM_UNKNOWN,  // statfs failed.
  FILE_SYSTEM_0,        // statfs.f_type == 0 means unknown, may indicate AFS.
  FILE_SYSTEM_ORDINARY,       // on-disk filesystem like ext2
  FILE_SYSTEM_NFS,
  FILE_SYSTEM_SMB,
  FILE_SYSTEM_CODA,
  FILE_SYSTEM_MEMORY,         // in-memory file system
  FILE_SYSTEM_CGROUP,         // cgroup control.
  FILE_SYSTEM_OTHER,          // any other value.
  FILE_SYSTEM_TYPE_COUNT
};

bool GetFileSystemType(const FilePath& path, FileSystemType* type);
bool GetShmemTempDir(bool executable, FilePath* path);

namespace internal {

bool MoveUnsafe(const FilePath& from_path,
                const FilePath& to_path);
bool CopyAndDeleteDirectory(const FilePath& from_path,
                            const FilePath& to_path);

} // namespace internal
} // namespace base
#endif // BASE_FILE_UTIL_H_
