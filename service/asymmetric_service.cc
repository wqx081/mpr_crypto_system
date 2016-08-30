#include "service/asymmetric_service.h"
#include "crypto/asymmetric_encryptor.h"
#include "crypto/sm2_asymmetric_encryptor.h"
#include "crypto/asymmetric_key.h"

#include "base/string_encode.h"
#include "crypto/memory_input_stream.h"
#include "crypto/memory_output_stream.h"

namespace crypto {

grpc::Status AsymmetricServiceImpl::CreateKeyPair(grpc::ServerContext* context,
                                                  const crypto::CreateKeyPairRequest* request,
                                                  crypto::CreateKeyPairResponse* response) {
  (void) context;
  crypto::AsymmetricKeyBits key_bits = request->key_bits();
  if (key_bits != 192 && key_bits != 256) {
    return grpc::Status(grpc::INVALID_ARGUMENT, "key_bits must be 128 or 256");
  }

  std::unique_ptr<AsymmetricKey> key = Sm2AsymmetricEncryptor::GetInstance()->Create((AsymmetricEncryptor::KeyBits)key_bits); 
  if (!key) {
    return grpc::Status(grpc::INTERNAL, "sm2 create key pair error");
  }
  
  ByteVector public_key;
  ByteVector private_key;

  if (!key->ExportPublicKey(&public_key)) {
    return grpc::Status(grpc::INTERNAL, "Export public key error");
  }
  if (!key->ExportPrivateKey(&private_key)) {
    return grpc::Status(grpc::INTERNAL, "Export private key error");
  }

  response->set_public_key( base::HexEncode((const char *)public_key.data(), public_key.size()));
  response->set_private_key( base::HexEncode((const char*) private_key.data(), private_key.size()));

  return grpc::Status::OK;
}

grpc::Status AsymmetricServiceImpl::PublicKeyEncrypt(grpc::ServerContext* context,
                                                     const crypto::PublicKeyEncryptRequest* request,
                                                     crypto::PublicKeyEncryptResponse* response) {
  (void) context;
  std::string public_key_string = request->public_key();
  std::string plaintext = request->plaintext();
  std::string public_key_hex  = base::HexDecode(public_key_string);
  std::unique_ptr<AsymmetricKey> key = Sm2Key::FromBuffer((uint8_t *)public_key_hex.data(), public_key_hex.size(), nullptr, 0);

  if (!key) {
    return grpc::Status(grpc::INTERNAL, "Export public key error");
  }

  InputStream* encrypt_is(new MemoryInputStream((const Byte *)plaintext.data(), plaintext.size()));
  OutputStream* encrypt_os(new MemoryOutputStream);

  if (!Sm2AsymmetricEncryptor::GetInstance()->PublicKeyEncrypt(key,
                                                               encrypt_is,
                                                               encrypt_os)) {
    delete encrypt_is;
    delete encrypt_os;
    return grpc::Status(grpc::INTERNAL, "public key encrypt error");
  }

  response->set_cipher(base::HexEncode(encrypt_os->ToString()));

  delete encrypt_is;
  delete encrypt_os;
  return grpc::Status::OK;
}

grpc::Status AsymmetricServiceImpl::PrivateKeyDecrypt(grpc::ServerContext* context,
                                                      const crypto::PrivateKeyDecryptRequest* request,
                                                      crypto::PrivateKeyDecryptResponse* response) {
  (void) context;
  std::string private_key_string = request->private_key();
  std::string cipher = base::HexDecode(request->cipher());
  std::string private_key_hex  = base::HexDecode(private_key_string);
  std::unique_ptr<AsymmetricKey> key = Sm2Key::FromBuffer(nullptr, 0, (uint8_t *) private_key_hex.data(), private_key_hex.size());
                
  if (!key) { 
    return grpc::Status(grpc::INTERNAL, "Export public key error");
  }           
                
  InputStream* decrypt_is(new MemoryInputStream((const Byte *)cipher.data(), cipher.size()));
  OutputStream* decrypt_os(new MemoryOutputStream);

  if (!Sm2AsymmetricEncryptor::GetInstance()->PrivateKeyDecrypt(key,
                                                                decrypt_is,
                                                                decrypt_os)) {
    delete decrypt_is;
    delete decrypt_os;
    return grpc::Status(grpc::INTERNAL, "private key decrypt error");
  }

  response->set_plaintext(decrypt_os->ToString());

  return grpc::Status::OK;
}


} // namespace crypto
