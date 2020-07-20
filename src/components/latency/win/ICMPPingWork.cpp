#include "uvw.hpp"
#include "ICMPPing.hpp"
#ifdef Q_OS_WIN
namespace Qv2ray::components::latency::icmping
{
    void ICMPPing::start(std::shared_ptr<uvw::Loop> loop, LatencyTestRequest &req, LatencyTestHost *testHost)
    {
        data.totalCount = req.totalCount;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        for (int i = 0; i < req.totalCount; ++i)
        {
            auto work = loop->resource<uvw::WorkReq>([ptr = shared_from_this(), this, addr = req.host, id = req.id, testHost]() mutable {
                auto pingres = ping(addr);
                if (!pingres.second.isEmpty())
                {
                    data.errorMessage = pingres.second;
                    data.failedCount++;
                }
                else
                {
                    data.avg += pingres.first;
                    data.best = std::min(pingres.first, data.best);
                    data.worst = std::max(pingres.first, data.worst);
                    successCount++;
                }
                notifyTestHost(testHost, id);
                ptr.reset();
            });
            work->queue();
        }
    }
}
#endif
