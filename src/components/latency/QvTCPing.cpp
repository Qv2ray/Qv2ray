#ifdef _WIN32
    #include <WS2tcpip.h>
    #include <WinSock2.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <sys/time.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif
#include "QtConcurrent/QtConcurrent"
#include "QvTCPing.hpp"
#include "core/handler/ConfigHandler.hpp"


#ifdef _WIN32
    using qv_socket_t=SOCKET;
#else
    using qv_socket_t=int;
#endif
namespace
{
    inline int setnonblocking(qv_socket_t sockno,int &opt)
    {
#ifdef _WIN32
        ULONG block = 1;
        if (ioctlsocket(sockno, FIONBIO, &block) == SOCKET_ERROR)
        {
            return -1;
        }
#else
        if ((opt = fcntl (sockno, F_GETFL, NULL)) < 0) {
            //get socket flags
            return -1;
        }
        if (fcntl (sockno, F_SETFL, opt | O_NONBLOCK) < 0) {
            //set socket non-blocking
            return -1;
        }
#endif
        return 0;
    }

    inline int setblocking(qv_socket_t sockno,int &opt)
    {
#ifdef _WIN32
        ULONG block = 0;
        if (ioctlsocket(sockno, FIONBIO, &block) == SOCKET_ERROR)
        {
            return -1;
        }
#else
        if (fcntl (sockno, F_SETFL, opt) < 0) {
            //reset socket flags
            return -1;
        }
#endif
        return 0;
    }


    int connect_wait (
            qv_socket_t sockno,
            struct sockaddr * addr,
            size_t addrlen,
            int timeout_sec=5)
    {
        int res;
        int opt;
        timeval tv = {0};
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;
        if ((res = setnonblocking(sockno,opt))!=0)
            return -1;
        if ((res = ::connect (sockno, addr, addrlen)) < 0) {
#ifdef _WIN32
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
            if (errno == EINPROGRESS) {
#endif
                //connecting
                fd_set wait_set;
                FD_ZERO (&wait_set);
                FD_SET (sockno, &wait_set);
                res = select (sockno + 1, NULL, &wait_set, NULL, &tv);
            }
        }
        else {
            //connect immediately
            res = 1;
        }
        if (setblocking(sockno,opt)!=0){
            return -1;
        }
        if (res < 0) {
            //an error occured
            return -1;
        }
        else if (res == 0) {
            //timeout
            return 1;
        }
        else {
            socklen_t len = sizeof (opt);
            if (getsockopt (sockno, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&opt), &len) < 0) {
                return -1;
            }
        }
        return 0;
    }
}

namespace Qv2ray::components::tcping
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
                data.worst = std::min(data.worst, ms);
                data.best = std::max(data.best, ms);

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
        data.avg = std::min(data.avg, QVTCPING_VALUE_ERROR);
        data.worst = std::min(data.worst, QVTCPING_VALUE_ERROR);
        data.best = std::min(data.best, QVTCPING_VALUE_ERROR);
        return data;
    }

    int resolveHost(const std::string &host, int port, addrinfo **res)
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
        return getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, res);
    }

    int testLatency(struct addrinfo *addr, std::chrono::system_clock::time_point *start, std::chrono::system_clock::time_point *end)
    {
        qv_socket_t fd;

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
            if (connect_wait(fd, addr->ai_addr, addr->ai_addrlen)==0)
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
} // namespace Qv2ray::components::tcping
