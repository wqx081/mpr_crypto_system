#include "service/symmetric_service.h"

#include "crypto/symmetric_util.h"
#include "crypto/symmetric_key.h"
#include "crypto/symmetric_encryptor.h"
#include "crypto/symmetric_crypt.h"

#include "base/string_encode.h"

#include "crypto/memory_input_stream.h"
#include "crypto/memory_output_stream.h"
#include "crypto/file_input_stream.h"
#include "crypto/file_output_stream.h"

#include "base/file.h"
#include "base/file_path.h"
#include "base/file_util.h"

const char* kDefaultCbcIV = "mpr_iv:123456789";

namespace crypto {

grpc::Status SymmetricServiceImpl::CreateSymmetricKey(grpc::ServerContext* context,
                                const crypto::CreateSymmetricKeyRequest* request,
                                crypto::CreateSymmetricKeyResponse* response) {
  (void) context;
  LOG(INFO) << "----CreateSymmetricKey";
  crypto::SymmetricKeyBits key_bits = request->key_bits();
  if (key_bits != 128 && key_bits != 256) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "key_bits must be 128 or 256");
  }
  std::unique_ptr<SymmetricKey> key = crypto::SymmetricKey::GenerateRandomKey(
                                        crypto::SymmetricKey::AES,
                                        key_bits);
  if (key == nullptr) {
    return grpc::Status(grpc::INTERNAL, "GenerateRandomKey error");
  }
  std::string raw_key;
  key->GetRawKey(&raw_key);
  response->set_key(base::HexEncode(raw_key));

  return grpc::Status::OK;
}

grpc::Status SymmetricServiceImpl::CbcEncryptFile(grpc::ServerContext* context,
                                const crypto::CbcEncryptFileRequest* request,
                                crypto::CbcEncryptFileResponse* response) {
  (void) context;
  (void) response;

  std::string iv = request->iv().empty() ? kDefaultCbcIV : request->iv();
  std::string raw_key = base::HexDecode(request->key());
  std::string file_source_path = request->file_source_path();
  std::string file_target_path = request->file_target_path();

  FileInputStream in;
  FileOutputStream out;

  if (!in.Open(file_source_path.c_str())) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_source_path + " error");
  }
  if (!out.Open(file_target_path.c_str())) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_target_path + " error");
  }  
  if (!crypto::CbcEncrypt(raw_key, iv, &in, &out)) {
    return grpc::Status(grpc::INTERNAL, 
     "Key: " + request->key() + 
     ": Encrypt file " + file_source_path + " to " + file_target_path + " error");
  } 
  return grpc::Status::OK;
}

grpc::Status SymmetricServiceImpl::CbcDecryptFile(grpc::ServerContext* context,
                                                  const crypto::CbcDecryptFileRequest* request,
                                                  crypto::CbcDecryptFileResponse* response) {
  (void) context;
  (void) response;

  std::string iv = request->iv().empty() ? kDefaultCbcIV : request->iv();
  std::string raw_key = base::HexDecode(request->key()); 
  std::string file_source_path = request->file_source_path();
  std::string file_target_path = request->file_target_path();

  FileInputStream in;
  FileOutputStream out;

  if (!in.Open(file_source_path.c_str())) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_source_path + " error");
  }
  if (!out.Open(file_target_path.c_str())) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_target_path + " error");
  }  
  if (!crypto::CbcDecrypt(raw_key, iv, &in, &out)) {
    return grpc::Status(grpc::INTERNAL, 
     "cbc Key: " + request->key() + 
     ": Encrypt file " + file_source_path + " to " + file_target_path + " error");
  } 

  return grpc::Status::OK;
}

grpc::Status SymmetricServiceImpl::CbcEncryptString(grpc::ServerContext* context,
                                  const crypto::CbcEncryptStringRequest* request,
                                  crypto::CbcEncryptStringResponse* response) {
  (void) context;

  std::string iv = request->iv().empty() ? kDefaultCbcIV : request->iv();
  std::string raw_key = base::HexDecode(request->key()); 
  std::string plaintext = request->plaintext();

  MemoryInputStream in(plaintext);
  MemoryOutputStream out;

  if (!crypto::CbcEncrypt(raw_key, iv, &in, &out)) {
    return grpc::Status(grpc::INTERNAL, "cbc encrypt string error");
  }
  response->set_cipher(base::HexEncode(out.ToString()));
  return grpc::Status::OK;
}                  

