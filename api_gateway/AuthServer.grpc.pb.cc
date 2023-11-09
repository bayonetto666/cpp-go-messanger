// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: AuthServer.proto

#include "AuthServer.pb.h"
#include "AuthServer.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>

static const char* Authentication_method_names[] = {
  "/Authentication/authUser",
  "/Authentication/registerUser",
  "/Authentication/getSubject",
  "/Authentication/verifyJWT",
};

std::unique_ptr< Authentication::Stub> Authentication::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Authentication::Stub> stub(new Authentication::Stub(channel, options));
  return stub;
}

Authentication::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_authUser_(Authentication_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_registerUser_(Authentication_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_getSubject_(Authentication_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_verifyJWT_(Authentication_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Authentication::Stub::authUser(::grpc::ClientContext* context, const ::authUserRequest& request, ::authUserResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::authUserRequest, ::authUserResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_authUser_, context, request, response);
}

void Authentication::Stub::async::authUser(::grpc::ClientContext* context, const ::authUserRequest* request, ::authUserResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::authUserRequest, ::authUserResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_authUser_, context, request, response, std::move(f));
}

void Authentication::Stub::async::authUser(::grpc::ClientContext* context, const ::authUserRequest* request, ::authUserResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_authUser_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::authUserResponse>* Authentication::Stub::PrepareAsyncauthUserRaw(::grpc::ClientContext* context, const ::authUserRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::authUserResponse, ::authUserRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_authUser_, context, request);
}

::grpc::ClientAsyncResponseReader< ::authUserResponse>* Authentication::Stub::AsyncauthUserRaw(::grpc::ClientContext* context, const ::authUserRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncauthUserRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Authentication::Stub::registerUser(::grpc::ClientContext* context, const ::registerUserRequest& request, ::registerUserResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::registerUserRequest, ::registerUserResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_registerUser_, context, request, response);
}

void Authentication::Stub::async::registerUser(::grpc::ClientContext* context, const ::registerUserRequest* request, ::registerUserResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::registerUserRequest, ::registerUserResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_registerUser_, context, request, response, std::move(f));
}

void Authentication::Stub::async::registerUser(::grpc::ClientContext* context, const ::registerUserRequest* request, ::registerUserResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_registerUser_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::registerUserResponse>* Authentication::Stub::PrepareAsyncregisterUserRaw(::grpc::ClientContext* context, const ::registerUserRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::registerUserResponse, ::registerUserRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_registerUser_, context, request);
}

::grpc::ClientAsyncResponseReader< ::registerUserResponse>* Authentication::Stub::AsyncregisterUserRaw(::grpc::ClientContext* context, const ::registerUserRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncregisterUserRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Authentication::Stub::getSubject(::grpc::ClientContext* context, const ::getSubjectRequest& request, ::getSubjectResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::getSubjectRequest, ::getSubjectResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_getSubject_, context, request, response);
}

void Authentication::Stub::async::getSubject(::grpc::ClientContext* context, const ::getSubjectRequest* request, ::getSubjectResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::getSubjectRequest, ::getSubjectResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_getSubject_, context, request, response, std::move(f));
}

void Authentication::Stub::async::getSubject(::grpc::ClientContext* context, const ::getSubjectRequest* request, ::getSubjectResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_getSubject_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::getSubjectResponse>* Authentication::Stub::PrepareAsyncgetSubjectRaw(::grpc::ClientContext* context, const ::getSubjectRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::getSubjectResponse, ::getSubjectRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_getSubject_, context, request);
}

::grpc::ClientAsyncResponseReader< ::getSubjectResponse>* Authentication::Stub::AsyncgetSubjectRaw(::grpc::ClientContext* context, const ::getSubjectRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncgetSubjectRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Authentication::Stub::verifyJWT(::grpc::ClientContext* context, const ::verifyJWTRequest& request, ::verifyJWTResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::verifyJWTRequest, ::verifyJWTResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_verifyJWT_, context, request, response);
}

void Authentication::Stub::async::verifyJWT(::grpc::ClientContext* context, const ::verifyJWTRequest* request, ::verifyJWTResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::verifyJWTRequest, ::verifyJWTResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_verifyJWT_, context, request, response, std::move(f));
}

void Authentication::Stub::async::verifyJWT(::grpc::ClientContext* context, const ::verifyJWTRequest* request, ::verifyJWTResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_verifyJWT_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::verifyJWTResponse>* Authentication::Stub::PrepareAsyncverifyJWTRaw(::grpc::ClientContext* context, const ::verifyJWTRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::verifyJWTResponse, ::verifyJWTRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_verifyJWT_, context, request);
}

::grpc::ClientAsyncResponseReader< ::verifyJWTResponse>* Authentication::Stub::AsyncverifyJWTRaw(::grpc::ClientContext* context, const ::verifyJWTRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncverifyJWTRaw(context, request, cq);
  result->StartCall();
  return result;
}

Authentication::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Authentication_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Authentication::Service, ::authUserRequest, ::authUserResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Authentication::Service* service,
             ::grpc::ServerContext* ctx,
             const ::authUserRequest* req,
             ::authUserResponse* resp) {
               return service->authUser(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Authentication_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Authentication::Service, ::registerUserRequest, ::registerUserResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Authentication::Service* service,
             ::grpc::ServerContext* ctx,
             const ::registerUserRequest* req,
             ::registerUserResponse* resp) {
               return service->registerUser(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Authentication_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Authentication::Service, ::getSubjectRequest, ::getSubjectResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Authentication::Service* service,
             ::grpc::ServerContext* ctx,
             const ::getSubjectRequest* req,
             ::getSubjectResponse* resp) {
               return service->getSubject(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Authentication_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Authentication::Service, ::verifyJWTRequest, ::verifyJWTResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Authentication::Service* service,
             ::grpc::ServerContext* ctx,
             const ::verifyJWTRequest* req,
             ::verifyJWTResponse* resp) {
               return service->verifyJWT(ctx, req, resp);
             }, this)));
}

Authentication::Service::~Service() {
}

::grpc::Status Authentication::Service::authUser(::grpc::ServerContext* context, const ::authUserRequest* request, ::authUserResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Authentication::Service::registerUser(::grpc::ServerContext* context, const ::registerUserRequest* request, ::registerUserResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Authentication::Service::getSubject(::grpc::ServerContext* context, const ::getSubjectRequest* request, ::getSubjectResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Authentication::Service::verifyJWT(::grpc::ServerContext* context, const ::verifyJWTRequest* request, ::verifyJWTResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


