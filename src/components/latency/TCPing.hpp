#pragma once
#include "LatencyTest.hpp"
#include "base/Qv2rayBase.hpp"
#include "coroutine.hpp"
#include "uvw.hpp"

#include <type_traits>
namespace Qv2ray::components::latency::tcping
{
    class TCPing
        : public std::enable_shared_from_this<TCPing>
        , public coroutine
    {
      public:
        TCPing(std::shared_ptr<uvw::Loop> loop, LatencyTestRequest &req, LatencyTestHost *testHost);
        void start();
        ~TCPing();

      private:
        int getAddrInfoRes(uvw::AddrInfoEvent &e);
        void tcp_ping();
        template<typename E, typename H>
        void async_DNS_lookup(E &&e, H &&h)
        {
            co_enter(*this)
            {
                if (getAddrHandle)
                {
                    getAddrHandle->once<uvw::ErrorEvent>(coro(async_DNS_lookup));
                    getAddrHandle->once<uvw::AddrInfoEvent>(coro(async_DNS_lookup));
                    co_yield return getAddrHandle->addrInfo(req.host.toStdString(), digitBuffer);
                    co_yield if constexpr (std::is_same_v<uvw::AddrInfoEvent, std::remove_reference_t<E>>)
                    {
                        if (getAddrInfoRes(e) != 0)
                        {
                            data.errorMessage = QObject::tr("DNS not resolved");
                            data.avg = LATENCY_TEST_VALUE_ERROR;
                            testHost->OnLatencyTestCompleted(req.id, data);
                            h.clear();
                            return;
                        }
                        h.clear();
                    }
                    else
                    {
                        if constexpr (std::is_same_v<uvw::ErrorEvent, std::remove_reference_t<E>>)
                        {
                            data.errorMessage = QObject::tr("DNS not resolved");
                            data.avg = LATENCY_TEST_VALUE_ERROR;
                            testHost->OnLatencyTestCompleted(req.id, data);
                            h.clear();
                            return;
                        }
                    }
                }
            }
            tcp_ping();
        }
        void notifyTestHost();
        int successCount = 0;
        LatencyTestRequest req;
        LatencyTestResult data;
        LatencyTestHost *testHost;
        struct sockaddr_storage storage;
        char digitBuffer[20] = { 0 };
        int af=AF_INET;
        std::shared_ptr<uvw::Loop> loop;
        std::shared_ptr<uvw::GetAddrInfoReq> getAddrHandle;
    };
} // namespace Qv2ray::components::latency::tcping
