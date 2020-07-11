/* Author: Maciek Muszkowski
 *
 * This is a simple ping implementation for Linux.
 * It will work ONLY on kernels 3.x+ and you need
 * to set allowed groups in /proc/sys/net/ipv4/ping_group_range */
#include "ICMPPing.hpp"

#include <QObject>
#ifdef Q_OS_UNIX
    #include <unistd.h>
    //
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <netinet/ip_icmp.h>
    #include <resolv.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #ifdef Q_OS_MAC
        #define SOL_IP 0
    #endif

namespace Qv2ray::components::latency::icmping
{
    /// 1s complementary checksum
    uint16_t ping_checksum(const char *buf, size_t size)
    {
        size_t i;
        uint64_t sum = 0;

        for (i = 0; i < size; i += 2)
        {
            sum += *(uint16_t *) buf;
            buf += 2;
        }
        if (size - i > 0)
        {
            sum += *(uint8_t *) buf;
        }

        while ((sum >> 16) != 0)
        {
            sum = (sum & 0xffff) + (sum >> 16);
        }

        return (uint16_t) ~sum;
    }
    void ICMPPing::deinit()
    {
        if (socketId >= 0)
        {
            close(socketId);
            socketId = -1;
        }
    }

    ICMPPing::ICMPPing(int ttl)
    {
        auto timeout_s = 5;
        // create socket
        if ( //((sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) &&
            ((socketId = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0))
        {
            initErrorMessage = "EPING_SOCK: " + QObject::tr("Socket creation failed");
            return;
        }
        // set TTL
        if (setsockopt(socketId, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) != 0)
        {
            deinit();
            initErrorMessage = "EPING_TTL: " + QObject::tr("Failed to setup TTL value");
            return;
        }

        // set timeout in secs (do not use secs - BUGGY)
        timeval timeout;
        timeout.tv_sec = timeout_s;
        timeout.tv_usec = 0;
        if (setsockopt(socketId, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) != 0)
        {
            deinit();
            initErrorMessage = "EPING_SETTO: " + QObject::tr("Setting timeout failed");
            return;
        }
        initialized = true;
    }

    /// @return value < 0 on error, response time in ms on success
    QPair<int64_t, QString> ICMPPing::ping(const QString &address)
    {
        if (!initialized)
        {
            return { 0, initErrorMessage };
        }
        timeval start, end;
        socklen_t slen;
        // not initialized
        if (socketId < 0)
            return { 0, "EPING_SOCK:" + QObject::tr("Socket creation failed") };

        // resolve hostname
        hostent *resolvedAddress = gethostbyname(address.toStdString().c_str());
        if (!resolvedAddress)
            return { 0, "EPING_HOST: " + QObject::tr("Unresolvable hostname") };

        // set IP address to ping
        sockaddr_in targetAddress;
        memset(&targetAddress, 0, sizeof(targetAddress));
        targetAddress.sin_family = resolvedAddress->h_addrtype;
        targetAddress.sin_port = 0;

        auto src = resolvedAddress->h_addr;
        if (src == nullptr)
        {
            // Buggy GCC detected
            return { 0, "GCC: COMPILER BUG. Cannot even dereference a char**" };
        }

        memcpy(&targetAddress.sin_addr, src, resolvedAddress->h_length);
        // prepare echo request packet
        icmp _icmp_request;
        memset(&_icmp_request, 0, sizeof(_icmp_request));
        _icmp_request.icmp_type = ICMP_ECHO;
        _icmp_request.icmp_hun.ih_idseq.icd_id = 0; // SOCK_DGRAM & 0 => id will be set by kernel
        unsigned short sent_seq;
        _icmp_request.icmp_hun.ih_idseq.icd_seq = sent_seq = seq++;
        _icmp_request.icmp_cksum = ping_checksum(reinterpret_cast<char *>(&_icmp_request), sizeof(_icmp_request));

        // send echo request
        gettimeofday(&start, NULL);
        if (sendto(socketId, &_icmp_request, sizeof(icmp), 0, (struct sockaddr *) &targetAddress, sizeof(targetAddress)) <= 0)
            return { 0, "EPING_SEND: " + QObject::tr("Sending echo request failed") };

        // receive response (if any)
        sockaddr_in remove_addr;
        slen = sizeof(remove_addr);
        int rlen;
        icmp resp;
        while ((rlen = recvfrom(socketId, &resp, sizeof(icmp), 0, (struct sockaddr *) &remove_addr, &slen)) > 0)
        {
            gettimeofday(&end, NULL);

            // skip malformed
            if (rlen != sizeof(icmp))
                continue;

            // skip the ones we didn't send
            if (resp.icmp_hun.ih_idseq.icd_seq != sent_seq)
                continue;

            switch (resp.icmp_type)
            {
                case ICMP_ECHOREPLY: return { 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec), {} };
                case ICMP_UNREACH: return { 0, "EPING_DST: " + QObject::tr("Destination unreachable") };
                case ICMP_TIMXCEED: return { 0, "EPING_TIME: " + QObject::tr("Timeout") };
                default: return { 0, "EPING_UNK: " + QObject::tr("Unknown error") };
            }
        }
        return { 0, "EPING_TIME: " + QObject::tr("Timeout") };
    }
} // namespace Qv2ray::components::latency::icmping
#endif
