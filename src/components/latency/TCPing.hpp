#pragma once
#include "LatencyTest.hpp"
#include "base/Qv2rayBase.hpp"
namespace Qv2ray::components::latency::tcping
{
    LatencyTestResult TestTCPLatency(const QString &host, int port, int testCount);
} // namespace Qv2ray::components::latency::tcping
