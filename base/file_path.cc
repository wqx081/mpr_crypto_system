#include "base/file_path.h"

#include <glog/logging.h>

#include "base/macros.h"
#include "base/string_piece.h"
#include "base/string_util.h"

namespace base {

const FilePath::CharType FilePath::kSeparators[] = "/";
const FilePath::CharType FilePath::kCurrentDirectory[] = ".";
const FilePath::CharType FilePath::kParentDirectory[] = "..";
const FilePath::CharType FilePath::kExtensionSeparator='.';
const FilePath::CharType kStringTerminator = FILE_PATH_LITERAL('\0');

typedef FilePath::StringType StringType;
typedef FilePath::StringPieceType StringPieceType;

namespace {

const char* kCommonDoubleExtensions[] = {"gz", "z", "bz2"};

StringType::size_type FindDriveLetter(const StringType& path) {
    (void)path;
    return StringType::npos;
}

bool IsPathAbsolute(const StringType& path) {
  return path.length() > 0 && FilePath::IsSeparator(path[0]);
}

bool IsPathAbsolute(StringPieceType path) {
  return path.length() > 0 && FilePath::IsSeparator(path[0]);
}

bool AreAllSeparators(const StringType& input) {
    for (StringType::const_iterator it=input.begin();
            it != input.end(); ++it) {
        if (!FilePath::IsSeparator(*it))
            return false;
    }
    return true;
}

StringType::size_type ExtensionSeparatorPosition(const StringType& path) {
    if (path == FilePath::kCurrentDirectory ||
            path == FilePath::kParentDirectory)
        return StringType::npos;
    const StringType::size_type last_dot =
        path.rfind(FilePath::kExtensionSeparator);

    if (last_dot == StringType::npos || last_dot == 0U)
        return last_dot;

    StringType extension(path, last_dot + 1);
    bool is_common_double_extension = false;
    for (size_t i=0; i < ARRAYSIZE(kCommonDoubleExtensions); ++i) {
        if (LowerCaseEqualsASCII(extension, kCommonDoubleExtensions[i]))
            is_common_double_extension = true;
    }
    if (!is_common_double_extension)
        return last_dot;

    const StringType::size_type penultimate_dot =
        path.rfind(FilePath::kExtensionSeparator, last_dot - 1);
    const StringType::size_type last_separator =
        path.find_last_of(FilePath::kSeparators, last_dot - 1,
                          ARRAYSIZE(FilePath::kSeparators) - 1);
    if (penultimate_dot != StringType::npos &&
            (last_separator == StringType::npos ||
             penultimate_dot > last_separator) &&
            last_dot - penultimate_dot <= 5U &&
            last_dot - penultimate_dot > 1U) {
        return penultimate_dot;
    }

    return last_dot;
}


} // namespace

FilePath::FilePath() {
}

FilePath::FilePath(const FilePath& other)
    : path_(other.path_) {
}

#if 0
FilePath::FilePath(const StringType& path)
    : path_(path) {
}
#endif

FilePath::FilePath(StringPieceType path) {
  path.CopyToString(&path_);
  StringType::size_type nul_pos = path_.find(kStringTerminator);
  if (nul_pos != StringType::npos)
    path_.erase(nul_pos, StringType::npos);
}

FilePath::~FilePath() {
}

FilePath& FilePath::operator=(const FilePath& other) {
    path_ = other.path_;
    return *this;
}

bool FilePath::IsSeparator(CharType c) {
    for (size_t i=0; i < ARRAYSIZE(kSeparators)-1; ++i) {
        if (c == kSeparators[i]) {
            return true;
        }
    }
    return false;
}

void FilePath::GetComponents(std::vector<FilePath::StringType>* c) const
{
    DCHECK(c);
    if (!c)
        return;
    c->clear();
    if (value().empty())
        return;

    std::vector<StringType> ret_val;
    FilePath current = *this;
    FilePath base;

    while (current != current.DirName()) {
        base = current.BaseName();
        if (!AreAllSeparators(base.value())) {
            ret_val.push_back(base.value());
        }
        current = current.DirName();
    }

    base = current.BaseName();
    if (!base.value().empty() && base.value() != kCurrentDirectory) {
        ret_val.push_back(current.BaseName().value());
    }

    FilePath dir = current.DirName();
    StringType::size_type letter = FindDriveLetter(dir.value());
    if (letter != StringType::npos) {
        ret_val.push_back(StringType(dir.value(), 0, letter + 1));
    }
    * c = std::vector<StringType>(ret_val.rbegin(), ret_val.rend());
}

bool FilePath::operator==(const FilePath& other) const {
    return path_ == other.path_;
}

bool FilePath::operator!=(const FilePath& other) const {
    return path_ != other.path_;
}

bool FilePath::IsParent(const FilePath& child) const {
    return AppendRelativePath(child, nullptr);
}

bool FilePath::AppendRelativePath(const FilePath& child,
                                  FilePath* path) const {
    std::vector<StringType> parent_components;
    std::vector<StringType> child_components;
    GetComponents(&parent_components);
    child.GetComponents(&child_components);

    if (parent_components.size() >= child_components.size())
        return false;
    if (parent_components.size() == 0)
        return false;

    std::vector<StringType>::const_iterator parent_comp =
        parent_components.begin();
    std::vector<StringType>::const_iterator child_comp =
        child_components.begin();

    while (parent_comp != parent_components.end()) {
        if (*parent_comp != *child_comp)
            return false;

        ++parent_comp;
        ++child_comp;
    }

    if (path != nullptr) {
        for (; child_comp != child_components.end(); ++child_comp) {
            *path = path->Append(*child_comp);
        }
    }
    return true;
}

FilePath FilePath::DirName() const {
    FilePath new_path(path_);
    new_path.StripTrailingSeparatorsInternal();

    StringType::size_type letter = FindDriveLetter(new_path.path_);
    StringType::size_type last_separator =
        new_path.path_.find_last_of(kSeparators, StringType::npos,
                                    ARRAYSIZE(kSeparators) - 1);

    if (last_separator == StringType::npos) {
        new_path.path_.resize(letter + 1);
    } else if (last_separator == letter + 1) {
        new_path.path_.resize(letter + 2);
    } else if (last_separator == letter + 2 &&
               IsSeparator(new_path.path_[letter + 1])) {
        new_path.path_.resize(letter + 3);
    } else if (last_separator != 0) {
        new_path.path_.resize(last_separator);
    }

    new_path.StripTrailingSeparatorsInternal();
    if (!new_path.path_.length()) {
        new_path.path_ = kCurrentDirectory;
    }
    return new_path;
}

FilePath FilePath::BaseName() const {
    FilePath new_path(path_);
    new_path.StripTrailingSeparatorsInternal();

    StringType::size_type letter = FindDriveLetter(new_path.path_);
    if (letter != StringType::npos) {
        new_path.path_.erase(0, letter + 1);
    }

    StringType::size_type last_separator =
        new_path.path_.find_last_of(kSeparators, StringType::npos,
                                    ARRAYSIZE(kSeparators) - 1);
    if (last_separator != StringType::npos &&
            last_separator < new_path.path_.length() - 1) {
        new_path.path_.erase(0, last_separator + 1);
    }

    return new_path;
}

StringType FilePath::Extension() const {
    FilePath base(BaseName());
    const StringType::size_type dot = ExtensionSeparatorPosition(base.path_);
    if (dot == StringType::npos)
        return StringType();

    return base.path_.substr(dot, StringType::npos);
}

FilePath FilePath::RemoveExtension() const {
    if (Extension().empty())
        return *this;

    const StringType::size_type dot = ExtensionSeparatorPosition(path_);
    if (dot == StringType::npos)
        return *this;

    return FilePath(path_.substr(0, dot));
}

FilePath FilePath::InsertBeforeExtension(StringType suffix) const {
    if (suffix.empty())
        return FilePath(path_);

    if (path_.empty())
        return FilePath();

    StringType base = BaseName().value();
    if (base.empty())
        return FilePath();
    if (*(base.end() - 1) == kExtensionSeparator) {
        if (base == kCurrentDirectory || base == kParentDirectory) {
            return FilePath();
        }
    }

    StringType ext = Extension();
    StringType ret = RemoveExtension().value();
    ret.append(suffix);
    ret.append(ext);
    return FilePath(ret);
}


FilePath FilePath::InsertBeforeExtensionASCII(StringPiece suffix) const {
    return InsertBeforeExtension(suffix.as_string());
}

FilePath FilePath::ReplaceExtension(const StringType& extension) const {
    if (path_.empty())
        return FilePath();

    StringType base = BaseName().value();
    if (base.empty())
        return FilePath();
    if (*(base.end() - 1) == kExtensionSeparator) {
// Special case "." and ".."
        if (base == kCurrentDirectory || base == kParentDirectory) {
            return FilePath();
        }
    }

    FilePath no_ext = RemoveExtension();
// If the new extension is "" or ".", then just remove the current extension  .
    if (extension.empty() || extension == StringType(1, kExtensionSeparator))
        return no_ext;

    StringType str = no_ext.value();
    if (extension[0] != kExtensionSeparator)
        str.append(1, kExtensionSeparator);
    str.append(extension);
    return FilePath(str);
}

bool FilePath::MatchesExtension(const StringType& extension) const {
    DCHECK(extension.empty() || extension[0] == kExtensionSeparator);

    StringType current_extension = Extension();

    if (current_extension.length() != extension.length())
        return false;

    return FilePath::CompareEqualIgnoreCase(extension, current_extension);
}


FilePath FilePath::Append(StringPieceType component) const {
    
  StringPieceType appended = component;
  StringType without_nuls;

  StringType::size_type nul_pos = component.find(kStringTerminator);
  if (nul_pos != StringPieceType::npos) {
    component.substr(0, nul_pos).CopyToString(&without_nuls);
    appended = StringPieceType(without_nuls);
  }

  DCHECK(!IsPathAbsolute(appended));

  if (path_.compare(kCurrentDirectory) == 0) {
    return FilePath(appended);
  }

  FilePath new_path(path_);
  new_path.StripTrailingSeparatorsInternal();

  if (!appended.empty() && !new_path.path_.empty()) {
    if (!IsSeparator(new_path.path_.back())) {
      if (FindDriveLetter(new_path.path_) + 1 != new_path.path_.length()) {
        new_path.path_.append(1, kSeparators[0]);
      }
    }
  }

  appended.AppendToString(&new_path.path_);
  return new_path;

}

#if 0
FilePath FilePath::Append(const StringType& component) const {
    if (path_.compare(kCurrentDirectory) == 0) {
        return FilePath(component);
    }
    FilePath new_path(path_);
    new_path.StripTrailingSeparatorsInternal();
    if (component.length() > 0 && new_path.path_.length() > 0) {
        if (!IsSeparator(new_path.path_[new_path.path_.length()-1])) {

            if (FindDriveLetter(new_path.path_) + 1 != new_path.path_.length()) {
                new_path.path_.append(1, kSeparators[0]);
            }
        }
    }
    new_path.path_.append(component);
    return new_path;
}
#endif

FilePath FilePath::Append(const FilePath& component) const {
    return Append(component.value());
}

FilePath FilePath::AppendASCII(StringPiece component) const {
  return Append(component);
}

int FilePath::CompareIgnoreCase(const StringType& string1,
                                const StringType& string2) {
    int comparison = ::strcasecmp(string1.c_str(), string2.c_str());
    if (comparison < 0)
        return -1;
    if (comparison > 0)
        return 1;
    return 0;
}

FilePath FilePath::StripTrailingSeparators() const {
    FilePath new_path(path_);
    new_path.StripTrailingSeparatorsInternal();

    return new_path;
}

void FilePath::StripTrailingSeparatorsInternal() {
    StringType::size_type start = FindDriveLetter(path_) + 2;

    StringType::size_type last_stripped = StringType::npos;
    for (StringType::size_type pos = path_.length();
            pos > start && IsSeparator(path_[pos - 1]);
            --pos) {
        if (pos != start + 1 || last_stripped == start + 2 ||
                !IsSeparator(path_[start - 1])) {
            path_.resize(pos - 1);
            last_stripped = pos;
        }
    }
}

bool FilePath::ReferencesParent() const {
    std::vector<StringType> components;
    GetComponents(&components);

    std::vector<StringType>::const_iterator it = components.begin();
    for (; it != components.end(); ++it) {
        const StringType& component = *it;
        if (component == kParentDirectory)
            return true;
    }
    return false;
}

bool FilePath::IsAbsolute() const {
  return IsPathAbsolute(path_);
}

} // namespace base
