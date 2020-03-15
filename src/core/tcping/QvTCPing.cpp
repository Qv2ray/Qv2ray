#ifdef _WIN32
    #include <WS2tcpip.h>
    #include <WinSock2.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <unistd.h>
#endif
#include "QtConcurrent/QtConcurrent"
#include "QvTCPing.hpp"
#include "core/handler/ConfigHandler.hpp"

namespace Qv2ray::core::tcping
{
    static int resolveHost(const std::string &host, int portnr, struct addrinfo **res);
    static int testLatency(struct addrinfo *addr, std::chrono::system_clock::time_point *start, std::chrono::system_clock::time_point *end);
    //
    QvTCPingHelper::QvTCPingHelper(const int defaultCount, QObject *parent) : QObject(parent)
    {
        count = defaultCount;
    }

    void QvTCPingHelper::StopAllLatenceTest()
    {
        while (!pingWorkingThreads.isEmpty())
        {
            auto worker = pingWorkingThreads.dequeue();
            worker->future().cancel();
            worker->cancel();
        }
    }
    void QvTCPingHelper::TestLatency(const ConnectionId &id)
    {
        auto watcher = new QFutureWatcher<QvTCPingResultObject>(this);
        watcher->setFuture(QtConcurrent::run(&QvTCPingHelper::TestLatency_p, id, count));
        pingWorkingThreads.enqueue(watcher);
        //
        connect(watcher, &QFutureWatcher<QvTCPingResultObject>::finished, this, [=]() {
            auto result = watcher->result();
            this->pingWorkingThreads.removeOne(watcher);

            if (!result.errorMessage.isEmpty())
            {
                LOG(MODULE_NETWORK, "Ping --> " + result.errorMessage)
                result.avg = QVTCPING_VALUE_ERROR;
                result.best = QVTCPING_VALUE_ERROR;
                result.worst = QVTCPING_VALUE_ERROR;
            }

            emit this->OnLatencyTestCompleted(result);
        });
    }

    QvTCPingResultObject QvTCPingHelper::TestLatency_p(const ConnectionId &id, const int count)
    {
        QvTCPingResultObject data;
        data.connectionId = id;

        if (isExiting)
            return data;

        auto [protocol, host, port] = GetConnectionInfo(id);
        Q_UNUSED(protocol)
        double successCount = 0, errorCount = 0;
        addrinfo *resolved;
        int errcode;

        if ((errcode = resolveHost(host.toStdString(), port, &resolved)) != 0)
        {
#ifdef _WIN32
            data.errorMessage = QString::fromStdWString(gai_strerror(errcode));
#else
            data.errorMessage = gai_strerror(errcode);
#endif
            return data;
        }

        bool noAddress = false;
        int currentCount = 0;

        data.avg = 0;
        data.worst = 0;
        data.best = 0;

        while (currentCount < count)
        {
            system_clock::time_point start;
            system_clock::time_point end;

            if ((errcode = testLatency(resolved, &start, &end)) != 0)
            {
                if (errcode != -EADDRNOTAVAIL)
                {
                    LOG(MODULE_NETWORK, "error connecting to host: " + host + ":" + QSTRN(port) + " " + strerror(-errcode))
                    errorCount++;
                }
                else
                {
                    if (noAddress)
                    {
                        LOG(MODULE_NETWORK, "no address error")
                    }
                    else
                    {
                        LOG(MODULE_NETWORK, "error connecting to host: " + QSTRN(-errcode) + " " + strerror(-errcode))
                    }

                    noAddress = true;
                }
            }
            else
            {
                noAddress = false;
                successCount++;
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                uint ms = milliseconds.count();
                data.avg += ms;
                data.worst = min(data.worst, ms);
                data.best = max(data.best, ms);

                if (ms > 1000)
                {
                    LOG(MODULE_NETWORK, "Stop the test on the first long connect()")
                    break; /* Stop the test on the first long connect() */
                }
            }

            currentCount++;
            QThread::msleep(500);
        }

        data.avg = data.avg / successCount;
        freeaddrinfo(resolved);
        data.avg = min(data.avg, QVTCPING_VALUE_ERROR);
        data.worst = min(data.worst, QVTCPING_VALUE_ERROR);
        data.best = min(data.best, QVTCPING_VALUE_ERROR);
        return data;
    }

    int resolveHost(const string &host, int port, addrinfo **res)
    {
        if (isExiting)
            return 0;

        addrinfo hints;
#ifdef _WIN32
        WSADATA wsadata;
        WSAStartup(0x0202, &wsadata);
        memset(&hints, 0, sizeof(struct addrinfo));
#else
        hints.ai_flags = AI_NUMERICSERV;
#endif
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        return getaddrinfo(host.c_str(), to_string(port).c_str(), &hints, res);
    }

    int testLatency(struct addrinfo *addr, std::chrono::system_clock::time_point *start, std::chrono::system_clock::time_point *end)
    {
#ifdef _WIN32
        SOCKET fd;
#else
        int fd;
#endif
        const int on = 1;
        /* int flags; */
        int rv = 0;

        /* try to connect for each of the entries: */
        while (addr != nullptr)
        {
            if (isExiting)
                return 0;

            /* create socket */
            fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

            if (!fd)
            {
                goto next_addr0;
            }

#ifdef _WIN32

            // Windows needs special conversion.
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on)) < 0)
#else
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
#endif
            {
                goto next_addr1;
            }

            *start = system_clock::now();

            /* connect to peer */
            // Qt has its own connect() function in QObject....
            // So we add "::" here
            if (::connect(fd, addr->ai_addr, addr->ai_addrlen) == 0)
            {
                *end = system_clock::now();
#ifdef Q_OS_WIN
                closesocket(fd);
#else
                close(fd);
#endif
                return 0;
            }

        next_addr1:
#ifdef _WIN32
            closesocket(fd);
#else
            close(fd);
#endif
        next_addr0:
            addr = addr->ai_next;
        }

        rv = rv ? rv : -errno;
        return rv;
    }
} // namespace Qv2ray::core::tcping
