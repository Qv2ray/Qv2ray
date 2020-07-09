#pragma once

/**
 * ICMPPinger - An Implementation of ICMPPing on Windows Platform
 * Required Windows Version: 2000 / XP / 7 / Vista+
 * License: WTFPL
 */
#include <QtGlobal>
#ifdef Q_OS_WIN

    #include "components/latency/LatencyTest.hpp"

    #include <QPair>
    #include <QString>
    #include <memory>
    #include <optional>
    #include <utility>
namespace uvw
{
    class Loop;
}
namespace Qv2ray::components::latency::icmping
{
    class ICMPPing : public std::enable_shared_from_this<ICMPPing>
    {
      public:
        ICMPPing(uint64_t timeout = DEFAULT_TIMEOUT);
        ~ICMPPing();

      public:
        static const uint64_t DEFAULT_TIMEOUT = 10000U;
        void start(std::shared_ptr<uvw::Loop> loop, LatencyTestRequest &req, LatencyTestHost *testHost);
        bool notifyTestHost(LatencyTestHost *testHost, const ConnectionId &id);

      public:
        QPair<long, QString> ping(const QString &ipAddr);

      private:
        uint64_t timeout = DEFAULT_TIMEOUT;
        int successCount = 0;
        LatencyTestResult data;
    };
} // namespace Qv2ray::components::latency::icmping
#endif
