#include "LatencyTestThread.hpp"

#include "TCPing.hpp"

#ifdef Q_OS_UNIX
    #include "unix/ICMPPing.hpp"
#else
    #include "win/ICMPPing.hpp"
#endif

namespace Qv2ray::components::latency
{

    LatencyTestThread::LatencyTestThread(const QString &host, int port, Qv2rayLatencyTestingMethod method, int count, QObject *parent)
        : QThread(parent)
    {
        this->count = count;
        this->host = host;
        this->port = port;
        this->method = method;
        this->resultData = {};
        this->resultData.method = method;
    }
    void LatencyTestThread::run()
    {
        resultData.avg = 0;
        resultData.best = 0;
        resultData.worst = 0;
        switch (method)
        {
            case TCPING:
            {
                this->resultData = tcping::TestTCPLatency(host, port, count);
                break;
            }
            case ICMPING:
            {
                icmping::ICMPPing ping(30);
                for (auto i = 0; i < count; i++)
                {
                    resultData.totalCount++;
                    const auto &[latency, errMessage] = ping.ping(host);
                    if (!errMessage.isEmpty())
                    {
                        resultData.errorMessage.append(NEWLINE + errMessage);
                        resultData.failedCount++;
                    }
                    else
                    {
                        resultData.avg += latency;
                        resultData.best = std::min(resultData.best, latency);
                        resultData.worst = std::max(resultData.worst, latency);
                    }
                }
                resultData.avg = resultData.avg / (resultData.totalCount - resultData.failedCount) / 1000;
                break;
            }
        }
    }
} // namespace Qv2ray::components::latency
