#ifndef ZIP_ZIP_READER_H_
#define ZIP_ZIP_READER_H_

#include "base/macros.h"
#include "base/file_path.h"
#include "base/file.h"
#include "base/time.h"

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <minizip/unzip.h>

#include <functional>

namespace zip {

// Usage example:
//
// ZipReader reader;
// reader.Open(zip_file_path);
//
// while (reader.HasMore()) {
//  reader.OpenCurrentEntryInfo();
//  reader.ExtractCurrentEntryIntoDirectory(output_directory_path);
//  reader.AdvanceToNextEntry();
// }

class WriterDelegate {
 public:
  virtual ~WriterDelegate() {}
  virtual bool PrepareOutput() = 0;
  virtual bool WriteBytes(const char* data, int num_bytes) = 0;
};

class ZipReader {
 public:

  class EntryInfo {
   public:
    EntryInfo(const std::string& filename_in_zip,
              const unz_file_info& raw_file_info);

    const base::FilePath& file_path() const { return file_path_; }
    int64_t original_size() const { return original_size_; }
    base::Time last_modified() const { return last_modified_; }
    bool is_directory() const { return is_directory_; }
    bool is_unsafe() const { return is_unsafe_; }

   private:
    const base::FilePath file_path_;
    int64_t original_size_;
    base::Time last_modified_;
    bool is_directory_;
    bool is_unsafe_;
    DISALLOW_COPY_AND_ASSIGN(EntryInfo);
  };
  
  ZipReader();
  ~ZipReader();

  bool Open(const base::FilePath& zip_file_path);
  bool OpenFromFD(int zip_fd);
  bool OpenFromString(const std::string& data);
  void Close();
  bool HasMore();
  bool AdvanceToNextEntry();
  bool OpenCurrentEntryInZip();
  bool LocateAndOpenEntry(const base::FilePath& path_in_zip);
  bool ExtractCurrentEntry(WriterDelegate* delegate) const;
  bool ExtractCurrentEntryToFilePath(const base::FilePath& output_file_path) const;
  bool ExtractCurrentEntryIntoDirectory(const base::FilePath& output_directory_path) const;
  bool ExtractCurrentEntryToFile(base::File* file) const;
  bool ExtractCurrentEntryToString(size_t max_read_bytes,
                                   std::string* output) const;

  EntryInfo* current_entry_info() const {
    return current_entry_info_.get();
  }

  int num_entries() const {
    return num_entries_;
  }

 private:
  bool OpenInternal();
  void Reset();

  unzFile zip_file_;
  int num_entries_;
  bool reached_end_;
  std::unique_ptr<EntryInfo> current_entry_info_;

  DISALLOW_COPY_AND_ASSIGN(ZipReader);
};

class FileWriterDelegate : public WriterDelegate {
 public:
  explicit FileWriterDelegate(base::File* file);
  ~FileWriterDelegate() override;
  bool PrepareOutput() override;
  bool WriteBytes(const char* data, int num_bytes) override;

 private:
  base::File* file_;  
  int64_t file_length_;

  DISALLOW_COPY_AND_ASSIGN(FileWriterDelegate);
};

} // namespace zip
#endif // ZIP_ZIP_READER_H_
