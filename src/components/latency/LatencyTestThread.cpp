#include "LatencyTestThread.hpp"

#include "TCPing.hpp"

#ifdef Q_OS_UNIX
    #include "unix/ICMPPing.hpp"
#else
    #include "win/ICMPPing.hpp"
#endif

namespace Qv2ray::components::latency
{

    LatencyTestThread::LatencyTestThread(QObject *parent)
        : QThread(parent)
    {
    }

    void LatencyTestThread::pushRequests(const ConnectionId&id,const QString& host,int port,int totalTestCount,Qv2rayLatencyTestingMethod method)
    {
        std::unique_lock<std::mutex> lockGuard{m};
        requests.emplace_back(LatencyTestRequest{id,host,port,totalTestCount,method});
    }

    void LatencyTestThread::run()
    {
        loop=uvw::Loop::create();
        stopTimer=loop->resource<uvw::TimerHandle>();
        stopTimer->on<uvw::TimerEvent>([this](auto&, auto& handle) {
            if (isStop) {
                handle.stop();
                handle.close();
                requests.clear();
                loop->clear();
                loop->close();
                loop->stop();
            } else {
                std::unique_lock<std::mutex> lockGuard{m};
                auto parent=qobject_cast<LatencyTestHost*>(this->parent());
                for(auto& req:requests)
                {
                    switch(req.method)
                    {
                        case ICMPING:
                        break;
                        case TCPING:
                        default:
                        {
                            std::make_shared<tcping::TCPing>(loop,req,parent);
                            break;
                        }
                    }
                }
                requests.clear();
            }
        });
        loop->run();
//        resultData.avg = 0;
//        resultData.best = 0;
//        resultData.worst = 0;
//        switch (method)
//        {
//            case ICMPING:
//            {
//                icmping::ICMPPing ping(30);
//                for (auto i = 0; i < count; i++)
//                {
//                    resultData.totalCount++;
//                    const auto pair = ping.ping(host);
//                    const auto &errMessage = pair.second;
//                    const long _latency = pair.first;
//                    if (!errMessage.isEmpty())
//                    {
//                        resultData.errorMessage.append(NEWLINE + errMessage);
//                        resultData.failedCount++;
//                    }
//                    else
//                    {
//#ifdef Q_OS_WIN
//                        // Is it Windows?
//    #undef min
//    #undef max
//#endif
//                        resultData.avg += _latency;
//                        resultData.best = std::min(resultData.best, _latency);
//                        resultData.worst = std::max(resultData.worst, _latency);
//                    }
//                }
//                if (resultData.totalCount != 0 && resultData.failedCount != 0)
//                {
//                    resultData.errorMessage.clear();
//                    resultData.avg = resultData.avg / (resultData.totalCount - resultData.failedCount) / 1000;
//                }
//                else
//                {
//                    resultData.avg = LATENCY_TEST_VALUE_ERROR;
//                    LOG(MODULE_NETWORK, resultData.errorMessage)
//                }
//                //
//                //
//                break;
//            }
//            case TCPING:
//            default:
//            {
//                this->resultData = tcping::TestTCPLatency(host, port, count);
//                break;
//            }
//        }
    }
} // namespace Qv2ray::components::latency
