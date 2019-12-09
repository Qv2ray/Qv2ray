#include "QvTCPing.hpp"
#include "QtConcurrent/QtConcurrent"

namespace Qv2ray
{
    namespace Components
    {
        QvTCPingModel::QvTCPingModel(int defaultCount, QObject *parent) : QObject(parent)
        {
            count = defaultCount;
        }
        void QvTCPingModel::StartPing(const QString &connectionName, const QString &hostName, int port)
        {
            QvTCPingData data;
            data.hostName = hostName;
            data.port = port;
            data.connectionName = connectionName;
            auto watcher = new QFutureWatcher<QvTCPingData>(this);
            DEBUG(MODULE_NETWORK, "Start Ping: " + hostName.toStdString() + ":" + to_string(port))
            watcher->setFuture(QtConcurrent::run(&QvTCPingModel::startTestLatency, data, count));
            pingWorkingThreads.enqueue(watcher);
            connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
                this->pingWorkingThreads.removeOne(watcher);
                auto result = watcher->result();
                DEBUG(MODULE_NETWORK, "Ping finished: " + result.hostName.toStdString() + ":" + to_string(result.port) + " --> " + to_string(result.avg) + "ms")

                if (!result.errorMessage.isEmpty()) {
                    LOG(MODULE_NETWORK, "Ping --> " + result.errorMessage.toStdString())
                }

                emit this->PingFinished(result);
            });
        }

        QvTCPingData QvTCPingModel::startTestLatency(QvTCPingData data, const int count)
        {
            double successCount = 0, errorCount = 0;
            addrinfo *resolved;
            int errcode;

            if ((errcode = resolveHost(data.hostName.toStdString(), data.port, &resolved)) != 0) {
#if defined (__WIN32) && defined (UNICODE)
                data.errorMessage = QString::fromStdWString(gai_strerror(errcode));
#else
                data.errorMessage = QSTRING(gai_strerror(errcode));
#endif
                return data;
            }

            bool noAddress = false;
            int currentCount = 0;

            while (currentCount < count) {
                timeval rtt;

                if ((errcode = testLatency(resolved, &rtt)) != 0) {
                    if (errcode != -EADDRNOTAVAIL) {
                        LOG(MODULE_NETWORK, "Error connecting to host " + data.hostName.toStdString() + ":" + to_string(data.port) + strerror(-errcode))
                        errorCount++;
                    } else {
                        if (noAddress) {
                            LOG(MODULE_NETWORK, ".")
                        } else {
                            LOG(MODULE_NETWORK, "error connecting to host:" + to_string(-errcode) + strerror(-errcode))
                        }

                        noAddress = true;
                    }
                } else {
                    noAddress = false;
                    successCount++;
                    double ms = (rtt.tv_sec * 1000.0) + (rtt.tv_usec / 1000.0);
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

        int QvTCPingModel::resolveHost(const string &host, int port, addrinfo **res)
        {
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

        int QvTCPingModel::testLatency(struct addrinfo *addr, struct timeval *rtt)
        {
            int fd;
            struct timeval start;
            int connect_result;
            const int on = 1;
            /* int flags; */
            int rv = 0;

            /* try to connect for each of the entries: */
            while (addr != nullptr) {
                /* create socket */
                if ((fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == -1)
                    goto next_addr0;

#ifdef _WIN32

                // Windows needs special conversion.
                if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) < 0)
#else
                if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
#endif
                    goto next_addr1;

                if (gettimeofday(&start, nullptr) == -1)
                    goto next_addr1;

                /* connect to peer */
                // Qt has its own connect() function in QObject....
                // So we add "::" here.s
                if ((connect_result = ::connect(fd, addr->ai_addr, addr->ai_addrlen)) == 0) {
                    if (gettimeofday(rtt, nullptr) == -1)
                        goto next_addr1;

                    rtt->tv_sec = rtt->tv_sec - start.tv_sec;
                    rtt->tv_usec = rtt->tv_usec - start.tv_usec;
                    close(fd);
                    return 0;
                }

next_addr1:
                close(fd);
next_addr0:
                addr = addr->ai_next;
            }

            rv = rv ? rv : -errno;
            return rv;
        }
    }
}
