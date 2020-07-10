#include "LatencyTest.hpp"

#include "LatencyTestThread.hpp"
#include "core/handler/ConfigHandler.hpp"

constexpr auto LATENCY_PROPERTY_KEY = "__QvLatencyTest__";

namespace Qv2ray::components::latency
{
    LatencyTestHost::LatencyTestHost(const int defaultCount, QObject *parent) : QObject(parent)
    {
        totalTestCount = defaultCount;
    }

    void LatencyTestHost::StopAllLatencyTest()
    {
        for (const auto &thread : latencyThreads)
        {
            thread->terminate();
        }
        latencyThreads.clear();
    }
    void LatencyTestHost::TestLatency(const ConnectionId &id, Qv2rayLatencyTestingMethod method)
    {
        const auto &[protocol, host, port] = GetConnectionInfo(id);
        auto thread = new LatencyTestThread(host, port, method, totalTestCount, this);
        connect(thread, &QThread::finished, this, &LatencyTestHost::OnLatencyThreadProcessCompleted);
        thread->setProperty(LATENCY_PROPERTY_KEY, QVariant::fromValue(id));
        latencyThreads.push_back(thread);
        thread->start();
    }

    void LatencyTestHost::OnLatencyThreadProcessCompleted()
    {
        const auto senderThread = qobject_cast<LatencyTestThread *>(sender());
        latencyThreads.removeOne(senderThread);
        auto result = senderThread->GetResult();

        if (!result.errorMessage.isEmpty())
        {
            LOG(MODULE_NETWORK, "Ping --> " + result.errorMessage)
            result.avg = LATENCY_TEST_VALUE_ERROR;
            result.best = LATENCY_TEST_VALUE_ERROR;
            result.worst = LATENCY_TEST_VALUE_ERROR;
        }

        emit OnLatencyTestCompleted(senderThread->property(LATENCY_PROPERTY_KEY).value<ConnectionId>(), result);
    }

} // namespace Qv2ray::components::latency
