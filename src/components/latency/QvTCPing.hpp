#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::components::tcping
{
    struct QvTCPingResultObject
    {
        ConnectionId connectionId = NullConnectionId;
        QString errorMessage;
        int total, succeed, failed;
        uint worst = QVTCPING_VALUE_ERROR, best = QVTCPING_VALUE_ERROR, avg = QVTCPING_VALUE_ERROR;
    };

    class QvTCPingHelper : public QObject
    {
        Q_OBJECT

      public:
        explicit QvTCPingHelper(const int defaultCount = 3, QObject *parent = nullptr);
        void TestLatency(const ConnectionId &connectionId);
        void StopAllLatenceTest();
      signals:
        void OnLatencyTestCompleted(const QvTCPingResultObject &data);

      private:
        static QvTCPingResultObject TestLatency_p(const ConnectionId &id, const int count);
        int count;
        QQueue<QFutureWatcher<QvTCPingResultObject> *> pingWorkingThreads;
    };
} // namespace Qv2ray::components::tcping

using namespace Qv2ray::components::tcping;