grpc::Status SymmetricServiceImpl::CbcDecryptString(grpc::ServerContext* context,
                                  const crypto::CbcDecryptStringRequest* request,
                                  crypto::CbcDecryptStringResponse* response) { 
  (void) context;

  std::string iv = request->iv().empty() ? kDefaultCbcIV : request->iv();
  std::string raw_key = base::HexDecode(request->key()); 
  std::string cipher = base::HexDecode(request->cipher());

  MemoryInputStream in(cipher);
  MemoryOutputStream out;

  if (!crypto::CbcDecrypt(raw_key, iv, &in, &out)) { 
    return grpc::Status(grpc::INTERNAL, "cbc decrypt string error");
  } 
  response->set_plaintext(out.ToString());
  return grpc::Status::OK;
}                   

// ECB

grpc::Status SymmetricServiceImpl::EcbEncryptFile(grpc::ServerContext* context,
                                const crypto::EcbEncryptFileRequest* request,
                                crypto::EcbEncryptFileResponse* response) {
  (void) context;
  (void) response;

  std::string raw_key = base::HexDecode(request->key());
  std::string file_source_path = request->file_source_path();
  std::string file_target_path = request->file_target_path();

  FileInputStream in;
  FileOutputStream out;

  if (!in.Open(file_source_path.c_str())) {
    LOG(ERROR) << "open source file " << file_source_path; 
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_source_path + " error");
  }
  if (!out.Open(file_target_path.c_str())) {
    LOG(ERROR) << "open target file " << file_source_path; 
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_target_path + " error");
  }  
  if (!crypto::EcbEncrypt(raw_key, &in, &out)) {
    LOG(ERROR) << "ecb encyrpt "; 
    return grpc::Status(grpc::INTERNAL, 
     "ecb Key: " + request->key() + 
     ": Encrypt file " + file_source_path + " to " + file_target_path + " error");
  } 
  return grpc::Status::OK;
}

grpc::Status SymmetricServiceImpl::EcbDecryptFile(grpc::ServerContext* context,
                                  const crypto::EcbDecryptFileRequest* request,
                                  crypto::EcbDecryptFileResponse* response) {
  (void) context;
  (void) response;

  std::string raw_key = base::HexDecode(request->key()); 
  std::string file_source_path = request->file_source_path();
  std::string file_target_path = request->file_target_path();

  FileInputStream in;
  FileOutputStream out;

  if (!in.Open(file_source_path.c_str())) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_source_path + " error");
  }
  if (!out.Open(file_target_path.c_str())) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "open file: " + file_target_path + " error");
  }  
  if (!crypto::EcbDecrypt(raw_key, &in, &out)) {
    return grpc::Status(grpc::INTERNAL, 
     "ecb Key: " + request->key() + 
     ": Encrypt file " + file_source_path + " to " + file_target_path + " error");
  } 

  return grpc::Status::OK;
}

grpc::Status SymmetricServiceImpl::EcbEncryptString(grpc::ServerContext* context,
                                  const crypto::EcbEncryptStringRequest* request,
                                  crypto::EcbEncryptStringResponse* response) {
  (void) context;

  std::string raw_key = base::HexDecode(request->key()); 
  std::string plaintext = request->plaintext();

  MemoryInputStream in(plaintext);
  MemoryOutputStream out;

  if (!crypto::EcbEncrypt(raw_key, &in, &out)) {
    return grpc::Status(grpc::INTERNAL, "ecb encrypt string error");
  }
  response->set_cipher(base::HexEncode(out.ToString()));
  return grpc::Status::OK;
}                  

grpc::Status SymmetricServiceImpl::EcbDecryptString(grpc::ServerContext* context,
                                  const crypto::EcbDecryptStringRequest* request,
                                  crypto::EcbDecryptStringResponse* response) { 
  (void) context;

  std::string raw_key = base::HexDecode(request->key()); 
  std::string cipher = base::HexDecode(request->cipher());

  MemoryInputStream in(cipher);
  MemoryOutputStream out;

  if (!crypto::EcbDecrypt(raw_key, &in, &out)) { 
    return grpc::Status(grpc::INTERNAL, "ecb decrypt string error");
  } 
  response->set_plaintext(out.ToString());
  return grpc::Status::OK;
}                   

} // namespace crypto
