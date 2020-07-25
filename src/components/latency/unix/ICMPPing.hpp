#pragma once
#include <QtGlobal>
#ifdef Q_OS_UNIX

    #include "../DNSBase.hpp"
    #include "uvw.hpp"

    #include <QString>
namespace Qv2ray::components::latency::icmping
{
    class ICMPPing : public DNSBase<ICMPPing>
    {
      public:
        using DNSBase<ICMPPing>::DNSBase;
        ~ICMPPing() override;
        void start(int ttl = 30);

      private:
        void ping() override;
        bool notifyTestHost();

      private:
        void deinit();
        // number incremented with every echo request packet send
        unsigned short seq = 1;
        // socket
        int socketId = -1;
        std::shared_ptr<uvw::TimerHandle> timeoutTimer;
        std::shared_ptr<uvw::PollHandle> pollHandle;
        std::vector<timeval> startTimevals;
    };
} // namespace Qv2ray::components::latency::icmping
#endif
