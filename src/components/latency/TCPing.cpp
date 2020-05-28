#include "TCPing.hpp"

#ifdef _WIN32
    #include <WS2tcpip.h>
    #include <WinSock2.h>
#else
    #include <fcntl.h>
    #include <netdb.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <unistd.h>
#endif

namespace Qv2ray::components::latency::tcping
{

#ifdef Q_OS_WIN
    using qv_socket_t = SOCKET;
#else
    using qv_socket_t = int;
#endif

    inline int setnonblocking(qv_socket_t sockno, int &opt)
    {
#ifdef _WIN32
        ULONG block = 1;
        if (ioctlsocket(sockno, FIONBIO, &block) == SOCKET_ERROR)
        {
            return -1;
        }
#else
        if ((opt = fcntl(sockno, F_GETFL, NULL)) < 0)
        {
            // get socket flags
            return -1;
        }
        if (fcntl(sockno, F_SETFL, opt | O_NONBLOCK) < 0)
        {
            // set socket non-blocking
            return -1;
        }
#endif
        return 0;
    }

    inline int setblocking(qv_socket_t sockno, int &opt)
    {
#ifdef _WIN32
        ULONG block = 0;
        if (ioctlsocket(sockno, FIONBIO, &block) == SOCKET_ERROR)
        {
            return -1;
        }
#else
        if (fcntl(sockno, F_SETFL, opt) < 0)
        {
            // reset socket flags
            return -1;
        }
#endif
        return 0;
    }

    int connect_wait(qv_socket_t sockno, struct sockaddr *addr, size_t addrlen, int timeout_sec = 5)
    {
        int res;
        int opt;
        timeval tv = { 0, 0 };
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;
        if ((res = setnonblocking(sockno, opt)) != 0)
            return -1;
        if ((res = ::connect(sockno, addr, addrlen)) < 0)
        {
#ifdef _WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
#else
            if (errno == EINPROGRESS)
            {
#endif
                // connecting
                fd_set wait_set;
                FD_ZERO(&wait_set);
                FD_SET(sockno, &wait_set);
                res = select(sockno + 1, NULL, &wait_set, NULL, &tv);
            }
        }
        else
            // connect immediately
            res = 1;

        if (setblocking(sockno, opt) != 0)
            return -1;

        if (res < 0)
            // an error occured
            return -1;
        else if (res == 0)
            // timeout
            return 1;
        else
        {
            socklen_t len = sizeof(opt);
            if (getsockopt(sockno, SOL_SOCKET, SO_ERROR, (char *) (&opt), &len) < 0)
                return -1;
        }
        return 0;
    }

    int resolveHost(const QString &host, int port, addrinfo **res)
    {
        addrinfo hints;
#ifdef Q_OS_WIN
        WSADATA wsadata;
        WSAStartup(0x0202, &wsadata);
        memset(&hints, 0, sizeof(struct addrinfo));
#else
        hints.ai_flags = AI_NUMERICSERV;
#endif
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        return getaddrinfo(host.toStdString().c_str(), std::to_string(port).c_str(), &hints, res);
    }

    int testLatency(struct addrinfo *addr, system_clock::time_point *start, system_clock::time_point *end)
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

            // create socket
            if (!(fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)))
            {
                goto next_addr0;
            }

            // Windows needs special conversion.
#ifdef _WIN32
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on)) < 0)
#else
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
#endif
                goto next_addr1;
            *start = system_clock::now();
            if (connect_wait(fd, addr->ai_addr, addr->ai_addrlen) == 0)
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

    LatencyTestResult TestTCPLatency(const QString &host, int port, int testCount)
    {
        LatencyTestResult data;
        int successCount = 0;
        addrinfo *resolved;
        int errcode;

        if ((errcode = resolveHost(host, port, &resolved)) != 0)
        {
#ifdef Q_OS_WIN
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

        while (currentCount < testCount)
        {
            system_clock::time_point start;
            system_clock::time_point end;

            if ((errcode = testLatency(resolved, &start, &end)) != 0)
            {
                if (errcode != -EADDRNOTAVAIL)
                {
                    LOG(MODULE_NETWORK, "error connecting to host: " + host + ":" + QSTRN(port) + " " + strerror(-errcode))
                }
                else
                {
                    LOG(MODULE_NETWORK, noAddress ? "no address" : "error connecting to host: " + QSTRN(-errcode) + " " + strerror(-errcode))
                    noAddress = true;
                }
            }
            else
            {
                noAddress = false;
                successCount++;
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                long ms = milliseconds.count();
                data.avg += ms;
#ifdef Q_OS_WIN
                // Is it Windows?
    #undef min
    #undef max
#endif
                data.worst = std::min(data.worst, ms);
                data.best = std::max(data.best, ms);

                if (ms > 1000)
                {
                    LOG(MODULE_NETWORK, "Stop the test on the first long connect()")
                    break; /* Stop the test on the first long connect() */
                }
            }

            currentCount++;
            QThread::msleep(200);
        }
        freeaddrinfo(resolved);
        if (successCount > 0)
        {
            data.avg = data.avg / successCount;
        }
        else
        {
            data.avg = LATENCY_TEST_VALUE_ERROR;
            data.worst = LATENCY_TEST_VALUE_ERROR;
            data.best = LATENCY_TEST_VALUE_ERROR;
            data.errorMessage = QObject::tr("Timeout");
        }
        return data;
    }
} // namespace Qv2ray::components::latency::tcping
