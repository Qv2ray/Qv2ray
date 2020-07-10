#pragma once
#include <QtGlobal>
#ifdef Q_OS_UNIX
    #include "components/latency/LatencyTest.hpp"

    #include <QPair>
    #include <QString>
    #include "components/latency/coroutine.hpp"
    #include "uvw.hpp"
namespace Qv2ray::components::latency::icmping
{
    class ICMPPing : public std::enable_shared_from_this<ICMPPing>,public coroutine
    {
      public:
        explicit ICMPPing(int ttl,std::shared_ptr<uvw::Loop> loop,LatencyTestRequest& req,LatencyTestHost* testHost);
        ~ICMPPing()
        {
            deinit();
        }
        void start();
      private:
        int getAddrInfoRes(uvw::AddrInfoEvent &e);
        void ping();
        bool notifyTestHost();
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
            ping();
        }

      private:
        void deinit();
        // number incremented with every echo request packet send
        unsigned short seq = 1;
        // socket
        int socketId = -1;
        bool initialized = false;
        int successCount = 0;
        LatencyTestRequest req;
        LatencyTestHost* testHost;
        LatencyTestResult data;
        struct sockaddr_storage storage;
        char digitBuffer[20] = { 0 };
        std::shared_ptr<uvw::Loop> loop;
        std::shared_ptr<uvw::GetAddrInfoReq> getAddrHandle;
        std::vector<timeval> startTimevals;
        QString initErrorMessage;
    };
} // namespace Qv2ray::components::latency::icmping
#endif
