#pragma once
#include "LatencyTest.hpp"
#include "uvw.hpp"
#include <mutex>

#include <QThread>
namespace Qv2ray::components::latency
{
    class LatencyTestThread : public QThread
    {
        Q_OBJECT
      public:
        explicit LatencyTestThread(QObject *parent = nullptr);
        void stopLatencyTest(){isStop=false;}
        void pushRequests(const ConnectionId&id,const QString& host,int port,int totalTestCount,Qv2rayLatencyTestingMethod method);
      protected:
        void run() override;

      private:
        std::shared_ptr<uvw::Loop> loop;
        bool isStop=false;
        std::shared_ptr<uvw::TimerHandle> stopTimer;
        std::vector<LatencyTestRequest> requests;
        std::mutex m;

        // static LatencyTestResult TestLatency_p(const ConnectionId &id, const int count);
    };

} // namespace Qv2ray::components::latency
