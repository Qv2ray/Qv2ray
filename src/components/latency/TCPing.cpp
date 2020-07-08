#include "TCPing.hpp"

#ifdef _WIN32
#define NOMINMAX
    #include <WinSock2.h>
#else
    #include <sys/socket.h>
#endif

namespace Qv2ray::components::latency::tcping
{

    void TCPing::start(std::shared_ptr<uvw::Loop> loop, LatencyTestRequest &req, LatencyTestHost *testHost)
    {
        struct sockaddr_storage storage;
        data.totalCount = req.totalCount;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        if (getSockAddress(loop, req.host.toStdString().data(), req.port, &storage, 0) != 0)
        {
            data.errorMessage = QObject::tr("DNS not resolved");
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        for (int i = 0; i < req.totalCount; ++i)
        {
            auto tcpClient = loop->resource<uvw::TCPHandle>();
            system_clock::time_point start = system_clock::now();
            tcpClient->connect(reinterpret_cast<const sockaddr &>(storage));
            tcpClient->once<uvw::ErrorEvent>([ptr = shared_from_this(), testHost, this, host = req.host, port = req.port,
                                              id = req.id](const uvw::ErrorEvent &e, uvw::TCPHandle &h) {
                LOG(MODULE_NETWORK, "error connecting to host: " + host + ":" + QSTRN(port) + " " + e.what())
                data.failedCount += 1;
                data.errorMessage = e.what();
                notifyTestHost(testHost, id);
                h.clear();
            });
            tcpClient->once<uvw::ConnectEvent>([ptr = shared_from_this(), start, testHost, id = req.id, this](auto &, auto &h) {
                ++successCount;
                system_clock::time_point end = system_clock::now();
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                long ms = milliseconds.count();
                data.avg += ms;
                data.worst = std::max(data.worst, ms);
                data.best = std::min(data.best, ms);
                notifyTestHost(testHost, id);
                h.clear();
                h.close();
            });
        }
    }
    void TCPing::notifyTestHost(LatencyTestHost *testHost, const ConnectionId &id)
    {
        if (data.failedCount + successCount == data.totalCount)
        {
            if (data.failedCount == data.totalCount)
                data.avg = LATENCY_TEST_VALUE_ERROR;
            else
                data.errorMessage.clear(),data.avg = data.avg / successCount;
            testHost->OnLatencyTestCompleted(id, data);
        }
    }
} // namespace Qv2ray::components::latency::tcping
