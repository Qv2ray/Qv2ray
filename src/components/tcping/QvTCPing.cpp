#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <unistd.h>
#endif
#include "QvTCPing.hpp"
#include "QtConcurrent/QtConcurrent"

namespace Qv2ray::components::tcping
{
    static int resolveHost(const std::string &host, int portnr, struct addrinfo **res);
    static int testLatency(struct addrinfo *addr, std::chrono::system_clock::time_point *start, std::chrono::system_clock::time_point *end);
    //
    QvTCPingModel::QvTCPingModel(int defaultCount, QObject *parent) : QObject(parent)
    {
        count = defaultCount;
    }
    void QvTCPingModel::StopAllPing()
    {
        while (!pingWorkingThreads.isEmpty()) {
            auto worker = pingWorkingThreads.dequeue();
            worker->future().cancel();
            worker->cancel();
        }
    }
    void QvTCPingModel::StartPing(const QvConnectionObject &connectionName, const QString &hostName, int port)
    {
        QvTCPingData data;
        data.hostName = hostName;
        data.port = port;
        data.connectionIdentifier = connectionName;
        auto watcher = new QFutureWatcher<QvTCPingData>(this);
        DEBUG(NETWORK, "Start Ping: " + hostName + ":" + QSTRN(port))
        watcher->setFuture(QtConcurrent::run(&QvTCPingModel::startTestLatency, data, count));
        pingWorkingThreads.enqueue(watcher);
        connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
            this->pingWorkingThreads.removeOne(watcher);
            auto result = watcher->result();
            DEBUG(NETWORK, "Ping finished: " + result.hostName + ":" + QSTRN(result.port) + " --> " + QSTRN(result.avg) + "ms")

            if (!result.errorMessage.isEmpty()) {
                LOG(NETWORK, "Ping --> " + result.errorMessage)
            }

            emit this->PingFinished(result);
        });
    }

    QvTCPingData QvTCPingModel::startTestLatency(QvTCPingData data, const int count)
    {
        if (isExiting) return QvTCPingData();

        double successCount = 0, errorCount = 0;
        addrinfo *resolved;
        int errcode;

        if ((errcode = resolveHost(data.hostName.toStdString(), data.port, &resolved)) != 0) {
#ifdef _WIN32
            data.errorMessage = QString::fromStdWString(gai_strerror(errcode));
#else
            data.errorMessage = gai_strerror(errcode);
#endif
            return data;
        }

        bool noAddress = false;
        int currentCount = 0;

        while (currentCount < count) {
            if (isExiting) return QvTCPingData();

            std::chrono::system_clock::time_point start;
            std::chrono::system_clock::time_point end;

            if ((errcode = testLatency(resolved, &start, &end)) != 0) {
                if (errcode != -EADDRNOTAVAIL) {
                    LOG(NETWORK, "Error connecting to host: " + data.hostName + ":" + QSTRN(data.port) + " " + strerror(-errcode))
                    errorCount++;
                } else {
                    if (noAddress) {
                        LOG(NETWORK, ".")
                    } else {
                        LOG(NETWORK, "error connecting to host: " + QSTRN(-errcode) + " " + strerror(-errcode))
                    }

                    noAddress = true;
                }
            } else {
                noAddress = false;
                successCount++;
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                double ms = milliseconds.count();
                ms = ms < 0 ? 0 : ms;
                data.avg += ms;
                data.min = min(data.min, ms);
                data.max = max(data.max, ms);

                if (ms > 1000) {
                    break; /* Stop the test on the first long connect() */
                }
            }

            currentCount++;
            QThread::msleep(500);
        }

        data.avg = data.avg / successCount;
        freeaddrinfo(resolved);
        return data;
    }

    int resolveHost(const string &host, int port, addrinfo **res)
    {
        if (isExiting) return 0;

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
        if (isExiting) return 0;

#ifdef _WIN32
        SOCKET fd;
#else
        int fd;
#endif
        int connect_result;
        const int on = 1;
        /* int flags; */
        int rv = 0;

        /* try to connect for each of the entries: */
        while (addr != nullptr) {
            if (isExiting) return 0;

            /* create socket */
            fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

            if (!fd) {
                goto next_addr0;
            }

#ifdef _WIN32

            // Windows needs special conversion.
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) < 0)
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
            connect_result = ::connect(fd, addr->ai_addr, addr->ai_addrlen);

            if (connect_result == 0) {
                *end = system_clock::now();
#ifdef _WIN32
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
}
