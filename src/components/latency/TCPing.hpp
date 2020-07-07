#pragma once
#include "LatencyTest.hpp"
#include "base/Qv2rayBase.hpp"
#include "uvw.hpp"

namespace Qv2ray::components::latency::tcping
{
    LatencyTestResult TestTCPLatency(const QString &host, int port, int testCount);
    struct TCPing:public std::enable_shared_from_this<TCPing>
    {
        TCPing(std::shared_ptr<uvw::Loop> loop,LatencyTestRequest& req,LatencyTestHost* testHost);
        void notifyTestHost(LatencyTestHost* testHost,const ConnectionId &id);
        int successCount=0;
        int failedCount=0;
        LatencyTestResult data;
    };
} // namespace Qv2ray::components::latency::tcping
