#pragma once
#include <QtGlobal>
#ifdef Q_OS_UNIX
    #include <QPair>
    #include <QString>
namespace Qv2ray::components::latency::icmping
{
    class ICMPPing
    {
      public:
        explicit ICMPPing(int ttl);
        ~ICMPPing()
        {
            deinit();
        }
        QPair<int64_t, QString> ping(const QString &address);

      private:
        void deinit();
        // number incremented with every echo request packet send
        unsigned short seq = 1;
        // socket
        int socketId = -1;
        bool initialized = false;
        QString initErrorMessage;
    };
} // namespace Qv2ray::components::latency::icmping
#endif
