#pragma once

/**
 * ICMPPinger - An Implementation of ICMPPing on Windows Platform
 * Required Windows Version: 2000 / XP / 7 / Vista+
 * License: WTFPL
 */
#include <QtGlobal>
#ifdef Q_OS_WIN

    #include <QPair>
    #include <QString>
    #include <memory>
    #include <optional>
    #include <utility>

namespace Qv2ray::components::latency::icmping
{
    class ICMPPing
    {
      public:
        ICMPPing(uint64_t timeout = DEFAULT_TIMEOUT);
        ~ICMPPing();

      public:
        static const uint64_t DEFAULT_TIMEOUT = 10000U;

      public:
        QPair<long, QString> ping(const QString &ipAddr);

      private:
        uint64_t timeout = DEFAULT_TIMEOUT;
    };
} // namespace Qv2ray::components::latency::icmping
#endif
