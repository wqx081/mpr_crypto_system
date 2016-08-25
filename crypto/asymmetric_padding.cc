#include "crypto/asymmetric_padding.h"

namespace crypto {


namespace {

void FillPaddingInternal(std::string& input, int block_size, int v) {
  int input_len = static_cast<int>(input.size());
  if (((input_len == block_size) || input_len == 0) && v == 0) {
    // Nothing to do.
  } else {
    // padding
    input.insert(input.size(), block_size - input.size(), v);
  }
}

void TrimPaddingInternal(std::string& input, int block_size, int v) {
  (void) block_size;
  if (input[input.size()-1] == v) {
    if (v == 0) {
      auto end = input.find_first_of(v);  
      std::string sub = input.substr(0, end);
      input = sub;
    } else {
      std::string sub = input.substr(0, v);
    }
  }
}

} // namespace

void ZeroPadding::FillPadding(std::string& str, int block_size) {
  FillPaddingInternal(str, block_size, 0);
}

void ZeroPadding::TrimPadding(std::string& str, int block_size) {
  TrimPaddingInternal(str, block_size, 0);
}

void MprPadding::FillPadding(std::string& str, int block_size) {
  int v = (block_size == static_cast<int>(str.size())) ?  block_size 
                      : block_size - str.size();
  FillPaddingInternal(str, block_size, v);
}

void MprPadding::TrimPadding(std::string& str, int block_size) {
  int v = (block_size == static_cast<int>(str.size())) ?  block_size 
                      : block_size - str.size();
  TrimPaddingInternal(str, block_size, v);
}

} // namespace crypto
