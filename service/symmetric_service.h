#ifndef SERVICE_SYMMETRIC_SERVICE_H_
#define SERVICE_SYMMETRIC_SERVICE_H_
#include "base/macros.h"
#include <glog/logging.h>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "protos/crypto_server.grpc.pb.h"

namespace crypto {

class SymmetricServiceImpl final : public crypto::SymmetricService::Service {
 public:
  SymmetricServiceImpl() {}
  virtual ~SymmetricServiceImpl() override {}

  virtual grpc::Status CreateSymmetricKey(grpc::ServerContext* context,
                                 const crypto::CreateSymmetricKeyRequest* request,
                                 crypto::CreateSymmetricKeyResponse* response) override;

  // CBC
  virtual grpc::Status CbcEncryptFile(grpc::ServerContext* context,
                                      const crypto::CbcEncryptFileRequest* request,
                                      crypto::CbcEncryptFileResponse* response) override;
  virtual grpc::Status CbcDecryptFile(grpc::ServerContext* context,
                                      const crypto::CbcDecryptFileRequest* request,
                                      crypto::CbcDecryptFileResponse* response) override;
  virtual grpc::Status CbcEncryptString(grpc::ServerContext* context,
                                        const crypto::CbcEncryptStringRequest* request,
                                        crypto::CbcEncryptStringResponse* response) override;
  virtual grpc::Status CbcDecryptString(grpc::ServerContext* context,
                                        const crypto::CbcDecryptStringRequest* request,
                                        crypto::CbcDecryptStringResponse* response) override;

  // ECB
  virtual grpc::Status EcbEncryptFile(grpc::ServerContext* context,
                                      const crypto::EcbEncryptFileRequest* request,
                                      crypto::EcbEncryptFileResponse* response) override;
  virtual grpc::Status EcbDecryptFile(grpc::ServerContext* context,
                                      const crypto::EcbDecryptFileRequest* request,
                                      crypto::EcbDecryptFileResponse* response) override;
  virtual grpc::Status EcbEncryptString(grpc::ServerContext* context,
                                        const crypto::EcbEncryptStringRequest* request,
                                        crypto::EcbEncryptStringResponse* response) override;
  virtual grpc::Status EcbDecryptString(grpc::ServerContext* context,
                                        const crypto::EcbDecryptStringRequest* request,
                                        crypto::EcbDecryptStringResponse* response) override;
  
 private:
  DISALLOW_COPY_AND_ASSIGN(SymmetricServiceImpl);
};

} // namespace crypto
#endif // SERVICE_SYMMETRIC_SERVICE_H_
