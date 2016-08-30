#ifndef SERVICE_CRYPTO_SERVER_H_
#define SERVICE_CRYPTO_SERVER_H_
#include "service/asymmetric_service.h"
#include "service/symmetric_service.h"

#include <string>

namespace crypto {

class CryptoServer {
 public:
  CryptoServer(const std::string& host, int port)
    : host_(host), port_(port) {}

  void Run() {
    std::string server_address(host_+":"+std::to_string(port_));
    AsymmetricServiceImpl asymmetric_service;
    SymmetricServiceImpl symmetric_service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address,
                             grpc::InsecureServerCredentials());

    builder.RegisterService(&asymmetric_service);
    builder.RegisterService(&symmetric_service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG(INFO) << "Server listening on " << server_address;
    server->Wait();
  }

 private:
  std::string host_;
  int port_;

  DISALLOW_COPY_AND_ASSIGN(CryptoServer);
};

} // namespace crypto
#endif // SERVICE_CRYPTO_SERVER_H_
