#include "TCPing.hpp"

#include "uvw.hpp"

namespace Qv2ray::components::latency::tcping
{
    constexpr int conn_timeout_sec = 5;

    int getSocket(int af, int socktype, int proto)
    {
        uv_os_sock_t fd;
#ifndef INVALID_SOCKET
    #define INVALID_SOCKET -1
#endif
        if ((fd = socket(af, socktype, proto)) == INVALID_SOCKET)
        {
            return 0;
        }
        // Set TCP connection timeout per-socket level.
        // See [https://github.com/libuv/help/issues/54] for details.
#if defined(_WIN32) && !defined(__SYMBIAN32__)
        setsockopt(fd, IPPROTO_TCP, TCP_MAXRT, (char *) &conn_timeout_sec, sizeof(conn_timeout_sec));
#elif defined(__APPLE__)
        // (billhoo) MacOS uses TCP_CONNECTIONTIMEOUT to do so.
        setsockopt(fd, IPPROTO_TCP, TCP_CONNECTIONTIMEOUT, (char *) &conn_timeout_sec, sizeof(conn_timeout_sec));
#else // Linux like systems
        uint32_t conn_timeout_ms = conn_timeout_sec * 1000;
        setsockopt(fd, IPPROTO_TCP, TCP_USER_TIMEOUT, (char *) &conn_timeout_ms, sizeof(conn_timeout_ms));
#endif
        return (int) fd;
    }

    void TCPing::start()
    {
        data.totalCount = 0;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        af = isAddr();
        if (af == -1)
        {
            getAddrHandle = loop->resource<uvw::GetAddrInfoReq>();
            sprintf(digitBuffer, "%d", req.port);
        }
        async_DNS_lookup(0, 0);
    }
    TCPing::~TCPing()
    {
    }
    void TCPing::notifyTestHost()
    {
        if (data.failedCount + successCount == req.totalCount)
        {
            if (data.failedCount == req.totalCount)
                data.avg = LATENCY_TEST_VALUE_ERROR;
            else
                data.errorMessage.clear(), data.avg = data.avg / successCount;
            testHost->OnLatencyTestCompleted(req.id, data);
        }
    }
    void TCPing::ping()
    {
        for (; data.totalCount < req.totalCount; ++data.totalCount)
        {
            auto tcpClient = loop->resource<uvw::TCPHandle>();
            tcpClient->open(getSocket(af, SOCK_STREAM, IPPROTO_TCP));
            tcpClient->once<uvw::ErrorEvent>([ptr = shared_from_this(), this](const uvw::ErrorEvent &e, uvw::TCPHandle &h) {
                LOG(MODULE_NETWORK, "error connecting to host: " + req.host + ":" + QSTRN(req.port) + " " + e.what())
                data.failedCount += 1;
                data.errorMessage = e.what();
                notifyTestHost();
                h.clear();
                h.close();
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
} // namespace Qv2ray::components::latency::tcping
