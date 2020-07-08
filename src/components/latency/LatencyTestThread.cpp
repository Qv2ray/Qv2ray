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
                handle.stop();
                handle.close();
                requests.clear();
                loop->clear();
                loop->close();
                loop->stop();
            }
            else
            {
                std::unique_lock<std::mutex> lockGuard{ m, std::defer_lock };
                if (!lockGuard.try_lock())
                    return;
                if (requests.empty())
                    return;
                auto parent = qobject_cast<LatencyTestHost *>(this->parent());
                for (auto &req : requests)
                {
                    switch (req.method)
                    {
                        case ICMPING:
                        {
                            auto ptr = std::make_shared<icmping::ICMPPing>(30);
                            ptr->start(loop, req, parent);
                        }
                        break;
                        case TCPING:
                        default:
                        {
                            auto ptr = std::make_shared<tcping::TCPing>();
                            ptr->start(loop, req, parent);
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
        std::unique_lock<std::mutex> lockGuard{ m };
        for (const auto &id : ids)
        {
            const auto &[protocol, host, port] = GetConnectionInfo(id);
            requests.emplace_back(LatencyTestRequest{ id, host, port, totalTestCount, method });
        }
    }
} // namespace Qv2ray::components::latency
