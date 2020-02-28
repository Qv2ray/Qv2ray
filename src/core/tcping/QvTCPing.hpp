#pragma once
#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"

namespace Qv2ray::core::tcping
{
    struct QvTCPingResultObject
    {
        ConnectionId connectionId = NullConnectionId;
        QString errorMessage;
        int total, succeed, failed;
        uint worst = 0, best = 0, avg = 0;
    };

    class QvTCPingHelper : public QObject
    {
        Q_OBJECT

      public:
        explicit QvTCPingHelper(const int defaultCount = 5, QObject *parent = nullptr);
        void TestLatency(const ConnectionId &connectionId);
        void StopAllLatenceTest();
      signals:
        void OnLatencyTestCompleted(const QvTCPingResultObject &data);

      private:
        static QvTCPingResultObject TestLatency_p(const ConnectionId &id, const int count);
        int count;
        QQueue<QFutureWatcher<QvTCPingResultObject> *> pingWorkingThreads;
    };
} // namespace Qv2ray::core::tcping

using namespace Qv2ray::core::tcping;
