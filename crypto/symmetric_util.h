//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/30
//
// 对称密钥工具函数
#ifndef CRYPTO_SYMMETRIC_UTIL_H_
#define CRYPTO_SYMMETRIC_UTIL_H_
#include "crypto/input_stream.h"
#include "crypto/output_stream.h"

#include <string>

namespace crypto {

bool CbcEncrypt(const std::string& raw_key, 
                const std::string& iv,
                InputStream* in, OutputStream* out);

bool CbcDecrypt(const std::string& raw_key,
                const std::string& iv,
                InputStream* in, OutputStream* out);

bool EcbEncrypt(const std::string& raw_key,
                InputStream* in, OutputStream* out);

bool EcbDecrypt(const std::string& raw_key,
                InputStream* in, OutputStream* out);

} // namespace crypto
#endif // CRYPTO_SYMMETRIC_UTIL_H_
