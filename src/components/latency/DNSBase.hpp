#pragma once
#include "LatencyTest.hpp"
#include "coroutine.hpp"
#include "uvw.hpp"

namespace Qv2ray::components::latency
{
    template<typename T>
    class DNSBase
        : public coroutine
        , public std::enable_shared_from_this<T>
    {
      public:
        DNSBase(const std::shared_ptr<uvw::Loop> &loopin, LatencyTestRequest &req, LatencyTestHost *testHost)
            : loop(loopin), req(std::move(req)), testHost(testHost)
        {
        }
        virtual ~DNSBase();

      protected:
        int isAddr()
        {
            auto host = req.host.toStdString();
            if (uv_ip4_addr(host.data(), req.port, reinterpret_cast<sockaddr_in *>(&storage)) == 0)
            {
                return AF_INET;
            }
            if (uv_ip6_addr(host.data(), req.port, reinterpret_cast<sockaddr_in6 *>(&storage)) == 0)
            {
                return AF_INET6;
            }
            return -1;
        }

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
            if (getAddrHandle)
                getAddrHandle->clear();
        }
        int getAddrInfoRes(uvw::AddrInfoEvent &e)
        {
            struct addrinfo *rp = nullptr;
            for (rp = e.data.get(); rp != nullptr; rp = rp->ai_next)
                if (rp->ai_family == AF_INET)
                {
                    if (rp->ai_family == AF_INET)
                    {
                        af = AF_INET;
                        memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in));
                    }
                    else if (rp->ai_family == AF_INET6)
                    {
                        af = AF_INET6;
                        memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in6));
                    }
                    break;
                }
            if (rp == nullptr)
            {
                // fallback: if we can't find prefered AF, then we choose alternative.
                for (rp = e.data.get(); rp != nullptr; rp = rp->ai_next)
                {
                    if (rp->ai_family == AF_INET)
                    {
                        af = AF_INET;
                        memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in));
                    }
                    else if (rp->ai_family == AF_INET6)
                    {
                        af = AF_INET6;
                        memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in6));
                    }
                    break;
                }
            }
            if (rp)
                return 0;
            return -1;
        }
        virtual void ping() = 0;

      protected:
        int af = AF_INET;
        int successCount = 0;
        LatencyTestRequest req;
        LatencyTestResult data;
        LatencyTestHost *testHost;
        struct sockaddr_storage storage;
        char digitBuffer[20] = { 0 };
        std::shared_ptr<uvw::Loop> loop;
        std::shared_ptr<uvw::GetAddrInfoReq> getAddrHandle;
    };
    template<typename T>
    DNSBase<T>::~DNSBase()
    {
    }
} // namespace Qv2ray::components::latency
