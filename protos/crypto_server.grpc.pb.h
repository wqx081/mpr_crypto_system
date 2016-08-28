// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: crypto_server.proto
// Original file comments:
// @Author: WangQixiang (wangqx at mpreader.com)
// @Date: 2016/08/29
#ifndef GRPC_crypto_5fserver_2eproto__INCLUDED
#define GRPC_crypto_5fserver_2eproto__INCLUDED

#include "crypto_server.pb.h"

#include <grpc++/impl/codegen/async_stream.h>
#include <grpc++/impl/codegen/async_unary_call.h>
#include <grpc++/impl/codegen/proto_utils.h>
#include <grpc++/impl/codegen/rpc_method.h>
#include <grpc++/impl/codegen/service_type.h>
#include <grpc++/impl/codegen/status.h>
#include <grpc++/impl/codegen/stub_options.h>
#include <grpc++/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class RpcService;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace crypto {

class SymmetricService GRPC_FINAL {
 public:
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status CreateCbcKey(::grpc::ClientContext* context, const ::crypto::CreateCbcKeyRequest& request, ::crypto::CreateCbcKeyResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CreateCbcKeyResponse>> AsyncCreateCbcKey(::grpc::ClientContext* context, const ::crypto::CreateCbcKeyRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CreateCbcKeyResponse>>(AsyncCreateCbcKeyRaw(context, request, cq));
    }
    // TODO 
    // rpc CbcEncryptString(CbcEncryptStringRequest) returns (CbcEncryptStringResponse);
    virtual ::grpc::Status CbcEncryptFile(::grpc::ClientContext* context, const ::crypto::CbcEncryptFileRequest& request, ::crypto::CbcEncryptFileResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CbcEncryptFileResponse>> AsyncCbcEncryptFile(::grpc::ClientContext* context, const ::crypto::CbcEncryptFileRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CbcEncryptFileResponse>>(AsyncCbcEncryptFileRaw(context, request, cq));
    }
    // TODO
    // rpc CbcDecryptString(CbcDecryptStringRequest) returns (CbcDecryptStringResponse);
    virtual ::grpc::Status CbcDecryptFile(::grpc::ClientContext* context, const ::crypto::CbcDecryptFileRequest& request, ::crypto::CbcDecryptFileResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CbcDecryptFileResponse>> AsyncCbcDecryptFile(::grpc::ClientContext* context, const ::crypto::CbcDecryptFileRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CbcDecryptFileResponse>>(AsyncCbcDecryptFileRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CreateCbcKeyResponse>* AsyncCreateCbcKeyRaw(::grpc::ClientContext* context, const ::crypto::CreateCbcKeyRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CbcEncryptFileResponse>* AsyncCbcEncryptFileRaw(::grpc::ClientContext* context, const ::crypto::CbcEncryptFileRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CbcDecryptFileResponse>* AsyncCbcDecryptFileRaw(::grpc::ClientContext* context, const ::crypto::CbcDecryptFileRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub GRPC_FINAL : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status CreateCbcKey(::grpc::ClientContext* context, const ::crypto::CreateCbcKeyRequest& request, ::crypto::CreateCbcKeyResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CreateCbcKeyResponse>> AsyncCreateCbcKey(::grpc::ClientContext* context, const ::crypto::CreateCbcKeyRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CreateCbcKeyResponse>>(AsyncCreateCbcKeyRaw(context, request, cq));
    }
    ::grpc::Status CbcEncryptFile(::grpc::ClientContext* context, const ::crypto::CbcEncryptFileRequest& request, ::crypto::CbcEncryptFileResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CbcEncryptFileResponse>> AsyncCbcEncryptFile(::grpc::ClientContext* context, const ::crypto::CbcEncryptFileRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CbcEncryptFileResponse>>(AsyncCbcEncryptFileRaw(context, request, cq));
    }
    ::grpc::Status CbcDecryptFile(::grpc::ClientContext* context, const ::crypto::CbcDecryptFileRequest& request, ::crypto::CbcDecryptFileResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CbcDecryptFileResponse>> AsyncCbcDecryptFile(::grpc::ClientContext* context, const ::crypto::CbcDecryptFileRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CbcDecryptFileResponse>>(AsyncCbcDecryptFileRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::crypto::CreateCbcKeyResponse>* AsyncCreateCbcKeyRaw(::grpc::ClientContext* context, const ::crypto::CreateCbcKeyRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::crypto::CbcEncryptFileResponse>* AsyncCbcEncryptFileRaw(::grpc::ClientContext* context, const ::crypto::CbcEncryptFileRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::crypto::CbcDecryptFileResponse>* AsyncCbcDecryptFileRaw(::grpc::ClientContext* context, const ::crypto::CbcDecryptFileRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    const ::grpc::RpcMethod rpcmethod_CreateCbcKey_;
    const ::grpc::RpcMethod rpcmethod_CbcEncryptFile_;
    const ::grpc::RpcMethod rpcmethod_CbcDecryptFile_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status CreateCbcKey(::grpc::ServerContext* context, const ::crypto::CreateCbcKeyRequest* request, ::crypto::CreateCbcKeyResponse* response);
    // TODO 
    // rpc CbcEncryptString(CbcEncryptStringRequest) returns (CbcEncryptStringResponse);
    virtual ::grpc::Status CbcEncryptFile(::grpc::ServerContext* context, const ::crypto::CbcEncryptFileRequest* request, ::crypto::CbcEncryptFileResponse* response);
    // TODO
    // rpc CbcDecryptString(CbcDecryptStringRequest) returns (CbcDecryptStringResponse);
    virtual ::grpc::Status CbcDecryptFile(::grpc::ServerContext* context, const ::crypto::CbcDecryptFileRequest* request, ::crypto::CbcDecryptFileResponse* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_CreateCbcKey : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_CreateCbcKey() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_CreateCbcKey() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CreateCbcKey(::grpc::ServerContext* context, const ::crypto::CreateCbcKeyRequest* request, ::crypto::CreateCbcKeyResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCreateCbcKey(::grpc::ServerContext* context, ::crypto::CreateCbcKeyRequest* request, ::grpc::ServerAsyncResponseWriter< ::crypto::CreateCbcKeyResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_CbcEncryptFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_CbcEncryptFile() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_CbcEncryptFile() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CbcEncryptFile(::grpc::ServerContext* context, const ::crypto::CbcEncryptFileRequest* request, ::crypto::CbcEncryptFileResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCbcEncryptFile(::grpc::ServerContext* context, ::crypto::CbcEncryptFileRequest* request, ::grpc::ServerAsyncResponseWriter< ::crypto::CbcEncryptFileResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_CbcDecryptFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_CbcDecryptFile() {
      ::grpc::Service::MarkMethodAsync(2);
    }
    ~WithAsyncMethod_CbcDecryptFile() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CbcDecryptFile(::grpc::ServerContext* context, const ::crypto::CbcDecryptFileRequest* request, ::crypto::CbcDecryptFileResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCbcDecryptFile(::grpc::ServerContext* context, ::crypto::CbcDecryptFileRequest* request, ::grpc::ServerAsyncResponseWriter< ::crypto::CbcDecryptFileResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_CreateCbcKey<WithAsyncMethod_CbcEncryptFile<WithAsyncMethod_CbcDecryptFile<Service > > > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_CreateCbcKey : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_CreateCbcKey() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_CreateCbcKey() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CreateCbcKey(::grpc::ServerContext* context, const ::crypto::CreateCbcKeyRequest* request, ::crypto::CreateCbcKeyResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_CbcEncryptFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_CbcEncryptFile() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_CbcEncryptFile() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CbcEncryptFile(::grpc::ServerContext* context, const ::crypto::CbcEncryptFileRequest* request, ::crypto::CbcEncryptFileResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_CbcDecryptFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_CbcDecryptFile() {
      ::grpc::Service::MarkMethodGeneric(2);
    }
    ~WithGenericMethod_CbcDecryptFile() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CbcDecryptFile(::grpc::ServerContext* context, const ::crypto::CbcDecryptFileRequest* request, ::crypto::CbcDecryptFileResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
};

class AsymmetricService GRPC_FINAL {
 public:
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status CreateKeyPair(::grpc::ClientContext* context, const ::crypto::CreateKeyPairRequest& request, ::crypto::CreateKeyPairResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CreateKeyPairResponse>> AsyncCreateKeyPair(::grpc::ClientContext* context, const ::crypto::CreateKeyPairRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CreateKeyPairResponse>>(AsyncCreateKeyPairRaw(context, request, cq));
    }
    virtual ::grpc::Status PublicKeyEncrypt(::grpc::ClientContext* context, const ::crypto::PublicKeyEncryptRequest& request, ::crypto::PublicKeyEncryptResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::PublicKeyEncryptResponse>> AsyncPublicKeyEncrypt(::grpc::ClientContext* context, const ::crypto::PublicKeyEncryptRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::PublicKeyEncryptResponse>>(AsyncPublicKeyEncryptRaw(context, request, cq));
    }
    virtual ::grpc::Status PrivateKeyDecrypt(::grpc::ClientContext* context, const ::crypto::PrivateKeyDecryptRequest& request, ::crypto::PrivateKeyDecryptResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::PrivateKeyDecryptResponse>> AsyncPrivateKeyDecrypt(::grpc::ClientContext* context, const ::crypto::PrivateKeyDecryptRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::crypto::PrivateKeyDecryptResponse>>(AsyncPrivateKeyDecryptRaw(context, request, cq));
    }
    // TODO(wqx):
    // signature & verify
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::crypto::CreateKeyPairResponse>* AsyncCreateKeyPairRaw(::grpc::ClientContext* context, const ::crypto::CreateKeyPairRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::crypto::PublicKeyEncryptResponse>* AsyncPublicKeyEncryptRaw(::grpc::ClientContext* context, const ::crypto::PublicKeyEncryptRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::crypto::PrivateKeyDecryptResponse>* AsyncPrivateKeyDecryptRaw(::grpc::ClientContext* context, const ::crypto::PrivateKeyDecryptRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub GRPC_FINAL : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status CreateKeyPair(::grpc::ClientContext* context, const ::crypto::CreateKeyPairRequest& request, ::crypto::CreateKeyPairResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CreateKeyPairResponse>> AsyncCreateKeyPair(::grpc::ClientContext* context, const ::crypto::CreateKeyPairRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::CreateKeyPairResponse>>(AsyncCreateKeyPairRaw(context, request, cq));
    }
    ::grpc::Status PublicKeyEncrypt(::grpc::ClientContext* context, const ::crypto::PublicKeyEncryptRequest& request, ::crypto::PublicKeyEncryptResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::PublicKeyEncryptResponse>> AsyncPublicKeyEncrypt(::grpc::ClientContext* context, const ::crypto::PublicKeyEncryptRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::PublicKeyEncryptResponse>>(AsyncPublicKeyEncryptRaw(context, request, cq));
    }
    ::grpc::Status PrivateKeyDecrypt(::grpc::ClientContext* context, const ::crypto::PrivateKeyDecryptRequest& request, ::crypto::PrivateKeyDecryptResponse* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::PrivateKeyDecryptResponse>> AsyncPrivateKeyDecrypt(::grpc::ClientContext* context, const ::crypto::PrivateKeyDecryptRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::crypto::PrivateKeyDecryptResponse>>(AsyncPrivateKeyDecryptRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::crypto::CreateKeyPairResponse>* AsyncCreateKeyPairRaw(::grpc::ClientContext* context, const ::crypto::CreateKeyPairRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::crypto::PublicKeyEncryptResponse>* AsyncPublicKeyEncryptRaw(::grpc::ClientContext* context, const ::crypto::PublicKeyEncryptRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::crypto::PrivateKeyDecryptResponse>* AsyncPrivateKeyDecryptRaw(::grpc::ClientContext* context, const ::crypto::PrivateKeyDecryptRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    const ::grpc::RpcMethod rpcmethod_CreateKeyPair_;
    const ::grpc::RpcMethod rpcmethod_PublicKeyEncrypt_;
    const ::grpc::RpcMethod rpcmethod_PrivateKeyDecrypt_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status CreateKeyPair(::grpc::ServerContext* context, const ::crypto::CreateKeyPairRequest* request, ::crypto::CreateKeyPairResponse* response);
    virtual ::grpc::Status PublicKeyEncrypt(::grpc::ServerContext* context, const ::crypto::PublicKeyEncryptRequest* request, ::crypto::PublicKeyEncryptResponse* response);
    virtual ::grpc::Status PrivateKeyDecrypt(::grpc::ServerContext* context, const ::crypto::PrivateKeyDecryptRequest* request, ::crypto::PrivateKeyDecryptResponse* response);
    // TODO(wqx):
    // signature & verify
  };
  template <class BaseClass>
  class WithAsyncMethod_CreateKeyPair : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_CreateKeyPair() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_CreateKeyPair() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CreateKeyPair(::grpc::ServerContext* context, const ::crypto::CreateKeyPairRequest* request, ::crypto::CreateKeyPairResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCreateKeyPair(::grpc::ServerContext* context, ::crypto::CreateKeyPairRequest* request, ::grpc::ServerAsyncResponseWriter< ::crypto::CreateKeyPairResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_PublicKeyEncrypt : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_PublicKeyEncrypt() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_PublicKeyEncrypt() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status PublicKeyEncrypt(::grpc::ServerContext* context, const ::crypto::PublicKeyEncryptRequest* request, ::crypto::PublicKeyEncryptResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestPublicKeyEncrypt(::grpc::ServerContext* context, ::crypto::PublicKeyEncryptRequest* request, ::grpc::ServerAsyncResponseWriter< ::crypto::PublicKeyEncryptResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_PrivateKeyDecrypt : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_PrivateKeyDecrypt() {
      ::grpc::Service::MarkMethodAsync(2);
    }
    ~WithAsyncMethod_PrivateKeyDecrypt() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status PrivateKeyDecrypt(::grpc::ServerContext* context, const ::crypto::PrivateKeyDecryptRequest* request, ::crypto::PrivateKeyDecryptResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestPrivateKeyDecrypt(::grpc::ServerContext* context, ::crypto::PrivateKeyDecryptRequest* request, ::grpc::ServerAsyncResponseWriter< ::crypto::PrivateKeyDecryptResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_CreateKeyPair<WithAsyncMethod_PublicKeyEncrypt<WithAsyncMethod_PrivateKeyDecrypt<Service > > > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_CreateKeyPair : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_CreateKeyPair() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_CreateKeyPair() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CreateKeyPair(::grpc::ServerContext* context, const ::crypto::CreateKeyPairRequest* request, ::crypto::CreateKeyPairResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_PublicKeyEncrypt : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_PublicKeyEncrypt() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_PublicKeyEncrypt() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status PublicKeyEncrypt(::grpc::ServerContext* context, const ::crypto::PublicKeyEncryptRequest* request, ::crypto::PublicKeyEncryptResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_PrivateKeyDecrypt : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_PrivateKeyDecrypt() {
      ::grpc::Service::MarkMethodGeneric(2);
    }
    ~WithGenericMethod_PrivateKeyDecrypt() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status PrivateKeyDecrypt(::grpc::ServerContext* context, const ::crypto::PrivateKeyDecryptRequest* request, ::crypto::PrivateKeyDecryptResponse* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
};

}  // namespace crypto


#endif  // GRPC_crypto_5fserver_2eproto__INCLUDED
