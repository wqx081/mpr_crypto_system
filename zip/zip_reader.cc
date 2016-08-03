#include "zip/zip_reader.h"
#include "zip/zip_internal.h"

#include <utility>
#include <functional>
#include <glog/logging.h>
#include <string>


#include "base/macros.h"
#include "base/file.h"
#include "base/file_util.h"
#include "base/string_util.h"
#include "base/time.h"

#include <minizip/unzip.h>

namespace zip {

namespace {

class FilePathWriterDelegate : public WriterDelegate {
 public:
  explicit FilePathWriterDelegate(const base::FilePath& output_file_path);
  ~FilePathWriterDelegate() override;

  bool PrepareOutput() override;
  bool WriteBytes(const char* data, int num_bytes) override;

 private:
  base::FilePath output_file_path_;
  base::File file_;

  DISALLOW_COPY_AND_ASSIGN(FilePathWriterDelegate);
};

FilePathWriterDelegate::FilePathWriterDelegate(
		    const base::FilePath& output_file_path)
	    : output_file_path_(output_file_path) {
}

FilePathWriterDelegate::~FilePathWriterDelegate() {
}

bool FilePathWriterDelegate::PrepareOutput() {
  if (!base::CreateDirectory(output_file_path_.DirName()))
    return false;

  file_.Initialize(output_file_path_,
		   base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);
  return file_.IsValid();
}

bool FilePathWriterDelegate::WriteBytes(const char* data, int num_bytes) {
  return num_bytes == file_.WriteAtCurrentPos(data, num_bytes);
}

class StringWriterDelegate : public WriterDelegate {
 public:
  StringWriterDelegate(size_t max_read_bytes, std::string* output);
  ~StringWriterDelegate() override;

  bool PrepareOutput() override;

  bool WriteBytes(const char* data, int num_bytes) override;

 private:
  size_t max_read_bytes_;
  std::string* output_;

