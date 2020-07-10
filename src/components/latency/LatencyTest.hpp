#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::components::latency
{
    class LatencyTestThread;
    struct LatencyTestResult
    {
        QString errorMessage;
        int totalCount;
        int failedCount;
        long worst = LATENCY_TEST_VALUE_ERROR;
        long best = LATENCY_TEST_VALUE_ERROR;
        long avg = LATENCY_TEST_VALUE_ERROR;
        Qv2rayLatencyTestingMethod method;
    };

    class LatencyTestHost : public QObject
    {
        Q_OBJECT
      public:
        explicit LatencyTestHost(const int defaultCount = 3, QObject *parent = nullptr);
        void TestLatency(const ConnectionId &connectionId, Qv2rayLatencyTestingMethod);
        void StopAllLatencyTest();
        ~LatencyTestHost()
        {
            StopAllLatencyTest();
        }
      signals:
        void OnLatencyTestCompleted(const ConnectionId &id, const LatencyTestResult &data);

      private slots:
        void OnLatencyThreadProcessCompleted();

      private:
        int totalTestCount;
        QList<LatencyTestThread *> latencyThreads;
    };
} // namespace Qv2ray::components::latency

using namespace Qv2ray::components::latency;
