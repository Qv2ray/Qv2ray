#pragma once
#include "LatencyTest.hpp"

#include <curl/curl.h>
#include <utility>
namespace uvw
{
    class Loop;
    class TimerHandle;
} // namespace uvw
namespace Qv2ray::components::latency::realping
{
    class RealPing : public std::enable_shared_from_this<RealPing>
    {
      public:
        RealPing(std::shared_ptr<uvw::Loop> loopin, LatencyTestRequest &req, LatencyTestHost *testHost);
        ~RealPing();
        void start(const std::string &request_name = "https://www.google.com");
        void notifyTestHost();
        void recordHanleTime(CURL *);
        long getHandleTime(CURL *);

      private:
        int successCount = 0;
        LatencyTestRequest req;
        LatencyTestResult data;
        LatencyTestHost *testHost;
        std::shared_ptr<uvw::Loop> loop;
        std::shared_ptr<uvw::TimerHandle> timeout;
        std::unordered_map<CURL *, std::chrono::system_clock::time_point> reqStartTime;
    };
} // namespace Qv2ray::components::latency::realping
