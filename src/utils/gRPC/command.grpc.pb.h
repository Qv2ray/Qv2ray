#ifndef GRPC_command_2eproto__INCLUDED
#define GRPC_command_2eproto__INCLUDED

#include "command.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc_impl
{
    class CompletionQueue;
    class ServerCompletionQueue;
    class ServerContext;
}  // namespace grpc_impl

namespace grpc
{
    namespace experimental
    {
        template <typename RequestT, typename ResponseT>
        class MessageAllocator;
    }  // namespace experimental
}  // namespace grpc

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

                    class StatsService final
                    {
                        public:
                            static constexpr char const *service_full_name()
                            {
                                return "v2ray.core.app.stats.command.StatsService";
                            }
                            class StubInterface
                            {
                                public:
                                    virtual ~StubInterface() {}
                                    virtual ::grpc::Status GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::v2ray::core::app::stats::command::GetStatsResponse *response) = 0;
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::GetStatsResponse>> AsyncGetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::GetStatsResponse>>(AsyncGetStatsRaw(context, request, cq));
                                    }
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::GetStatsResponse>> PrepareAsyncGetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::GetStatsResponse>>(PrepareAsyncGetStatsRaw(context, request, cq));
                                    }
                                    virtual ::grpc::Status QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::v2ray::core::app::stats::command::QueryStatsResponse *response) = 0;
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::QueryStatsResponse>> AsyncQueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::QueryStatsResponse>>(AsyncQueryStatsRaw(context, request, cq));
                                    }
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::QueryStatsResponse>> PrepareAsyncQueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::QueryStatsResponse>>(PrepareAsyncQueryStatsRaw(context, request, cq));
                                    }
                                    virtual ::grpc::Status GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::v2ray::core::app::stats::command::SysStatsResponse *response) = 0;
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::SysStatsResponse>> AsyncGetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::SysStatsResponse>>(AsyncGetSysStatsRaw(context, request, cq));
                                    }
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::SysStatsResponse>> PrepareAsyncGetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::SysStatsResponse>>(PrepareAsyncGetSysStatsRaw(context, request, cq));
                                    }
                                    class experimental_async_interface
                                    {
                                        public:
                                            virtual ~experimental_async_interface() {}
                                            virtual void GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, std::function<void(::grpc::Status)>) = 0;
                                            virtual void GetStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, std::function<void(::grpc::Status)>) = 0;
                                            virtual void GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) = 0;
                                            virtual void GetStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) = 0;
                                            virtual void QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, std::function<void(::grpc::Status)>) = 0;
                                            virtual void QueryStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, std::function<void(::grpc::Status)>) = 0;
                                            virtual void QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) = 0;
                                            virtual void QueryStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) = 0;
                                            virtual void GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, std::function<void(::grpc::Status)>) = 0;
                                            virtual void GetSysStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, std::function<void(::grpc::Status)>) = 0;
                                            virtual void GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) = 0;
                                            virtual void GetSysStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) = 0;
                                    };
                                    virtual class experimental_async_interface *experimental_async()
                                    {
                                            return nullptr;
                                    }
                                private:
                                    virtual ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::GetStatsResponse> *AsyncGetStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq) = 0;
                                    virtual ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::GetStatsResponse> *PrepareAsyncGetStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq) = 0;
                                    virtual ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::QueryStatsResponse> *AsyncQueryStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq) = 0;
                                    virtual ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::QueryStatsResponse> *PrepareAsyncQueryStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq) = 0;
                                    virtual ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::SysStatsResponse> *AsyncGetSysStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq) = 0;
                                    virtual ::grpc::ClientAsyncResponseReaderInterface< ::v2ray::core::app::stats::command::SysStatsResponse> *PrepareAsyncGetSysStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq) = 0;
                            };
                            class Stub final : public StubInterface
                            {
                                public:
                                    Stub(const std::shared_ptr< ::grpc::ChannelInterface> &channel);
                                    ::grpc::Status GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::v2ray::core::app::stats::command::GetStatsResponse *response) override;
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse>> AsyncGetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse>>(AsyncGetStatsRaw(context, request, cq));
                                    }
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse>> PrepareAsyncGetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse>>(PrepareAsyncGetStatsRaw(context, request, cq));
                                    }
                                    ::grpc::Status QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::v2ray::core::app::stats::command::QueryStatsResponse *response) override;
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse>> AsyncQueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse>>(AsyncQueryStatsRaw(context, request, cq));
                                    }
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse>> PrepareAsyncQueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse>>(PrepareAsyncQueryStatsRaw(context, request, cq));
                                    }
                                    ::grpc::Status GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::v2ray::core::app::stats::command::SysStatsResponse *response) override;
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse>> AsyncGetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse>>(AsyncGetSysStatsRaw(context, request, cq));
                                    }
                                    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse>> PrepareAsyncGetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq)
                                    {
                                        return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse>>(PrepareAsyncGetSysStatsRaw(context, request, cq));
                                    }
                                    class experimental_async final :
                                        public StubInterface::experimental_async_interface
                                    {
                                        public:
                                            void GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, std::function<void(::grpc::Status)>) override;
                                            void GetStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, std::function<void(::grpc::Status)>) override;
                                            void GetStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) override;
                                            void GetStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::GetStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) override;
                                            void QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, std::function<void(::grpc::Status)>) override;
                                            void QueryStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, std::function<void(::grpc::Status)>) override;
                                            void QueryStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) override;
                                            void QueryStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) override;
                                            void GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, std::function<void(::grpc::Status)>) override;
                                            void GetSysStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, std::function<void(::grpc::Status)>) override;
                                            void GetSysStats(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) override;
                                            void GetSysStats(::grpc::ClientContext *context, const ::grpc::ByteBuffer *request, ::v2ray::core::app::stats::command::SysStatsResponse *response, ::grpc::experimental::ClientUnaryReactor *reactor) override;
                                        private:
                                            friend class Stub;
                                            explicit experimental_async(Stub *stub): stub_(stub) { }
                                            Stub *stub()
                                            {
                                                return stub_;
                                            }
                                            Stub *stub_;
                                    };
                                    class experimental_async_interface *experimental_async() override
                                    {
                                            return &async_stub_;
                                    }

                                private:
                                    std::shared_ptr< ::grpc::ChannelInterface> channel_;
                                    class experimental_async async_stub_
                                    {
                                            this
                                    };
                                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse> *AsyncGetStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq) override;
                                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::GetStatsResponse> *PrepareAsyncGetStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest &request, ::grpc::CompletionQueue *cq) override;
                                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse> *AsyncQueryStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq) override;
                                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::QueryStatsResponse> *PrepareAsyncQueryStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest &request, ::grpc::CompletionQueue *cq) override;
                                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse> *AsyncGetSysStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq) override;
                                    ::grpc::ClientAsyncResponseReader< ::v2ray::core::app::stats::command::SysStatsResponse> *PrepareAsyncGetSysStatsRaw(::grpc::ClientContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest &request, ::grpc::CompletionQueue *cq) override;
                                    const ::grpc::internal::RpcMethod rpcmethod_GetStats_;
                                    const ::grpc::internal::RpcMethod rpcmethod_QueryStats_;
                                    const ::grpc::internal::RpcMethod rpcmethod_GetSysStats_;
                            };
                            static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface> &channel, const ::grpc::StubOptions &options = ::grpc::StubOptions());

                            class Service : public ::grpc::Service
                            {
                                public:
                                    Service();
                                    virtual ~Service();
                                    virtual ::grpc::Status GetStats(::grpc::ServerContext *context, const ::v2ray::core::app::stats::command::GetStatsRequest *request, ::v2ray::core::app::stats::command::GetStatsResponse *response);
                                    virtual ::grpc::Status QueryStats(::grpc::ServerContext *context, const ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::v2ray::core::app::stats::command::QueryStatsResponse *response);
                                    virtual ::grpc::Status GetSysStats(::grpc::ServerContext *context, const ::v2ray::core::app::stats::command::SysStatsRequest *request, ::v2ray::core::app::stats::command::SysStatsResponse *response);
                            };
                            template <class BaseClass>
                            class WithAsyncMethod_GetStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithAsyncMethod_GetStats()
                                    {
                                        ::grpc::Service::MarkMethodAsync(0);
                                    }
                                    ~WithAsyncMethod_GetStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::GetStatsRequest * /*request*/, ::v2ray::core::app::stats::command::GetStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    void RequestGetStats(::grpc::ServerContext *context, ::v2ray::core::app::stats::command::GetStatsRequest *request, ::grpc::ServerAsyncResponseWriter< ::v2ray::core::app::stats::command::GetStatsResponse> *response, ::grpc::CompletionQueue *new_call_cq, ::grpc::ServerCompletionQueue *notification_cq, void *tag)
                                    {
                                        ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
                                    }
                            };
                            template <class BaseClass>
                            class WithAsyncMethod_QueryStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithAsyncMethod_QueryStats()
                                    {
                                        ::grpc::Service::MarkMethodAsync(1);
                                    }
                                    ~WithAsyncMethod_QueryStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status QueryStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::QueryStatsRequest * /*request*/, ::v2ray::core::app::stats::command::QueryStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    void RequestQueryStats(::grpc::ServerContext *context, ::v2ray::core::app::stats::command::QueryStatsRequest *request, ::grpc::ServerAsyncResponseWriter< ::v2ray::core::app::stats::command::QueryStatsResponse> *response, ::grpc::CompletionQueue *new_call_cq, ::grpc::ServerCompletionQueue *notification_cq, void *tag)
                                    {
                                        ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
                                    }
                            };
                            template <class BaseClass>
                            class WithAsyncMethod_GetSysStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithAsyncMethod_GetSysStats()
                                    {
                                        ::grpc::Service::MarkMethodAsync(2);
                                    }
                                    ~WithAsyncMethod_GetSysStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetSysStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::SysStatsRequest * /*request*/, ::v2ray::core::app::stats::command::SysStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    void RequestGetSysStats(::grpc::ServerContext *context, ::v2ray::core::app::stats::command::SysStatsRequest *request, ::grpc::ServerAsyncResponseWriter< ::v2ray::core::app::stats::command::SysStatsResponse> *response, ::grpc::CompletionQueue *new_call_cq, ::grpc::ServerCompletionQueue *notification_cq, void *tag)
                                    {
                                        ::grpc::Service::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
                                    }
                            };
                            typedef WithAsyncMethod_GetStats<WithAsyncMethod_QueryStats<WithAsyncMethod_GetSysStats<Service > > > AsyncService;
                            template <class BaseClass>
                            class ExperimentalWithCallbackMethod_GetStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    ExperimentalWithCallbackMethod_GetStats()
                                    {
                                        ::grpc::Service::experimental().MarkMethodCallback(0,
                                                new ::grpc_impl::internal::CallbackUnaryHandler< ::v2ray::core::app::stats::command::GetStatsRequest, ::v2ray::core::app::stats::command::GetStatsResponse>(
                                                    [this](::grpc::ServerContext * context,
                                                           const ::v2ray::core::app::stats::command::GetStatsRequest * request,
                                                           ::v2ray::core::app::stats::command::GetStatsResponse * response,
                                        ::grpc::experimental::ServerCallbackRpcController * controller) {
                                            return this->GetStats(context, request, response, controller);
                                        }));
                                    }
                                    void SetMessageAllocatorFor_GetStats(
                                        ::grpc::experimental::MessageAllocator< ::v2ray::core::app::stats::command::GetStatsRequest, ::v2ray::core::app::stats::command::GetStatsResponse> *allocator)
                                    {
                                        static_cast<::grpc_impl::internal::CallbackUnaryHandler< ::v2ray::core::app::stats::command::GetStatsRequest, ::v2ray::core::app::stats::command::GetStatsResponse>*>(
                                            ::grpc::Service::experimental().GetHandler(0))
                                        ->SetMessageAllocator(allocator);
                                    }
                                    ~ExperimentalWithCallbackMethod_GetStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::GetStatsRequest * /*request*/, ::v2ray::core::app::stats::command::GetStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    virtual void GetStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::GetStatsRequest * /*request*/, ::v2ray::core::app::stats::command::GetStatsResponse * /*response*/, ::grpc::experimental::ServerCallbackRpcController *controller)
                                    {
                                        controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, ""));
                                    }
                            };
                            template <class BaseClass>
                            class ExperimentalWithCallbackMethod_QueryStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    ExperimentalWithCallbackMethod_QueryStats()
                                    {
                                        ::grpc::Service::experimental().MarkMethodCallback(1,
                                                new ::grpc_impl::internal::CallbackUnaryHandler< ::v2ray::core::app::stats::command::QueryStatsRequest, ::v2ray::core::app::stats::command::QueryStatsResponse>(
                                                    [this](::grpc::ServerContext * context,
                                                           const ::v2ray::core::app::stats::command::QueryStatsRequest * request,
                                                           ::v2ray::core::app::stats::command::QueryStatsResponse * response,
                                        ::grpc::experimental::ServerCallbackRpcController * controller) {
                                            return this->QueryStats(context, request, response, controller);
                                        }));
                                    }
                                    void SetMessageAllocatorFor_QueryStats(
                                        ::grpc::experimental::MessageAllocator< ::v2ray::core::app::stats::command::QueryStatsRequest, ::v2ray::core::app::stats::command::QueryStatsResponse> *allocator)
                                    {
                                        static_cast<::grpc_impl::internal::CallbackUnaryHandler< ::v2ray::core::app::stats::command::QueryStatsRequest, ::v2ray::core::app::stats::command::QueryStatsResponse>*>(
                                            ::grpc::Service::experimental().GetHandler(1))
                                        ->SetMessageAllocator(allocator);
                                    }
                                    ~ExperimentalWithCallbackMethod_QueryStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status QueryStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::QueryStatsRequest * /*request*/, ::v2ray::core::app::stats::command::QueryStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    virtual void QueryStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::QueryStatsRequest * /*request*/, ::v2ray::core::app::stats::command::QueryStatsResponse * /*response*/, ::grpc::experimental::ServerCallbackRpcController *controller)
                                    {
                                        controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, ""));
                                    }
                            };
                            template <class BaseClass>
                            class ExperimentalWithCallbackMethod_GetSysStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    ExperimentalWithCallbackMethod_GetSysStats()
                                    {
                                        ::grpc::Service::experimental().MarkMethodCallback(2,
                                                new ::grpc_impl::internal::CallbackUnaryHandler< ::v2ray::core::app::stats::command::SysStatsRequest, ::v2ray::core::app::stats::command::SysStatsResponse>(
                                                    [this](::grpc::ServerContext * context,
                                                           const ::v2ray::core::app::stats::command::SysStatsRequest * request,
                                                           ::v2ray::core::app::stats::command::SysStatsResponse * response,
                                        ::grpc::experimental::ServerCallbackRpcController * controller) {
                                            return this->GetSysStats(context, request, response, controller);
                                        }));
                                    }
                                    void SetMessageAllocatorFor_GetSysStats(
                                        ::grpc::experimental::MessageAllocator< ::v2ray::core::app::stats::command::SysStatsRequest, ::v2ray::core::app::stats::command::SysStatsResponse> *allocator)
                                    {
                                        static_cast<::grpc_impl::internal::CallbackUnaryHandler< ::v2ray::core::app::stats::command::SysStatsRequest, ::v2ray::core::app::stats::command::SysStatsResponse>*>(
                                            ::grpc::Service::experimental().GetHandler(2))
                                        ->SetMessageAllocator(allocator);
                                    }
                                    ~ExperimentalWithCallbackMethod_GetSysStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetSysStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::SysStatsRequest * /*request*/, ::v2ray::core::app::stats::command::SysStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    virtual void GetSysStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::SysStatsRequest * /*request*/, ::v2ray::core::app::stats::command::SysStatsResponse * /*response*/, ::grpc::experimental::ServerCallbackRpcController *controller)
                                    {
                                        controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, ""));
                                    }
                            };
                            typedef ExperimentalWithCallbackMethod_GetStats<ExperimentalWithCallbackMethod_QueryStats<ExperimentalWithCallbackMethod_GetSysStats<Service > > > ExperimentalCallbackService;
                            template <class BaseClass>
                            class WithGenericMethod_GetStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithGenericMethod_GetStats()
                                    {
                                        ::grpc::Service::MarkMethodGeneric(0);
                                    }
                                    ~WithGenericMethod_GetStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::GetStatsRequest * /*request*/, ::v2ray::core::app::stats::command::GetStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                            };
                            template <class BaseClass>
                            class WithGenericMethod_QueryStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithGenericMethod_QueryStats()
                                    {
                                        ::grpc::Service::MarkMethodGeneric(1);
                                    }
                                    ~WithGenericMethod_QueryStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status QueryStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::QueryStatsRequest * /*request*/, ::v2ray::core::app::stats::command::QueryStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                            };
                            template <class BaseClass>
                            class WithGenericMethod_GetSysStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithGenericMethod_GetSysStats()
                                    {
                                        ::grpc::Service::MarkMethodGeneric(2);
                                    }
                                    ~WithGenericMethod_GetSysStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetSysStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::SysStatsRequest * /*request*/, ::v2ray::core::app::stats::command::SysStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                            };
                            template <class BaseClass>
                            class WithRawMethod_GetStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithRawMethod_GetStats()
                                    {
                                        ::grpc::Service::MarkMethodRaw(0);
                                    }
                                    ~WithRawMethod_GetStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::GetStatsRequest * /*request*/, ::v2ray::core::app::stats::command::GetStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    void RequestGetStats(::grpc::ServerContext *context, ::grpc::ByteBuffer *request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer> *response, ::grpc::CompletionQueue *new_call_cq, ::grpc::ServerCompletionQueue *notification_cq, void *tag)
                                    {
                                        ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
                                    }
                            };
                            template <class BaseClass>
                            class WithRawMethod_QueryStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithRawMethod_QueryStats()
                                    {
                                        ::grpc::Service::MarkMethodRaw(1);
                                    }
                                    ~WithRawMethod_QueryStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status QueryStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::QueryStatsRequest * /*request*/, ::v2ray::core::app::stats::command::QueryStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    void RequestQueryStats(::grpc::ServerContext *context, ::grpc::ByteBuffer *request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer> *response, ::grpc::CompletionQueue *new_call_cq, ::grpc::ServerCompletionQueue *notification_cq, void *tag)
                                    {
                                        ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
                                    }
                            };
                            template <class BaseClass>
                            class WithRawMethod_GetSysStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithRawMethod_GetSysStats()
                                    {
                                        ::grpc::Service::MarkMethodRaw(2);
                                    }
                                    ~WithRawMethod_GetSysStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetSysStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::SysStatsRequest * /*request*/, ::v2ray::core::app::stats::command::SysStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    void RequestGetSysStats(::grpc::ServerContext *context, ::grpc::ByteBuffer *request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer> *response, ::grpc::CompletionQueue *new_call_cq, ::grpc::ServerCompletionQueue *notification_cq, void *tag)
                                    {
                                        ::grpc::Service::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
                                    }
                            };
                            template <class BaseClass>
                            class ExperimentalWithRawCallbackMethod_GetStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    ExperimentalWithRawCallbackMethod_GetStats()
                                    {
                                        ::grpc::Service::experimental().MarkMethodRawCallback(0,
                                                new ::grpc_impl::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
                                                    [this](::grpc::ServerContext * context,
                                                           const ::grpc::ByteBuffer * request,
                                                           ::grpc::ByteBuffer * response,
                                        ::grpc::experimental::ServerCallbackRpcController * controller) {
                                            this->GetStats(context, request, response, controller);
                                        }));
                                    }
                                    ~ExperimentalWithRawCallbackMethod_GetStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::GetStatsRequest * /*request*/, ::v2ray::core::app::stats::command::GetStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    virtual void GetStats(::grpc::ServerContext * /*context*/, const ::grpc::ByteBuffer * /*request*/, ::grpc::ByteBuffer * /*response*/, ::grpc::experimental::ServerCallbackRpcController *controller)
                                    {
                                        controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, ""));
                                    }
                            };
                            template <class BaseClass>
                            class ExperimentalWithRawCallbackMethod_QueryStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    ExperimentalWithRawCallbackMethod_QueryStats()
                                    {
                                        ::grpc::Service::experimental().MarkMethodRawCallback(1,
                                                new ::grpc_impl::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
                                                    [this](::grpc::ServerContext * context,
                                                           const ::grpc::ByteBuffer * request,
                                                           ::grpc::ByteBuffer * response,
                                        ::grpc::experimental::ServerCallbackRpcController * controller) {
                                            this->QueryStats(context, request, response, controller);
                                        }));
                                    }
                                    ~ExperimentalWithRawCallbackMethod_QueryStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status QueryStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::QueryStatsRequest * /*request*/, ::v2ray::core::app::stats::command::QueryStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    virtual void QueryStats(::grpc::ServerContext * /*context*/, const ::grpc::ByteBuffer * /*request*/, ::grpc::ByteBuffer * /*response*/, ::grpc::experimental::ServerCallbackRpcController *controller)
                                    {
                                        controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, ""));
                                    }
                            };
                            template <class BaseClass>
                            class ExperimentalWithRawCallbackMethod_GetSysStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    ExperimentalWithRawCallbackMethod_GetSysStats()
                                    {
                                        ::grpc::Service::experimental().MarkMethodRawCallback(2,
                                                new ::grpc_impl::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
                                                    [this](::grpc::ServerContext * context,
                                                           const ::grpc::ByteBuffer * request,
                                                           ::grpc::ByteBuffer * response,
                                        ::grpc::experimental::ServerCallbackRpcController * controller) {
                                            this->GetSysStats(context, request, response, controller);
                                        }));
                                    }
                                    ~ExperimentalWithRawCallbackMethod_GetSysStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable synchronous version of this method
                                    ::grpc::Status GetSysStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::SysStatsRequest * /*request*/, ::v2ray::core::app::stats::command::SysStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    virtual void GetSysStats(::grpc::ServerContext * /*context*/, const ::grpc::ByteBuffer * /*request*/, ::grpc::ByteBuffer * /*response*/, ::grpc::experimental::ServerCallbackRpcController *controller)
                                    {
                                        controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, ""));
                                    }
                            };
                            template <class BaseClass>
                            class WithStreamedUnaryMethod_GetStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithStreamedUnaryMethod_GetStats()
                                    {
                                        ::grpc::Service::MarkMethodStreamed(0,
                                                                            new ::grpc::internal::StreamedUnaryHandler< ::v2ray::core::app::stats::command::GetStatsRequest, ::v2ray::core::app::stats::command::GetStatsResponse>(std::bind(&WithStreamedUnaryMethod_GetStats<BaseClass>::StreamedGetStats, this, std::placeholders::_1, std::placeholders::_2)));
                                    }
                                    ~WithStreamedUnaryMethod_GetStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable regular version of this method
                                    ::grpc::Status GetStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::GetStatsRequest * /*request*/, ::v2ray::core::app::stats::command::GetStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    // replace default version of method with streamed unary
                                    virtual ::grpc::Status StreamedGetStats(::grpc::ServerContext *context, ::grpc::ServerUnaryStreamer< ::v2ray::core::app::stats::command::GetStatsRequest, ::v2ray::core::app::stats::command::GetStatsResponse> *server_unary_streamer) = 0;
                            };
                            template <class BaseClass>
                            class WithStreamedUnaryMethod_QueryStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithStreamedUnaryMethod_QueryStats()
                                    {
                                        ::grpc::Service::MarkMethodStreamed(1,
                                                                            new ::grpc::internal::StreamedUnaryHandler< ::v2ray::core::app::stats::command::QueryStatsRequest, ::v2ray::core::app::stats::command::QueryStatsResponse>(std::bind(&WithStreamedUnaryMethod_QueryStats<BaseClass>::StreamedQueryStats, this, std::placeholders::_1, std::placeholders::_2)));
                                    }
                                    ~WithStreamedUnaryMethod_QueryStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable regular version of this method
                                    ::grpc::Status QueryStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::QueryStatsRequest * /*request*/, ::v2ray::core::app::stats::command::QueryStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    // replace default version of method with streamed unary
                                    virtual ::grpc::Status StreamedQueryStats(::grpc::ServerContext *context, ::grpc::ServerUnaryStreamer< ::v2ray::core::app::stats::command::QueryStatsRequest, ::v2ray::core::app::stats::command::QueryStatsResponse> *server_unary_streamer) = 0;
                            };
                            template <class BaseClass>
                            class WithStreamedUnaryMethod_GetSysStats : public BaseClass
                            {
                                private:
                                    void BaseClassMustBeDerivedFromService(const Service * /*service*/) {}
                                public:
                                    WithStreamedUnaryMethod_GetSysStats()
                                    {
                                        ::grpc::Service::MarkMethodStreamed(2,
                                                                            new ::grpc::internal::StreamedUnaryHandler< ::v2ray::core::app::stats::command::SysStatsRequest, ::v2ray::core::app::stats::command::SysStatsResponse>(std::bind(&WithStreamedUnaryMethod_GetSysStats<BaseClass>::StreamedGetSysStats, this, std::placeholders::_1, std::placeholders::_2)));
                                    }
                                    ~WithStreamedUnaryMethod_GetSysStats() override
                                    {
                                        BaseClassMustBeDerivedFromService(this);
                                    }
                                    // disable regular version of this method
                                    ::grpc::Status GetSysStats(::grpc::ServerContext * /*context*/, const ::v2ray::core::app::stats::command::SysStatsRequest * /*request*/, ::v2ray::core::app::stats::command::SysStatsResponse * /*response*/) override
                                    {
                                        abort();
                                        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
                                    }
                                    // replace default version of method with streamed unary
                                    virtual ::grpc::Status StreamedGetSysStats(::grpc::ServerContext *context, ::grpc::ServerUnaryStreamer< ::v2ray::core::app::stats::command::SysStatsRequest, ::v2ray::core::app::stats::command::SysStatsResponse> *server_unary_streamer) = 0;
                            };
                            typedef WithStreamedUnaryMethod_GetStats<WithStreamedUnaryMethod_QueryStats<WithStreamedUnaryMethod_GetSysStats<Service > > > StreamedUnaryService;
                            typedef Service SplitStreamedService;
                            typedef WithStreamedUnaryMethod_GetStats<WithStreamedUnaryMethod_QueryStats<WithStreamedUnaryMethod_GetSysStats<Service > > > StreamedService;
                    };

                }  // namespace command
            }  // namespace stats
        }  // namespace app
    }  // namespace core
}  // namespace v2ray


#endif  // GRPC_command_2eproto__INCLUDED
