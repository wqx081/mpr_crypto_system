#ifndef ZIP_ZIP_H_
#define ZIP_ZIP_H_
#include <vector>
#include <functional>

#include "base/file_path.h"

namespace zip {

typedef std::function<bool(const base::FilePath&)> FilterCallback;

bool ZipWithFilterCallback(const base::FilePath& src_dir,
                           const base::FilePath& dst_dir,
                           const FilterCallback& filter_cb);

bool Zip(const base::FilePath& src_dir,
         const base::FilePath& dst_file,
         bool include_hidden_files);
bool ZipFiles(const base::FilePath& src_dir,
              const std::vector<base::FilePath>& src_relative_paths,
              int dst_fd);

bool Unzip(const base::FilePath& zip_file,
           const base::FilePath& dst_dir);

} // namespace zip
#endif // ZIP_ZIP_H_
