#ifndef ZIP_ZIP_INTERNAL_H_
#define ZIP_ZIP_INTERNAL_H_

#include <string>
#include <minizip/unzip.h>
#include <minizip/zip.h>

namespace base {
class FilePath;
}

namespace zip {
namespace internal {

unzFile OpenForUnzipping(const std::string& file_name);
unzFile OpenFdForUnzipping(int zip_fd);
unzFile PrepareMemoryForUnzipping(const std::string& data);

zipFile OpenForZipping(const std::string& file_name, int append_flag);
zipFile OpenFdForZipping(int zip_fd, int append_flag);
zip_fileinfo GetFileInfoForZipping(const base::FilePath& path);
bool ZipOpenNewFileInZip(zipFile zip_file,
		         const std::string& str_path,
			 const zip_fileinfo* file_info);

const int kZipMaxPath = 256;
const int kZipBufSize = 8192;

} // internal
} // zip
#endif // ZIP_ZIP_INTERNAL_H_
