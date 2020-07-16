#include "uvw.hpp"
#include "LatencyTest.hpp"

#include "LatencyTestThread.hpp"
#include "core/handler/ConfigHandler.hpp"

namespace Qv2ray::components::latency
{
    int isAddr(const char *host, int port, struct sockaddr_storage *storage, int ipv6first)
    {
        if (uv_ip4_addr(host, port, reinterpret_cast<sockaddr_in *>(storage)) == 0)
        {
            return AF_INET;
        }
        if (uv_ip6_addr(host, port, reinterpret_cast<sockaddr_in6 *>(storage)) == 0)
        {
            return AF_INET6;
        }
        return -1;
    }
    LatencyTestHost::LatencyTestHost(const int defaultCount, QObject *parent) : QObject(parent)
    {
        qRegisterMetaType<ConnectionId>();
        qRegisterMetaType<LatencyTestResult>();
        totalTestCount = defaultCount;
        latencyThread = new LatencyTestThread(this);
        latencyThread->start();
    }

    LatencyTestHost::~LatencyTestHost()
    {
        latencyThread->stopLatencyTest();
        latencyThread->wait();
    }

    void LatencyTestHost::StopAllLatencyTest()
    {
        latencyThread->stopLatencyTest();
        latencyThread->wait();
        latencyThread->start();
    }

    void LatencyTestHost::TestLatency(const ConnectionId &id, Qv2rayLatencyTestingMethod method)
    {
        latencyThread->pushRequest(id, totalTestCount, method);
    }
    void LatencyTestHost::TestLatency(const QList<ConnectionId> &ids, Qv2rayLatencyTestingMethod method)
    {
        latencyThread->pushRequest(ids, totalTestCount, method);
    }

} // namespace Qv2ray::components::latency