  DISALLOW_COPY_AND_ASSIGN(StringWriterDelegate);
};

StringWriterDelegate::StringWriterDelegate(size_t max_read_bytes,
                                           std::string* output)
    : max_read_bytes_(max_read_bytes),
      output_(output) {
}

StringWriterDelegate::~StringWriterDelegate() {
}

bool StringWriterDelegate::PrepareOutput() {
  return true;
}

bool StringWriterDelegate::WriteBytes(const char* data, int num_bytes) {
  if (output_->size() + num_bytes > max_read_bytes_)
    return false;
  output_->append(data, num_bytes);
  return true;
}

} // namespace


ZipReader::EntryInfo::EntryInfo(const std::string& file_name,
                                const unz_file_info& raw_file_info)
    : file_path_(file_name),
      is_directory_(false) {
  original_size_ = raw_file_info.uncompressed_size;
  is_directory_ = base::EndsWith(file_name, "/", base::CompareCase::INSENSITIVE_ASCII);

  is_unsafe_ = file_path_.ReferencesParent();
  
  if(file_path_.IsAbsolute() || base::StartsWith(file_name, "/",
                                                 base::CompareCase::INSENSITIVE_ASCII)) {
    is_unsafe_ = true;
  }

  base::Time::Exploded exploded_time = {};  // Zero-clear.
  exploded_time.year = raw_file_info.tmu_date.tm_year;
  exploded_time.month = raw_file_info.tmu_date.tm_mon + 1;
  exploded_time.day_of_month = raw_file_info.tmu_date.tm_mday;
  exploded_time.hour = raw_file_info.tmu_date.tm_hour;
  exploded_time.minute = raw_file_info.tmu_date.tm_min;
  exploded_time.second = raw_file_info.tmu_date.tm_sec;
  exploded_time.millisecond = 0;

  if (!base::Time::FromLocalExploded(exploded_time, &last_modified_))
    last_modified_ = base::Time::UnixEpoch();
}

ZipReader::ZipReader() {
  Reset();
}

ZipReader::~ZipReader() {
  Close();
}

bool ZipReader::Open(const base::FilePath& zip_file_path) {
  DCHECK(!zip_file_);

  zip_file_ = internal::OpenForUnzipping(zip_file_path.value());  
  if (!zip_file_) {
    return false;
  }

  return OpenInternal();
}

bool ZipReader::OpenFromFD(int fd) {
  DCHECK(!zip_file_);

  zip_file_ = internal::OpenFdForUnzipping(fd);
  if (!zip_file_) {
    return false;
  }

  return OpenInternal();
}

bool ZipReader::OpenFromString(const std::string& data) {
  zip_file_ = internal::PrepareMemoryForUnzipping(data);
  if (!zip_file_) {
    return false;
  }
  return OpenInternal();
}

void ZipReader::Close() {
  if (zip_file_) {
    unzClose(zip_file_);
  }
  Reset();
}

bool ZipReader::HasMore() {
  return !reached_end_;
}

bool ZipReader::AdvanceToNextEntry() {
  DCHECK(zip_file_);

  if (reached_end_) {
    return false;
  }

  unz_file_pos position = {};
  if (unzGetFilePos(zip_file_, &position) != UNZ_OK) {
    return false;
  }
  const int current_entry_index = position.num_of_file;
  if (current_entry_index + 1 == num_entries_) {
    reached_end_ = true;
  } else {
    DCHECK_LT(current_entry_index + 1, num_entries_);
    if (unzGoToNextFile(zip_file_) != UNZ_OK) {
      return false;
    }
  }
  current_entry_info_.reset();
  return true;
}

bool ZipReader::OpenCurrentEntryInZip() {
  DCHECK(zip_file_);

  unz_file_info raw_file_info = {};
  char raw_file_name_in_zip[internal::kZipMaxPath] = {};
  const int result = unzGetCurrentFileInfo(zip_file_,
                                           &raw_file_info,
                                           raw_file_name_in_zip,
                                           sizeof(raw_file_name_in_zip) - 1,
                                           nullptr,
                                           0,
                                           nullptr,
                                           0);
  if (result != UNZ_OK) {
    return false;
  }
  if (raw_file_name_in_zip[0] == '\0') {
    return false;
  }
  current_entry_info_.reset(
    new EntryInfo(raw_file_name_in_zip, raw_file_info));
  return true;
}

bool ZipReader::LocateAndOpenEntry(const base::FilePath& path) {
  DCHECK(zip_file_);
  
  current_entry_info_.reset();
  reached_end_ = false;
  const int kDefaultCaseSensivityOfOS = 0;
  const int result = unzLocateFile(zip_file_,
                                   path.value().c_str(),
                                   kDefaultCaseSensivityOfOS);
  if (result != UNZ_OK) {
    return false;
  }

  return OpenCurrentEntryInZip();
}

bool ZipReader::ExtractCurrentEntry(WriterDelegate* delegate) const {
  DCHECK(zip_file_);

  const int open_result = unzOpenCurrentFile(zip_file_);
  if (open_result != UNZ_OK) {
    return false;
  }

  if (!delegate->PrepareOutput()) {
    return false;
  }

  bool success = true;
  std::unique_ptr<char[]> buf(new char[internal::kZipBufSize]);
  while (true) {
    const int num_bytes_read = unzReadCurrentFile(zip_file_,
                                                  buf.get(),
                                                  internal::kZipBufSize);
    if (num_bytes_read == 0) {
      break;
    } else if (num_bytes_read < 0) {
      success = false;
      break;
    } else if (num_bytes_read > 0) {
      if (!delegate->WriteBytes(buf.get(), num_bytes_read)) {
        success = false;
        break;
      }
    }
  }

  unzCloseCurrentFile(zip_file_);
  return success;
}

bool ZipReader::ExtractCurrentEntryToFilePath(
    const base::FilePath& output_file_path) const {
  DCHECK(zip_file_);

  if (current_entry_info()->is_directory())
    return base::CreateDirectory(output_file_path);

  bool success = false;
  {
    FilePathWriterDelegate writer(output_file_path);
    success = ExtractCurrentEntry(&writer);
  }

  if (success &&
      current_entry_info()->last_modified() != base::Time::UnixEpoch()) {
//    base::TouchFile(output_file_path,
//                    base::Time::Now(),
//                    current_entry_info()->last_modified());
  }

  return success;
}

bool ZipReader::ExtractCurrentEntryIntoDirectory(
    const base::FilePath& output_directory_path) const {
  DCHECK(current_entry_info_.get());

  base::FilePath output_file_path = output_directory_path.Append(
      current_entry_info()->file_path());
  return ExtractCurrentEntryToFilePath(output_file_path);
}

bool ZipReader::ExtractCurrentEntryToFile(base::File* file) const {
  DCHECK(zip_file_);

  if (current_entry_info()->is_directory())
    return false;

  FileWriterDelegate writer(file);
  return ExtractCurrentEntry(&writer);
}

bool ZipReader::ExtractCurrentEntryToString(size_t max_read_bytes,
                                            std::string* output) const {
  DCHECK(output);
  DCHECK(zip_file_);
  DCHECK_NE(0U, max_read_bytes);

  if (current_entry_info()->is_directory()) {
    output->clear();
    return true;
  }

  std::string contents;
  contents.reserve(
      static_cast<size_t>(std::min(static_cast<int64_t>(max_read_bytes),
                          current_entry_info()->original_size())));

  StringWriterDelegate writer(max_read_bytes, &contents);
  if (!ExtractCurrentEntry(&writer))
    return false;
  output->swap(contents);
  return true;
}

bool ZipReader::OpenInternal() {
  DCHECK(zip_file_);

  unz_global_info zip_info = {};
  if (unzGetGlobalInfo(zip_file_, &zip_info) != UNZ_OK) {
    return false;
  }
  num_entries_ = zip_info.number_entry;
  if (num_entries_ < 0) {
    return false;
  }

  reached_end_ = (num_entries_ == 0);
  return true;
}

void ZipReader::Reset() {
  zip_file_ = nullptr;
  num_entries_ = 0;
  reached_end_ = false;
  current_entry_info_.reset();
}


// FileWriterDelegate
FileWriterDelegate::FileWriterDelegate(base::File* file)
  : file_(file),
    file_length_(0) {
}

FileWriterDelegate::~FileWriterDelegate() {
  file_->SetLength(file_length_);
}

bool FileWriterDelegate::PrepareOutput() {
  return file_->Seek(base::File::FROM_BEGIN, 0) >= 0;
}

bool FileWriterDelegate::WriteBytes(const char* data, int num_bytes) {
  int bytes_written = file_->WriteAtCurrentPos(data, num_bytes);
  if (bytes_written > 0) {
    file_length_ += bytes_written;
  }
  return bytes_written == num_bytes;
}

} // namespace zip
