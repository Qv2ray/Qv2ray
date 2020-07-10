#pragma once
#include "LatencyTest.hpp"
#include "base/Qv2rayBase.hpp"
#include "coroutine.hpp"
#include "uvw.hpp"

#include <type_traits>
namespace Qv2ray::components::latency::tcping
{
    struct TCPing
        : public std::enable_shared_from_this<TCPing>
        , public coroutine
    {
        TCPing(std::shared_ptr<uvw::Loop> loop, LatencyTestRequest &req, LatencyTestHost *testHost);
        void start();
        int getAddrInfoRes(uvw::AddrInfoEvent &e);
        ~TCPing();
        template<typename E, typename H>
        void start(E &&e, H &&h)
        {
            co_enter(*this)
            {
                if (getAddrHandle)
                {
                    getAddrHandle->once<uvw::ErrorEvent>(coro(start));
                    getAddrHandle->once<uvw::AddrInfoEvent>(coro(start));
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
            for (; data.totalCount <= req.totalCount; ++data.totalCount)
            {
                auto tcpClient = loop->resource<uvw::TCPHandle>();
                tcpClient->once<uvw::ErrorEvent>([ptr = shared_from_this(), this](const uvw::ErrorEvent &e, uvw::TCPHandle &h) {
                    LOG(MODULE_NETWORK, "error connecting to host: " + req.host + ":" + QSTRN(req.port) + " " + e.what())
                    data.failedCount += 1;
                    data.errorMessage = e.what();
                    notifyTestHost();
                    h.clear();
                });
                tcpClient->once<uvw::ConnectEvent>([ptr = shared_from_this(), start = system_clock::now(), this](auto &, auto &h) {
                    ++successCount;
                    system_clock::time_point end = system_clock::now();
                    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    long ms = milliseconds.count();
                    data.avg += ms;
                    data.worst = std::max(data.worst, ms);
                    data.best = std::min(data.best, ms);
                    notifyTestHost();
                    h.clear();
                    h.close();
                });
                tcpClient->connect(reinterpret_cast<const sockaddr &>(storage));
            }
        }
        void notifyTestHost();
        int successCount = 0;
        LatencyTestRequest req;
        LatencyTestResult data;
        LatencyTestHost *testHost;
        struct sockaddr_storage storage;
        char digitBuffer[20] = { 0 };
        std::shared_ptr<uvw::Loop> loop;
        std::shared_ptr<uvw::GetAddrInfoReq> getAddrHandle;
    };
} // namespace Qv2ray::components::latency::tcping
