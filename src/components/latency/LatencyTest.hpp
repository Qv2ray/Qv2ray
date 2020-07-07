#pragma once
#include "base/Qv2rayBase.hpp"
#include "uvw.hpp"

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

    int getSockAddress(std::shared_ptr<uvw::Loop>& loop, const char* host, int port, struct sockaddr_storage* storage, int ipv6first);


    class LatencyTestHost : public QObject
    {
        Q_OBJECT
      public:
        explicit LatencyTestHost(const int defaultCount = 3, QObject *parent = nullptr);
        void TestLatency(const ConnectionId &connectionId, Qv2rayLatencyTestingMethod);
        void StopAllLatencyTest();
        ~LatencyTestHost();
      signals:
        void OnLatencyTestCompleted(const ConnectionId &id, const LatencyTestResult &data);

      private:
        int totalTestCount;
        QVector<LatencyTestThread *> latencyThreads;
        int nextLatencyTestThread=0;
    };
} // namespace Qv2ray::components::latency

using namespace Qv2ray::components::latency;
