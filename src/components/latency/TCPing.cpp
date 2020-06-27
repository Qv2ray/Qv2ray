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
    struct TranslateWSAError{
        ~TranslateWSAError(){
            errno = translate_sys_error(WSAGetLastError());
        }
        int translate_sys_error(int sys_errno) {
            LOG(MODULE_NETWORK, "translate_sys_error:WSAGetLastError()==" +  QSTRN(sys_errno))
            switch (sys_errno) {
                case ERROR_NOACCESS:                    return EACCES;
                case WSAEACCES:                         return EACCES;
                case ERROR_ELEVATION_REQUIRED:          return EACCES;
                case ERROR_CANT_ACCESS_FILE:            return EACCES;
                case ERROR_ADDRESS_ALREADY_ASSOCIATED:  return EADDRINUSE;
                case WSAEADDRINUSE:                     return EADDRINUSE;
                case WSAEADDRNOTAVAIL:                  return EADDRNOTAVAIL;
                case WSAEAFNOSUPPORT:                   return EAFNOSUPPORT;
                case WSAEWOULDBLOCK:                    return EAGAIN;
                case WSAEALREADY:                       return EALREADY;
                case ERROR_INVALID_FLAGS:               return EBADF;
                case ERROR_INVALID_HANDLE:              return EBADF;
                case ERROR_LOCK_VIOLATION:              return EBUSY;
                case ERROR_PIPE_BUSY:                   return EBUSY;
                case ERROR_SHARING_VIOLATION:           return EBUSY;
                case ERROR_OPERATION_ABORTED:           return ECANCELED;
                case WSAEINTR:                          return ECANCELED;
                                                        /*case ERROR_NO_UNICODE_TRANSLATION:      return ECHARSET;*/
                case ERROR_CONNECTION_ABORTED:          return ECONNABORTED;
                case WSAECONNABORTED:                   return ECONNABORTED;
                case ERROR_CONNECTION_REFUSED:          return ECONNREFUSED;
                case WSAECONNREFUSED:                   return ECONNREFUSED;
                case ERROR_NETNAME_DELETED:             return ECONNRESET;
                case WSAECONNRESET:                     return ECONNRESET;
                case ERROR_ALREADY_EXISTS:              return EEXIST;
                case ERROR_FILE_EXISTS:                 return EEXIST;
                case ERROR_BUFFER_OVERFLOW:             return EFAULT;
                case WSAEFAULT:                         return EFAULT;
                case ERROR_HOST_UNREACHABLE:            return EHOSTUNREACH;
                case WSAEHOSTUNREACH:                   return EHOSTUNREACH;
                case ERROR_INSUFFICIENT_BUFFER:         return EINVAL;
                case ERROR_INVALID_DATA:                return EINVAL;
                case ERROR_INVALID_PARAMETER:           return EINVAL;
                case ERROR_SYMLINK_NOT_SUPPORTED:       return EINVAL;
                case WSAEINVAL:                         return EINVAL;
                case WSAEPFNOSUPPORT:                   return EINVAL;
                case WSAESOCKTNOSUPPORT:                return EINVAL;
                case ERROR_BEGINNING_OF_MEDIA:          return EIO;
                case ERROR_BUS_RESET:                   return EIO;
                case ERROR_CRC:                         return EIO;
                case ERROR_DEVICE_DOOR_OPEN:            return EIO;
                case ERROR_DEVICE_REQUIRES_CLEANING:    return EIO;
                case ERROR_DISK_CORRUPT:                return EIO;
                case ERROR_EOM_OVERFLOW:                return EIO;
                case ERROR_FILEMARK_DETECTED:           return EIO;
                case ERROR_GEN_FAILURE:                 return EIO;
                case ERROR_INVALID_BLOCK_LENGTH:        return EIO;
                case ERROR_IO_DEVICE:                   return EIO;
                case ERROR_NO_DATA_DETECTED:            return EIO;
                case ERROR_NO_SIGNAL_SENT:              return EIO;
                case ERROR_OPEN_FAILED:                 return EIO;
                case ERROR_SETMARK_DETECTED:            return EIO;
                case ERROR_SIGNAL_REFUSED:              return EIO;
                case WSAEISCONN:                        return EISCONN;
                case ERROR_CANT_RESOLVE_FILENAME:       return ELOOP;
                case ERROR_TOO_MANY_OPEN_FILES:         return EMFILE;
                case WSAEMFILE:                         return EMFILE;
                case WSAEMSGSIZE:                       return EMSGSIZE;
                case ERROR_FILENAME_EXCED_RANGE:        return ENAMETOOLONG;
                case ERROR_NETWORK_UNREACHABLE:         return ENETUNREACH;
                case WSAENETUNREACH:                    return ENETUNREACH;
                case WSAENOBUFS:                        return ENOBUFS;
                case ERROR_BAD_PATHNAME:                return ENOENT;
                case ERROR_DIRECTORY:                   return ENOENT;
                case ERROR_ENVVAR_NOT_FOUND:            return ENOENT;
                case ERROR_FILE_NOT_FOUND:              return ENOENT;
                case ERROR_INVALID_NAME:                return ENOENT;
                case ERROR_INVALID_DRIVE:               return ENOENT;
                case ERROR_INVALID_REPARSE_DATA:        return ENOENT;
                case ERROR_MOD_NOT_FOUND:               return ENOENT;
                case ERROR_PATH_NOT_FOUND:              return ENOENT;
                case WSAHOST_NOT_FOUND:                 return ENOENT;
                case WSANO_DATA:                        return ENOENT;
                case ERROR_NOT_ENOUGH_MEMORY:           return ENOMEM;
                case ERROR_OUTOFMEMORY:                 return ENOMEM;
                case ERROR_CANNOT_MAKE:                 return ENOSPC;
                case ERROR_DISK_FULL:                   return ENOSPC;
                case ERROR_EA_TABLE_FULL:               return ENOSPC;
                case ERROR_END_OF_MEDIA:                return ENOSPC;
                case ERROR_HANDLE_DISK_FULL:            return ENOSPC;
                case ERROR_NOT_CONNECTED:               return ENOTCONN;
                case WSAENOTCONN:                       return ENOTCONN;
                case ERROR_DIR_NOT_EMPTY:               return ENOTEMPTY;
                case WSAENOTSOCK:                       return ENOTSOCK;
                case ERROR_NOT_SUPPORTED:               return ENOTSUP;
                case ERROR_BROKEN_PIPE:                 return EOF;
                case ERROR_ACCESS_DENIED:               return EPERM;
                case ERROR_PRIVILEGE_NOT_HELD:          return EPERM;
                case ERROR_BAD_PIPE:                    return EPIPE;
                case ERROR_NO_DATA:                     return EPIPE;
                case ERROR_PIPE_NOT_CONNECTED:          return EPIPE;
                case WSAESHUTDOWN:                      return EPIPE;
                case WSAEPROTONOSUPPORT:                return EPROTONOSUPPORT;
                case ERROR_WRITE_PROTECT:               return EROFS;
                case ERROR_SEM_TIMEOUT:                 return ETIMEDOUT;
                case WSAETIMEDOUT:                      return ETIMEDOUT;
                case ERROR_NOT_SAME_DEVICE:             return EXDEV;
                case ERROR_INVALID_FUNCTION:            return EISDIR;
                case ERROR_META_EXPANSION_TOO_LONG:     return E2BIG;
                default:                                return EIO;/*unknown*/
            }
        }
    };
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
#ifdef _WIN32
        TranslateWSAError _translateWSAError_;
#endif
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
        {
            // timeout
            errno = ETIMEDOUT;
            return 1;
        }
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

        return -1;
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
               LOG(MODULE_NETWORK, "error connecting to host: " + host + ":" + QSTRN(port) + " " + strerror(errno))
            }
            else
            {
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
