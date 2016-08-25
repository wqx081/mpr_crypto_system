//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/25
//
//
//PaddingInterface 定义了Padding 的策略接口. 
//
//ZeroPadding
//  在加密的时候，如果输入不足32字节，那么以0填充
//  这种策略适用于对m3u8的媒体key的URL做加密
//
//MprPadding
//  兼容PKCS5 如果还差n 到32字节，那么我们填充 n 个值为 n 字节.
//  如果正好32个字节，那么填充二位的32个字节.
//
#ifndef CRYPTO_ASSYMETRIC_PADDING_H_
#define CRYPTO_ASSYMETRIC_PADDING_H_

#include <string>

namespace crypto {

class PaddingInterface {
 public:
  virtual ~PaddingInterface() {}
  virtual void FillPadding(std::string& str, int block_size) = 0;
  virtual void TrimPadding(std::string& str, int block_size) = 0;
};

class ZeroPadding : public PaddingInterface {
 public:
  virtual void FillPadding(std::string& str, int block_size) override;
  virtual void TrimPadding(std::string& str, int block_size) override;
};

class MprPadding : public PaddingInterface {
 public:
  virtual void FillPadding(std::string& str, int block_size) override;
  virtual void TrimPadding(std::string& str, int block_size) override;
};

} // namespace crypto
#endif // CRYPTO_ASSYMETRIC_PADDING_H_
