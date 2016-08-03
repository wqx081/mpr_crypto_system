#include "zip/zip.h"
#include <string>
#include <vector>

#include "base/file.h"
#include "base/file_util.h"
#include "base/file_enumerator.h"
#include "base/string_util.h"

#include "zip/zip_internal.h"
#include "zip/zip_reader.h"

#include <minizip/unzip.h>
#include <minizip/zip.h>

#include <glog/logging.h>

namespace {

bool AddFileToZip(zipFile zip_file, const base::FilePath& src_dir) {
  base::File file(src_dir,
                  base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!file.IsValid()) {
    LOG(ERROR) << "Could not open file for path " << src_dir.value();
    return false;
  }

  int num_bytes;
  char buf[zip::internal::kZipBufSize];
  do {
    num_bytes = file.ReadAtCurrentPos(buf, zip::internal::kZipBufSize);
    if (num_bytes > 0) {
      if (ZIP_OK != zipWriteInFileInZip(zip_file, buf, num_bytes)) {
        LOG(INFO) << "Could not write data to zip for path" << src_dir.value();
        return false;
      }
    }
  } while (num_bytes > 0);
  return true;
}

bool AddEntryToZip(zipFile zip_file, const base::FilePath& path,
                   const base::FilePath& root_path) {

  base::FilePath relative_path;
  bool result = root_path.AppendRelativePath(path, &relative_path);
  DCHECK(result);
  std::string str_path = relative_path.value();
  bool is_directory = base::DirectoryExists(path);

  if (is_directory) {
    str_path += "/";
  }

  zip_fileinfo file_info = zip::internal::GetFileInfoForZipping(path);
  if (!zip::internal::ZipOpenNewFileInZip(zip_file, str_path, &file_info)) {
    return false;
  }

  bool success = true;
  if (!is_directory) {
    success = AddFileToZip(zip_file, path);
  }

  if (ZIP_OK != zipCloseFileInZip(zip_file)) {
    return false;
  }

  return success;
}

bool ExcludeNoFilesFilter(const base::FilePath& file_path) {
  (void) file_path;
  return true;
}

bool ExcludeHiddenFilesFilter(const base::FilePath& file_path) {
  return file_path.BaseName().value()[0] != '.';
}

} // namespace


namespace zip {

bool Unzip(const base::FilePath& src_file, const base::FilePath& dst_dir) {
  ZipReader reader;
  if (!reader.Open(src_file)) {
    DLOG(WARNING) << "Failed to open " << src_file.value();
    return false;
  }
  while (reader.HasMore()) {
    if (!reader.OpenCurrentEntryInZip()) {
      DLOG(WARNING) << "Failed to open the current file in zip";
      return false;
    }
    if (reader.current_entry_info()->is_unsafe()) {
      DLOG(WARNING) << "Found an unsafe file in zip "
                    << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.ExtractCurrentEntryIntoDirectory(dst_dir)) {
      DLOG(WARNING) << "Failed to extract "
                    << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.AdvanceToNextEntry()) {
      DLOG(WARNING) << "Failed to advance to the next file";
      return false;
    }
  }
  return true;
}

bool ZipWithFilterCallback(const base::FilePath& src_dir,
                           const base::FilePath& dst_file,
                           const FilterCallback& filter_cb) {
  DCHECK(base::DirectoryExists(src_dir));

  zipFile zip_file = internal::OpenForZipping(dst_file.value(),
                                              APPEND_STATUS_CREATE);
  if (!zip_file) {
    DLOG(WARNING) << "could not create file " << dst_file.value();
    return false;
  }
  
  bool success = true;
  base::FileEnumerator file_enumerator(src_dir,
                                       true,
                                       base::FileEnumerator::FILES |
                                       base::FileEnumerator::DIRECTORIES);
  for (base::FilePath path = file_enumerator.Next();
       !path.value().empty();
       path = file_enumerator.Next()) {
    if (!filter_cb(path)) {
      continue;
    }
    if (!AddEntryToZip(zip_file, path, src_dir)) {
      success = false;
      break;
    }
  }

  if (ZIP_OK != zipClose(zip_file, nullptr)) {
    DLOG(ERROR) << "Error closing zip file " << dst_file.value();
    return false;
  }
  return success;
}

bool Zip(const base::FilePath& src_dir, const base::FilePath& dest_file,
                  bool include_hidden_files) {
  if (include_hidden_files) {
    return ZipWithFilterCallback(
        src_dir, dest_file, ExcludeNoFilesFilter);
  } else {
    return ZipWithFilterCallback(
        src_dir, dest_file, ExcludeHiddenFilesFilter);
  }
}

bool ZipFiles(const base::FilePath& src_dir,
              const std::vector<base::FilePath>& src_relative_paths,
              int dest_fd) {
  DCHECK(base::DirectoryExists(src_dir));
  zipFile zip_file = internal::OpenFdForZipping(dest_fd, APPEND_STATUS_CREATE);

  if (!zip_file) {
    DLOG(ERROR) << "couldn't create file for fd " << dest_fd;
    return false;
  }

  bool success = true;
  for (std::vector<base::FilePath>::const_iterator iter =
       src_relative_paths.begin();
       iter != src_relative_paths.end(); ++iter) {
    const base::FilePath& path = src_dir.Append(*iter);
    if (!AddEntryToZip(zip_file, path, src_dir)) {
      success = false;
      break;
    }
  }

  if (ZIP_OK != zipClose(zip_file, NULL)) {
    DLOG(ERROR) << "Error closing zip file for fd " << dest_fd;
    success = false;
  }

  return success;
}

} // namespace zip
