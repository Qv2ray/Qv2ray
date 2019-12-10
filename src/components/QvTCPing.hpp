#ifndef QVTCPING_H
#define QVTCPING_H
#ifdef _WIN32
#include <winsock2.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#endif
#include <sys/time.h>
#include <unistd.h>
#include "QvUtils.hpp"


namespace Qv2ray
{
    namespace Components
    {
        struct QvTCPingData {
            QString connectionName;
            QString hostName;
            int port;
            QString errorMessage;
            int total, succeed, failed;
            double min = 999999999999999.0, max = 0.0, avg = 0.0;
        };
        //

        class QvTCPingModel : public QObject
        {
                Q_OBJECT

            public:
                explicit QvTCPingModel(int defaultCount = 5, QObject *parent = nullptr);
                void StartPing(const QString &connectionName, const QString &hostName, int port);
            signals:
                void PingFinished(QvTCPingData data);
            private:
                static int resolveHost(const string &host, int portnr, struct addrinfo **res);
                static int testLatency(struct addrinfo *addr, struct timeval *rtt);
                static QvTCPingData startTestLatency(QvTCPingData data, const int count);
                int count;
                QQueue<QFutureWatcher<QvTCPingData>*> pingWorkingThreads;
        };
    }
}

using namespace Qv2ray;
using namespace Qv2ray::Components;
#endif
