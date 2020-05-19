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
                    const auto pair = ping.ping(host);
                    const auto &errMessage = pair.second;
                    const long _latency = pair.first;
                    if (!errMessage.isEmpty())
                    {
                        resultData.errorMessage.append(NEWLINE + errMessage);
                        resultData.failedCount++;
                    }
                    else
                    {
#ifdef Q_OS_WIN
                        // Is it Windows?
    #undef min
    #undef max
#endif
                        resultData.avg += _latency;
                        resultData.best = std::min(resultData.best, _latency);
                        resultData.worst = std::max(resultData.worst, _latency);
                    }
                }
                if (resultData.totalCount != resultData.failedCount != 0)
                {
                    resultData.errorMessage.clear();
                    resultData.avg = resultData.avg / (resultData.totalCount - resultData.failedCount) / 1000;
                }
                else
                {
                    resultData.avg = LATENCY_TEST_VALUE_ERROR;
                    LOG(MODULE_NETWORK, resultData.errorMessage)
                }
                //
                //
                break;
            }
        }
    }
} // namespace Qv2ray::components::latency
