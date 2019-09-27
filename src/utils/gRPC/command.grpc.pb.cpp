#include "command.pb.h"
#include "command.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace v2ray
{
    namespace core
    {
        namespace app
        {
            namespace stats
            {
                namespace command
                {

                    static const char *StatsService_method_names[] = {
                        "/v2ray.core.app.stats.command.StatsService/GetStats",
                        "/v2ray.core.app.stats.command.StatsService/QueryStats",
                        "/v2ray.core.app.stats.command.StatsService/GetSysStats",
                    };

                    std::unique_ptr< StatsService::Stub> StatsService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface> &channel, const ::grpc::StubOptions &options)
                    {
                        (void)options;
                        std::unique_ptr< StatsService::Stub> stub(new StatsService::Stub(channel));
                        return stub;
                    }

                    StatsService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface> &channel)
                        : channel_(channel), rpcmethod_GetStats_(StatsService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
                        , rpcmethod_QueryStats_(StatsService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
                        , rpcmethod_GetSysStats_(StatsService_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
                    {}

                    ::grpc::Status StatsService::Stub::GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::v2ray::core::app::stats::command::GetStatsResponse *response)
                    {
                        return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetStats_, context, request, response);
                    }

                    void StatsService::Stub::experimental_async::GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, std::function<void(::grpc::Status)> f)
                    {
                        ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetStats_, context, request, response, std::move(f));
                    }

                    void StatsService::Stub::experimental_async::GetStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, std::function<void(::grpc::Status)> f)
                    {
                        ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetStats_, context, request, response, std::move(f));
                    }

                    void StatsService::Stub::experimental_async::GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor)
                    {
                        ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetStats_, context, request, response, reactor);
                    }

                    void StatsService::Stub::experimental_async::GetStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor)
                    {
                        ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetStats_, context, request, response, reactor);
                    }

                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse> *StatsService::Stub::AsyncGetStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq)
                    {
                        return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::v2ray::core::app::stats::command::GetStatsResponse>::Create(channel_.get(), cq, rpcmethod_GetStats_, context, request, true);
                    }

                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse> *StatsService::Stub::PrepareAsyncGetStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq)
                    {
                        return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::v2ray::core::app::stats::command::GetStatsResponse>::Create(channel_.get(), cq, rpcmethod_GetStats_, context, request, false);
                    }

                    ::grpc::Status StatsService::Stub::QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::v2ray::core::app::stats::command::QueryStatsResponse *response)
                    {
                        return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_QueryStats_, context, request, response);
                    }

                    void StatsService::Stub::experimental_async::QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, std::function<void(::grpc::Status)> f)
                    {
                        ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_QueryStats_, context, request, response, std::move(f));
                    }

                    void StatsService::Stub::experimental_async::QueryStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, std::function<void(::grpc::Status)> f)
                    {
                        ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_QueryStats_, context, request, response, std::move(f));
                    }

                    void StatsService::Stub::experimental_async::QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor)
                    {
                        ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_QueryStats_, context, request, response, reactor);
                    }

                    void StatsService::Stub::experimental_async::QueryStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor)
                    {
                        ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_QueryStats_, context, request, response, reactor);
                    }

                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse> *StatsService::Stub::AsyncQueryStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq)
                    {
                        return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::v2ray::core::app::stats::command::QueryStatsResponse>::Create(channel_.get(), cq, rpcmethod_QueryStats_, context, request, true);
                    }

                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse> *StatsService::Stub::PrepareAsyncQueryStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq)
                    {
                        return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::v2ray::core::app::stats::command::QueryStatsResponse>::Create(channel_.get(), cq, rpcmethod_QueryStats_, context, request, false);
                    }

                    ::grpc::Status StatsService::Stub::GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::v2ray::core::app::stats::command::SysStatsResponse *response)
                    {
                        return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetSysStats_, context, request, response);
                    }

                    void StatsService::Stub::experimental_async::GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, std::function<void(::grpc::Status)> f)
                    {
                        ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetSysStats_, context, request, response, std::move(f));
                    }

                    void StatsService::Stub::experimental_async::GetSysStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, std::function<void(::grpc::Status)> f)
                    {
                        ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetSysStats_, context, request, response, std::move(f));
                    }

                    void StatsService::Stub::experimental_async::GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor)
                    {
                        ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetSysStats_, context, request, response, reactor);
                    }

                    void StatsService::Stub::experimental_async::GetSysStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor)
                    {
                        ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetSysStats_, context, request, response, reactor);
                    }

                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse> *StatsService::Stub::AsyncGetSysStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq)
                    {
                        return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::v2ray::core::app::stats::command::SysStatsResponse>::Create(channel_.get(), cq, rpcmethod_GetSysStats_, context, request, true);
                    }

                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse> *StatsService::Stub::PrepareAsyncGetSysStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq)
                    {
                        return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::v2ray::core::app::stats::command::SysStatsResponse>::Create(channel_.get(), cq, rpcmethod_GetSysStats_, context, request, false);
                    }

                    StatsService::Service::Service()
                    {
                        AddMethod(new ::grpc::internal::RpcServiceMethod(
                                      StatsService_method_names[0],
                                      ::grpc::internal::RpcMethod::NORMAL_RPC,
                                      new ::grpc::internal::RpcMethodHandler< StatsService::Service, ::v2ray::core::app::stats::command::GetStatsRequest, ::v2ray::core::app::stats::command::GetStatsResponse>(
                                          std::mem_fn(&StatsService::Service::GetStats), this)));
                        AddMethod(new ::grpc::internal::RpcServiceMethod(
                                      StatsService_method_names[1],
                                      ::grpc::internal::RpcMethod::NORMAL_RPC,
                                      new ::grpc::internal::RpcMethodHandler< StatsService::Service, ::v2ray::core::app::stats::command::QueryStatsRequest, ::v2ray::core::app::stats::command::QueryStatsResponse>(
                                          std::mem_fn(&StatsService::Service::QueryStats), this)));
                        AddMethod(new ::grpc::internal::RpcServiceMethod(
                                      StatsService_method_names[2],
                                      ::grpc::internal::RpcMethod::NORMAL_RPC,
                                      new ::grpc::internal::RpcMethodHandler< StatsService::Service, ::v2ray::core::app::stats::command::SysStatsRequest, ::v2ray::core::app::stats::command::SysStatsResponse>(
                                          std::mem_fn(&StatsService::Service::GetSysStats), this)));
                    }

                    StatsService::Service::~Service()
                    {
                    }

                    ::grpc::Status StatsService::Service::GetStats(::grpc::ServerContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response)
                    {
                        (void) context;
                        (void) request;
                        (void) response;
                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                    }

                    ::grpc::Status StatsService::Service::QueryStats(::grpc::ServerContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response)
                    {
                        (void) context;
                        (void) request;
                        (void) response;
                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                    }

                    ::grpc::Status StatsService::Service::GetSysStats(::grpc::ServerContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response)
                    {
                        (void) context;
                        (void) request;
                        (void) response;
                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                    }


                }  // namespace v2ray
            }  // namespace core
        }  // namespace app
    }  // namespace stats
}  // namespace command

