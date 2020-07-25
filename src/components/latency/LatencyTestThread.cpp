#include "LatencyTestThread.hpp"

#include "TCPing.hpp"
#include "core/CoreUtils.hpp"

#ifdef Q_OS_UNIX
    #include "unix/ICMPPing.hpp"
#else
    #include "win/ICMPPing.hpp"
#endif
#include "uvw.hpp"

namespace Qv2ray::components::latency
{

    LatencyTestThread::LatencyTestThread(QObject *parent) : QThread(parent)
    {
    }

    void LatencyTestThread::pushRequest(const ConnectionId &id, int totalTestCount, Qv2rayLatencyTestingMethod method)
    {
        if (isStop)
            return;
        std::unique_lock<std::mutex> lockGuard{ m };
        const auto &[protocol, host, port] = GetConnectionInfo(id);
        requests.emplace_back(LatencyTestRequest{ id, host, port, totalTestCount, method });
    }

    void LatencyTestThread::run()
    {
        loop = uvw::Loop::create();
        stopTimer = loop->resource<uvw::TimerHandle>();
        stopTimer->on<uvw::TimerEvent>([this](auto &, auto &handle) {
            if (isStop)
            {
                if (!requests.empty())
                    requests.clear();
                int timer_count = 0;
                uv_walk(
                    loop->raw(),
                    [](uv_handle_t *handle, void *arg) {
                        int &counter = *static_cast<int *>(arg);
                        if (uv_is_closing(handle) == 0)
                            counter++;
                    },
                    &timer_count);
                if (timer_count == 1) // only current timer
                {
                    handle.stop();
                    handle.close();
                    loop->clear();
                    loop->close();
                    loop->stop();
                }
            }
            else
            {
                if (requests.empty())
                    return;
                std::unique_lock<std::mutex> lockGuard{ m };
                auto parent = qobject_cast<LatencyTestHost *>(this->parent());
                for (auto &req : requests)
                {
                    switch (req.method)
                    {
                        case ICMPING:
                        {
                            auto ptr = std::make_shared<icmping::ICMPPing>(loop, req, parent);
                            ptr->start();
                        }
                        break;
                        case TCPING:
                        default:
                        {
                            auto ptr = std::make_shared<tcping::TCPing>(loop, req, parent);
                            ptr->start();
                            break;
                        }
                    }
                }
                requests.clear();
            }
        });
        stopTimer->start(uvw::TimerHandle::Time{ 500 }, uvw::TimerHandle::Time{ 500 });
        loop->run();
    }
    void LatencyTestThread::pushRequest(const QList<ConnectionId> &ids, int totalTestCount, Qv2rayLatencyTestingMethod method)
    {
        if (isStop)
            return;
        std::unique_lock<std::mutex> lockGuard{ m };
        for (const auto &id : ids)
        {
            const auto &[protocol, host, port] = GetConnectionInfo(id);
            requests.emplace_back(LatencyTestRequest{ id, host, port, totalTestCount, method });
        }
    }
} // namespace Qv2ray::components::latency
