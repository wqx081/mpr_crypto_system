#ifndef SERVICE_SM2_SERVICE_H_
#define SERVICE_SM2_SERVICE_H_

#include "base/macros.h"
#include <glog/logging.h>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "protos/crypto_server.grpc.pb.h"

namespace crypto {

class AsymmetricServiceImpl final : public crypto::AsymmetricService::Service {
 public:
  AsymmetricServiceImpl() {}
  virtual ~AsymmetricServiceImpl() {}

  virtual grpc::Status CreateKeyPair(grpc::ServerContext* context,
                                     const crypto::CreateKeyPairRequest* request,
                                     crypto::CreateKeyPairResponse* response) override;
  virtual grpc::Status PublicKeyEncrypt(grpc::ServerContext* context,
                                        const crypto::PublicKeyEncryptRequest* request,
                                        crypto::PublicKeyEncryptResponse* response) override;
  virtual grpc::Status PrivateKeyDecrypt(grpc::ServerContext* context,
                                        const crypto::PrivateKeyDecryptRequest* request,
                                         crypto::PrivateKeyDecryptResponse* response) override;

};

} // namespace crypto
#endif // SERVICE_SM2_SERVICE_H_
