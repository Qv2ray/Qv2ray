/* Author: Maciek Muszkowski
 *
 * This is a simple ping implementation for Linux.
 * It will work ONLY on kernels 3.x+ and you need
 * to set allowed groups in /proc/sys/net/ipv4/ping_group_range */
#include "ICMPPing.hpp"

#include <QObject>
#ifdef Q_OS_UNIX
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/ip_icmp.h>
    #include <resolv.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <unistd.h>

// constexpr auto EPING_SOCK = -1;  // Socket creation failed
// constexpr auto EPING_TTL = -2;   // Setting TTL failed
// constexpr auto EPING_SETTO = -3; // Setting timeout failed
// constexpr auto EPING_HOST = -4;  // Unresolvable hostname
// constexpr auto EPING_SEND = -5;  // Sending echo request failed
// constexpr auto EPING_DST = -6;   // Destination unreachable
// constexpr auto EPING_TIME = -7;  // Timeout
// constexpr auto EPING_UNK = -8;   // Unknown error

    #define ECHO_PACKET_SIZE 64
    #ifdef Q_OS_MAC
        #define SOL_IP 0
struct icmphdr
{
    uint8_t type; /* message type */
    uint8_t code; /* type sub-code */
    uint16_t checksum;
    union
    {
        struct
        {
            uint16_t id;
            uint16_t sequence;
        } echo;           /* echo datagram */
        uint32_t gateway; /* gateway address */
        struct
        {
            uint16_t __glibc_reserved;
            uint16_t mtu;
        } frag; /* path mtu discovery */
    } un;
};
    #endif

/// ICMP echo request packet, response packet is the same when using SOCK_DGRAM
struct icmp_echo
{
    icmphdr icmp;
    char data[ECHO_PACKET_SIZE - sizeof(icmphdr)];
};
namespace Qv2ray::components::latency::icmping
{
    /// 1s complementary checksum
    unsigned short ping_checksum(icmp_echo *b, int len)
    {
        unsigned short *buf = reinterpret_cast<unsigned short *>(b);
        unsigned int sum = 0;
        unsigned short result;

        for (sum = 0; len > 1; len -= 2) sum += *buf++;
        if (len == 1)
            sum += *(unsigned char *) buf;
        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);
        result = ~sum;
        return result;
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
        hostent *hname = gethostbyname(address.toStdString().c_str());
        if (!hname)
            return { 0, "EPING_HOST: " + QObject::tr("Unresolvable hostname") };

        // set IP address to ping
        sockaddr_in addr_ping, *addr;
        memset(&addr_ping, 0, sizeof(addr_ping));
        addr_ping.sin_family = hname->h_addrtype;
        addr_ping.sin_port = 0;
        memcpy(&addr_ping.sin_addr, hname->h_addr, hname->h_length);
        addr = &addr_ping;

        // prepare echo request packet
        icmp_echo req;
        memset(&req, 0, sizeof(req));
        req.icmp.type = ICMP_ECHO;
        req.icmp.un.echo.id = 0; // SOCK_DGRAM & 0 => id will be set by kernel
        //
        size_t i;
        for (i = 0; i < sizeof(req.data) - 1; i++)
        {
            req.data[i] = i + '0';
        }
        req.data[i] = 0;
        //
        unsigned short sent_seq;
        req.icmp.un.echo.sequence = sent_seq = seq++;
        req.icmp.checksum = ping_checksum(&req, sizeof(req));

        // send echo request
        gettimeofday(&start, NULL);
        if (sendto(socketId, &req, ECHO_PACKET_SIZE, 0, (struct sockaddr *) addr, sizeof(*addr)) <= 0)
            return { 0, "EPING_SEND: " + QObject::tr("Sending echo request failed") };

        // receive response (if any)

        sockaddr_in r_addr;
        slen = sizeof(r_addr);
        int rlen;
        icmp_echo resp;
        while ((rlen = recvfrom(socketId, &resp, ECHO_PACKET_SIZE, 0, (struct sockaddr *) &r_addr, &slen)) > 0)
        {
            gettimeofday(&end, NULL);

            // skip malformed
            if (rlen != ECHO_PACKET_SIZE)
                continue;

            // skip the ones we didn't send
            if (resp.icmp.un.echo.sequence != sent_seq)
                continue;

            switch (resp.icmp.type)
            {
                case ICMP_ECHOREPLY:
                {
                    return { 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec), {} };
                }
                case ICMP_UNREACH:
                {
                    return { 0, "EPING_DST: " + QObject::tr("Destination unreachable") };
                }
                case ICMP_TIMXCEED:
                {
                    return { 0, "EPING_TIME: " + QObject::tr("Timeout") };
                }
                default:
                {
                    return { 0, "EPING_UNK: " + QObject::tr("Unknown error") };
                }
            }
        }

        return { 0, "EPING_TIME: " + QObject::tr("Timeout") };
    }

    //#include <iostream>
    //    int xmain()
    //    {
    //        init();
    //        std::cout << ping("baidu.com") << std::endl;
    //        deinit();
    //        return 0;
    //    }
} // namespace Qv2ray::components::latency::icmping
#endif
