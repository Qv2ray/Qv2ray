#pragma once
#include <QtGlobal>
#ifdef Q_OS_UNIX
    #include "components/latency/LatencyTest.hpp"
    #include "uvw.hpp"

    #include <QPair>
    #include <QString>

namespace Qv2ray::components::latency::icmping
{
    class ICMPPing : public std::enable_shared_from_this<ICMPPing>
    {
      public:
        explicit ICMPPing(int ttl);
        ~ICMPPing()
        {
            deinit();
        }
        void start(std::shared_ptr<uvw::Loop> loop, LatencyTestRequest &req, LatencyTestHost *testHost);
        bool notifyTestHost(LatencyTestHost *testHost, const ConnectionId &id);

      private:
        void deinit();
        // number incremented with every echo request packet send
        unsigned short seq = 1;
        // socket
        int socketId = -1;
        bool initialized = false;
        int successCount = 0;
        LatencyTestResult data;
        std::vector<timeval> startTimevals;
        QString initErrorMessage;
    };
} // namespace Qv2ray::components::latency::icmping
#endif
