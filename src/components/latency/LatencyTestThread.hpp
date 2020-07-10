#pragma once
#include "LatencyTest.hpp"

#include <QThread>
namespace Qv2ray::components::latency
{
    class LatencyTestThread : public QThread
    {
        Q_OBJECT
      public:
        explicit LatencyTestThread(const QString &host, int port, Qv2rayLatencyTestingMethod, int count, QObject *parent = nullptr);
        LatencyTestResult GetResult() const
        {
            return resultData;
        }

      protected:
        void run() override;

      private:
        LatencyTestResult resultData;
        QString host;
        int port;
        int count;
        Qv2rayLatencyTestingMethod method;

        // static LatencyTestResult TestLatency_p(const ConnectionId &id, const int count);
    };

} // namespace Qv2ray::components::latency
