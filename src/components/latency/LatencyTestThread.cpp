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
            case ICMPING:
            {
                icmping::ICMPPing pingHelper(30);
                for (auto i = 0; i < count; i++)
                {
                    resultData.totalCount++;
                    const auto value = pingHelper.ping(host);
                    const auto _latency = value.first;
                    const auto errMessage = value.second;
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
#define _qvmin_(x, y) ((x) < (y) ? (x) : (y))
#define _qvmax_(x, y) ((x) > (y) ? (x) : (y))
                        resultData.best = _qvmin_(resultData.best, _latency);
                        resultData.worst = _qvmax_(resultData.worst, _latency);
#undef _qvmax_
#undef _qvmin_
                    }
                }
                if (resultData.totalCount > 0 && resultData.failedCount != resultData.totalCount)
                {
                    resultData.errorMessage.clear();
                    // ms to s
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
            case TCPING:
            default:
            {
                this->resultData = tcping::TestTCPLatency(host, port, count);
                break;
            }
        }
    }
} // namespace Qv2ray::components::latency
