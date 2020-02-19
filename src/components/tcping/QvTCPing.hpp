#pragma once
#include "base/Qv2rayBase.hpp"
#include <limits>

namespace Qv2ray::components::tcping
{
    struct QvTCPingData {
        ConnectionObject connectionIdentifier;
        QString hostName;
        int port;
        QString errorMessage;
        int total, succeed, failed;
        double min = numeric_limits<double>::max(), max = 0.0, avg = 0.0;
    };

    class QvTCPingModel : public QObject
    {
            Q_OBJECT

        public:
            explicit QvTCPingModel(int defaultCount = 5, QObject *parent = nullptr);
            void StartPing(const ConnectionObject &connectionName, const QString &hostName, int port);
            void StopAllPing();
        signals:
            void PingFinished(QvTCPingData data);
        private:
            static QvTCPingData startTestLatency(QvTCPingData data, const int count);
            int count;
            QQueue<QFutureWatcher<QvTCPingData>*> pingWorkingThreads;
    };
}

using namespace Qv2ray::components::tcping;
