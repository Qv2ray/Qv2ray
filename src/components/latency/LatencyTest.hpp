#pragma once
#include "base/Qv2rayBase.hpp"
namespace uvw
{
    class Loop;
}
struct sockaddr_storage;
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
    struct LatencyTestRequest
    {
        ConnectionId id;
        QString host;
        int port;
        int totalCount;
        Qv2rayLatencyTestingMethod method;
    };

    class LatencyTestHost : public QObject
    {
        Q_OBJECT
      public:
        explicit LatencyTestHost(const int defaultCount = 3, QObject *parent = nullptr);
        void TestLatency(const ConnectionId &connectionId, Qv2rayLatencyTestingMethod);
        void TestLatency(const QList<ConnectionId> &connectionIds, Qv2rayLatencyTestingMethod);
        void StopAllLatencyTest();

        ~LatencyTestHost() override;

      signals:
        void OnLatencyTestCompleted(ConnectionId id, LatencyTestResult data);

      private:
        int totalTestCount;
        // we're not introduce multi latency test thread for now,
        // cause it's easy to use a scheduler like round-robin scheme
        // and libuv event loop is fast.
        LatencyTestThread *latencyThread;
    };
} // namespace Qv2ray::components::latency

using namespace Qv2ray::components::latency;
Q_DECLARE_METATYPE(LatencyTestResult)
