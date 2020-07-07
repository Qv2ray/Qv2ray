#pragma once
#include "LatencyTest.hpp"
#include "TCPing.hpp"
#include "uvw.hpp"

#include <QThread>
#include <mutex>
#include <unordered_set>
namespace Qv2ray::components::latency
{
    class LatencyTestThread : public QThread
    {
        Q_OBJECT
      public:
        explicit LatencyTestThread(QObject *parent = nullptr);
        void stopLatencyTest()
        {
            isStop = true;
        }
        void pushRequest(const QList<ConnectionId> &ids, int totalTestCount, Qv2rayLatencyTestingMethod method);
        void pushRequest(const ConnectionId &id, int totalTestCount, Qv2rayLatencyTestingMethod method);

      protected:
        void run() override;

      private:
        std::shared_ptr<uvw::Loop> loop;
        bool isStop = false;
        std::shared_ptr<uvw::TimerHandle> stopTimer;
        std::vector<LatencyTestRequest> requests;
        std::mutex m;

        // static LatencyTestResult TestLatency_p(const ConnectionId &id, const int count);
    };

} // namespace Qv2ray::components::latency
