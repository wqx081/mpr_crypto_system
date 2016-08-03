#include "zip/zip_internal.h"

#include "base/time.h"
#include "base/file.h"
#include "base/file_util.h"

#include <stddef.h>
#include <algorithm>
#include <stdlib.h>

#include <glog/logging.h>

#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <minizip/ioapi.h>


namespace {

void* FdOpenFileFunc(void* opaque, const char* filename, int mode) {
  FILE* file = nullptr;
  const char* mode_fopen = nullptr;

  if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
    mode_fopen = "rb";
  else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
    mode_fopen = "r+b";
  else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
    mode_fopen = "wb";

  if ((filename != nullptr) && (mode_fopen != nullptr)) {
    int fd = dup(*static_cast<int*>(opaque));
    if (fd != -1) {
      file = fdopen(fd, mode_fopen);
    }
  }
  return file;
}

int FdCloseFileFunc(void* opaque, void* stream) {
  fclose(static_cast<FILE*>(stream));
  free(opaque);
  return 0;
}

void FillFdOpenFileFunc(zlib_filefunc_def* pzlib_filefunc_def, int fd) {
  fill_fopen_filefunc(pzlib_filefunc_def);
  pzlib_filefunc_def->zopen_file = FdOpenFileFunc;
  pzlib_filefunc_def->zclose_file = FdCloseFileFunc;
  int* ptr_fd = static_cast<int*>(malloc(sizeof(fd)));
  *ptr_fd = fd;
  pzlib_filefunc_def->opaque = ptr_fd;
}

struct ZipBuffer {
  const char* data;
  size_t length;
  size_t offset;
};

void* OpenZipBuffer(void* opaque, const char* /* filename */, int mode) {
  if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) 
      != ZLIB_FILEFUNC_MODE_READ) {
    //not reached
    return nullptr;
  }
  ZipBuffer* buffer = static_cast<ZipBuffer*>(opaque);
  if (!buffer || !buffer->data || !buffer->length) {
    return nullptr;
  }
  buffer->offset = 0;
  return opaque;
}

uLong ReadZipBuffer(void* opaque, void* /* stream */, 
		    void* buf, uLong size) {
  ZipBuffer* buffer = static_cast<ZipBuffer *>(opaque);
  DCHECK_LE(buffer->offset, buffer->length);
  size_t remainning_bytes = buffer->length - buffer->offset;
  if (!buffer || !buffer->data || !remainning_bytes) {
    return 0;
  }
  size = std::min(size, static_cast<uLong>(remainning_bytes));
  memcpy(buf, &buffer->data[buffer->offset], size);
  buffer->offset += size;
  return size;
}

uLong WriteZipBuffer(void* /* opaque */,
		     void* /* stream */,
		     const void* /* buf */,
		     uLong /* size */) {
  // never come here
  return 0;
}

long GetOffsetOfZipBuffer(void* opaque, void* /* stream */) {
  ZipBuffer* buffer = static_cast<ZipBuffer *>(opaque);
  if (!buffer) {
    return -1;
  }
  return static_cast<long>(buffer->offset);
}

long SeekZipBuffer(void* opaque,
		   void* /* stream */,
		   uLong offset,
		   int origin) {
  ZipBuffer* buffer = static_cast<ZipBuffer *>(opaque);
  if (!buffer) {
    return -1;
  }

  if (origin == ZLIB_FILEFUNC_SEEK_CUR) {
    buffer->offset = std::min(buffer->offset + static_cast<size_t>(offset),
		              buffer->length);
    return 0;
  }
  if (origin == ZLIB_FILEFUNC_SEEK_END) {
    buffer->offset = (buffer->length > offset) ? buffer->length - offset
	             : 0;
    return 0;
  }
  if (origin == ZLIB_FILEFUNC_SEEK_SET) {
    buffer->offset = std::min(buffer->length, static_cast<size_t>(offset));
    return 0;
  }
  //never come here
  return -1;
}

int CloseZipBuffer(void* opaque, void* /* stream */) {
  if (opaque) {
    free(opaque);
  }
  return 0;
}

