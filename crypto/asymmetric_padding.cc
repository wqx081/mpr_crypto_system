#include "crypto/asymmetric_padding.h"

#include <glog/logging.h>

namespace crypto {


namespace {

void FillPaddingInternal(std::string& input, int block_size, int v) {
  int input_len = static_cast<int>(input.size());
  if (((input_len == block_size) || input_len == 0) && v == 0) {
    // Nothing to do.
  } else {
    // padding
    input.insert(input.size(), input_len == block_size ?  block_size : 
                               block_size - input.size(), v);
  }
}

void TrimPaddingInternal(std::string& input, int block_size, int v) {
  int t = input[input.size()-1];
  if (t == v) {
    if (v == 0) {
      auto end = input.find_first_of(v);  
      std::string sub = input.substr(0, end);
      input = sub;
    } else {
      auto it = input.end() - 1;
      int c = 0;
      while (*it == t) {
        ++c;
        --it;
      }
      if (c == t) {
        std::string sub = input.substr(0, block_size - t);
        input  = sub;
      }
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
  LOG(INFO) << "---v: " << v;
  FillPaddingInternal(str, block_size, v);
}

void MprPadding::TrimPadding(std::string& str, int block_size) {
  TrimPaddingInternal(str, block_size, str[str.size()-1]);
}

} // namespace crypto
