#ifndef SPARROW_FILES_FILE_ENUMERATOR_H_
#define SPARROW_FILES_FILE_ENUMERATOR_H_

#include <stddef.h>
#include <stdint.h>
#include <stack>
#include <vector>

#include "base/macros.h"
#include "base/file_path.h"
#include "base/time.h"
#include <sys/stat.h>
#include <unistd.h>

namespace base {

class FileEnumerator {
 public:
  class FileInfo {
   public:
    FileInfo();
    ~FileInfo();

    bool IsDirectory() const;
    FilePath GetName() const;
    int64_t GetSize() const;
    Time GetLastModifiedTime() const;
    const struct stat& stat() const { return stat_; }
   private:
    friend class FileEnumerator;

    struct stat stat_;
    FilePath filename_; 
  };

  enum FileType {
    FILES	= 1 << 0,
    DIRECTORIES = 1 << 1,
    INCLUDE_DOT_DOT = 1 << 2,
    SHOW_SYM_LINKS = 1 << 4,
  };

  FileEnumerator(const FilePath& root_path,
                 bool recursive,
		 int file_type);
  FileEnumerator(const FilePath& root_path,
                 bool recursive,
		 int file_type,
		 const FilePath::StringType& pattern);
  ~FileEnumerator();

  FilePath Next();
  FileInfo GetInfo() const;

 private:
  bool ShouldSkip(const FilePath& path);
  static bool ReadDirectory(std::vector<FileInfo>* entries,
		            const FilePath& source,
			    bool show_links);
  std::vector<FileInfo> directory_entries_;
  size_t current_directory_entry_;
  FilePath root_path_;
  bool recursive_;
  int file_type_;
  FilePath::StringType pattern_;
  std::stack<FilePath> pending_paths_;
  
  DISALLOW_COPY_AND_ASSIGN(FileEnumerator);
};


} // namespace base
#endif // SPARROW_FILES_FILE_ENUMERATOR_H_