int GetErrorOfZipBuffer(void* /* opaque */, void* /* stream */) {
  return 0;
}

zip_fileinfo TimeToZipFileInfo(const base::Time& file_time) {
  base::Time::Exploded file_time_parts;
  file_time.LocalExplode(&file_time_parts);

  zip_fileinfo zip_info = {};
  if (file_time_parts.year >= 1980) {
    zip_info.tmz_date.tm_year = file_time_parts.year;
    zip_info.tmz_date.tm_mon = file_time_parts.month - 1;
    zip_info.tmz_date.tm_mday = file_time_parts.day_of_month;
    zip_info.tmz_date.tm_hour = file_time_parts.hour;
    zip_info.tmz_date.tm_min = file_time_parts.minute;
    zip_info.tmz_date.tm_sec = file_time_parts.second;
  }
  return zip_info;
}

} // namespace

namespace zip {
namespace internal {

unzFile OpenForUnzipping(const std::string& file_name) {
  zlib_filefunc_def* zip_func_ptrs = nullptr;
  return unzOpen2(file_name.c_str(), zip_func_ptrs);
}

unzFile OpenFdForUnzipping(int zip_fd) {
  zlib_filefunc_def zip_funcs;
  FillFdOpenFileFunc(&zip_funcs, zip_fd);
  return unzOpen2("fd", &zip_funcs);
}

// static
unzFile PrepareMemoryForUnzipping(const std::string& data) {
  if (data.empty()) {
    return nullptr;
  }

  ZipBuffer* buffer = static_cast<ZipBuffer *>(malloc(sizeof(ZipBuffer)));
  if (!buffer) {
    return nullptr;
  }

  buffer->data = data.data();
  buffer->length = data.length();
  buffer->offset = 0;

  zlib_filefunc_def zip_functions;
  zip_functions.zopen_file = OpenZipBuffer;
  zip_functions.zread_file = ReadZipBuffer;
  zip_functions.zwrite_file = WriteZipBuffer;
  zip_functions.ztell_file = GetOffsetOfZipBuffer;
  zip_functions.zseek_file = SeekZipBuffer;
  zip_functions.zclose_file = CloseZipBuffer;
  zip_functions.zerror_file = GetErrorOfZipBuffer;
  zip_functions.opaque = static_cast<void *>(buffer);
  return unzOpen2(nullptr, &zip_functions);
}

zipFile OpenForZipping(const std::string& file_name, int append_flag) {
  zlib_filefunc_def* zip_func_ptrs = nullptr;
  return zipOpen2(file_name.c_str(),
		  append_flag,
		  nullptr,
		  zip_func_ptrs);
}

zipFile OpenFdForZipping(int zip_fd, int append_flag) {
  zlib_filefunc_def zip_funcs;
  FillFdOpenFileFunc(&zip_funcs, zip_fd);
  return zipOpen2("fd", append_flag, nullptr, &zip_funcs);
}


zip_fileinfo GetFileInfoForZipping(const base::FilePath& path) {
  base::Time file_time;
  base::File::Info file_info;

  if (base::GetFileInfo(path, &file_info)) {
    file_time = file_info.last_modified;
  }
  return TimeToZipFileInfo(file_time);
}

bool ZipOpenNewFileInZip(zipFile zip_file,
		         const std::string& src_path,
			 const zip_fileinfo* file_info) {
  const uLong LANGUAGE_ENCODING_FLAG = 0x1 << 11;

  if (ZIP_OK != zipOpenNewFileInZip4(
			  zip_file,
			  src_path.c_str(),
			  file_info,
			  nullptr,
			  0u,
			  nullptr,
			  0u,
			  nullptr,
			  Z_DEFLATED,
			  Z_DEFAULT_COMPRESSION,
			  0,
			  -MAX_WBITS,
			  DEF_MEM_LEVEL,
			  Z_DEFAULT_STRATEGY,
			  nullptr,
			  0,
			  0,
			  LANGUAGE_ENCODING_FLAG)) {
  
    DLOG(ERROR) << "Could not open zip file entry " << src_path;
    return false;
  }
  return true;
}


} // internal
} // zip
