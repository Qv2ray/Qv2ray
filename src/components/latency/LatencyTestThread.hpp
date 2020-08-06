#pragma once
#include "LatencyTest.hpp"

#include <QThread>
#include <mutex>
#include <unordered_set>
#include <curl/curl.h>

namespace uvw
{
    class Loop;
    class TimerHandle;
} // namespace uvw
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
        struct CURLGlobal
        {
            CURLGlobal(){curl_global_init(CURL_GLOBAL_ALL);}
            ~CURLGlobal(){curl_global_cleanup();}
        };
        std::shared_ptr<uvw::Loop> loop;
        CURLGlobal curlGlobal;
        bool isStop = false;
        std::shared_ptr<uvw::TimerHandle> stopTimer;
        std::vector<LatencyTestRequest> requests;
        std::mutex m;

        // static LatencyTestResult TestLatency_p(const ConnectionId &id, const int count);
    };

} // namespace Qv2ray::components::latency
