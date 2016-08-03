#ifndef SPARROW_FILES_FILE_PATH_H_
#define SPARROW_FILES_FILE_PATH_H_

#include <string>
#include <vector>

#include "base/string_piece.h"

#define FILE_PATH_LITERAL(x) x
#define PRFilePath "s"
#define PRFilePathLiteral "%s"

namespace base {

class FilePath {
 public:
  typedef std::string StringType;
  typedef BasicStringPiece<StringType> StringPieceType;
  typedef StringType::value_type CharType;

  static const CharType kSeparators[];
  static const CharType kCurrentDirectory[];
  static const CharType kParentDirectory[];
  static const CharType kExtensionSeparator;
  static bool IsSeparator(CharType character);

  FilePath();
  FilePath(const FilePath& other);
//  explicit FilePath(const StringType& path);
  explicit FilePath(StringPieceType path);

  ~FilePath();
  FilePath& operator=(const FilePath& other);

  bool operator==(const FilePath& other) const;
  bool operator!=(const FilePath& other) const;
  bool operator<(const FilePath& other) const {
    return path_ < other.path_;
  }
  const StringType& value() const { return path_; }
  bool empty() const { return path_.empty(); }
  void clear() { path_.clear(); }

  void GetComponents(std::vector<FilePath::StringType>* components) const;
  bool IsParent(const FilePath& child) const;
  bool AppendRelativePath(const FilePath& child, FilePath* path) const;
  FilePath DirName() const;
  FilePath BaseName() const;
  StringType Extension() const;
  FilePath RemoveExtension() const;

  FilePath InsertBeforeExtension(StringType suffix) const;
  FilePath InsertBeforeExtensionASCII(StringPiece suffix) const;

  FilePath ReplaceExtension(const StringType& extension) const;
  bool MatchesExtension(const StringType& extension) const;

  FilePath Append(StringPieceType component) const;
//  FilePath Append(const StringType& component) const;
  FilePath Append(const FilePath& component) const;

  FilePath AppendASCII(StringPiece component) const;

  bool IsAbsolute() const;
  FilePath StripTrailingSeparators() const;
  bool ReferencesParent() const;

  static int CompareIgnoreCase(const StringType& s1,
		               const StringType& s2);
  static bool CompareEqualIgnoreCase(const StringType& s1,
		                     const StringType& s2) {
    return CompareIgnoreCase(s1, s2) == 0;
  }
  static bool CompareLessIgnoreCase(const StringType& s1,
		                    const StringType& s2) {
    return CompareIgnoreCase(s1, s2) < 0;
  }

 private:
  void StripTrailingSeparatorsInternal();
  StringType path_;
};


} // namespace base

#endif // SPARROW_FILES_FILE_PATH_H_
