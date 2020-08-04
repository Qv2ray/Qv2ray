#pragma once

#include <QtGlobal>
#ifdef Q_OS_WIN

    #include "../DNSBase.hpp"

    #include <memory>

namespace Qv2ray::components::latency::icmping
{
    class ICMPPing : public DNSBase<ICMPPing>
    {
      public:
        using DNSBase<ICMPPing>::DNSBase;
        ~ICMPPing();

      public:
        static const uint64_t DEFAULT_TIMEOUT = 10000U;
        void start();
        bool notifyTestHost(LatencyTestHost *testHost, const ConnectionId &id);

      private:
        void ping() override;

      private:
        void pingImpl();

      private:
        uint64_t timeout = DEFAULT_TIMEOUT;
        std::shared_ptr<uvw::TimerHandle> waitHandleTimer;
    };
} // namespace Qv2ray::components::latency::icmping
#endif
