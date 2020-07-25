/* Author: Maciek Muszkowski
 *
 * This is a simple ping implementation for Linux.
 * It will work ONLY on kernels 3.x+ and you need
 * to set allowed groups in /proc/sys/net/ipv4/ping_group_range */
#include "ICMPPing.hpp"

#include <QObject>
#ifdef Q_OS_UNIX
    #include <netinet/in.h>
    #include <netinet/ip.h> //macos need that
    #include <netinet/ip_icmp.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <unistd.h>

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

    void ICMPPing::start(int ttl)
    {
        if (((socketId = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0))
        {
            data.errorMessage = "EPING_SOCK: " + QObject::tr("Socket creation failed");
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        // set TTL
        if (setsockopt(socketId, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) != 0)
        {
            data.errorMessage = "EPING_TTL: " + QObject::tr("Failed to setup TTL value");
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        data.totalCount = req.totalCount;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        if (isAddr() == -1)
        {
            getAddrHandle = loop->resource<uvw::GetAddrInfoReq>();
            sprintf(digitBuffer, "%d", req.port);
        }
        async_DNS_lookup(0, 0);
    }

    bool ICMPPing::notifyTestHost()
    {
        if (data.failedCount + successCount == data.totalCount)
        {
            if (data.failedCount == data.totalCount)
                data.avg = LATENCY_TEST_VALUE_ERROR;
            else
                data.errorMessage.clear(), data.avg = data.avg / successCount;
            testHost->OnLatencyTestCompleted(req.id, data);
            if (timeoutTimer)
            {
                timeoutTimer->stop();
                timeoutTimer->clear();
                timeoutTimer->close();
            }
            if (pollHandle)
            {
                if (!pollHandle->closing())
                    pollHandle->stop();
                pollHandle->clear();
                pollHandle->close();
            }
            return true;
        }
        return false;
    }

    void ICMPPing::ping()
    {
        timeoutTimer = loop->resource<uvw::TimerHandle>();
        uvw::OSSocketHandle osSocketHandle{ socketId };
        pollHandle = loop->resource<uvw::PollHandle>(osSocketHandle);
        timeoutTimer->once<uvw::TimerEvent>([this, ptr = std::weak_ptr<ICMPPing>{ shared_from_this() }](auto &, uvw::TimerHandle &h) {
            if (ptr.expired())
                return;
            else
            {
                auto p = ptr.lock();
                pollHandle->clear();
                if (!pollHandle->closing())
                    pollHandle->stop();
                pollHandle->close();
                successCount = 0;
                data.failedCount = data.totalCount = req.totalCount;
                notifyTestHost();
            }
        });
        timeoutTimer->start(uvw::TimerHandle::Time{ 10000 }, uvw::TimerHandle::Time{ 0 });
        auto pollEvent = uvw::Flags<uvw::PollHandle::Event>::from<uvw::PollHandle::Event::READABLE>();
        pollHandle->on<uvw::PollEvent>([this, ptr = shared_from_this()](uvw::PollEvent &, uvw::PollHandle &h) {
            timeval end;
            sockaddr_in addr;
            socklen_t slen = sizeof(sockaddr_in);
            int rlen = 0;
            char buf[1024];
            do
            {
                do
                {
                    rlen = recvfrom(socketId, buf, 1024, 0, (struct sockaddr *) &addr, &slen);
                } while (rlen == -1 && errno == EINTR);

                // skip malformed
    #ifdef Q_OS_MAC
                if (rlen < sizeof(icmp) + 20)
    #else
                if (rlen < sizeof(icmp))
    #endif
                    continue;

    #ifdef Q_OS_MAC
                auto &resp = *reinterpret_cast<icmp *>(buf + 20);
    #else
                auto &resp = *reinterpret_cast<icmp *>(buf);
    #endif
                // skip the ones we didn't send
                auto cur_seq = resp.icmp_hun.ih_idseq.icd_seq;
                if (cur_seq >= seq)
                    continue;

                switch (resp.icmp_type)
                {
                    case ICMP_ECHOREPLY:
                        gettimeofday(&end, nullptr);
                        data.avg +=
                            1000 * (end.tv_sec - startTimevals[cur_seq - 1].tv_sec) + (end.tv_usec - startTimevals[cur_seq - 1].tv_usec) / 1000;
                        successCount++;
                        notifyTestHost();
                        continue;
                    case ICMP_UNREACH:
                        data.errorMessage = "EPING_DST: " + QObject::tr("Destination unreachable");
                        data.failedCount++;
                        if (notifyTestHost())
                        {
                            h.clear();
                            h.close();
                            return;
                        }
                        continue;
                    case ICMP_TIMXCEED:
                        data.errorMessage = "EPING_TIME: " + QObject::tr("Timeout");
                        data.failedCount++;
                        if (notifyTestHost())
                        {
                            h.clear();
                            h.close();
                            return;
                        }
                        continue;
                    default:
                        data.errorMessage = "EPING_UNK: " + QObject::tr("Unknown error");
                        data.failedCount++;
                        if (notifyTestHost())
                        {
                            h.clear();
                            h.close();
                            return;
                        }
                        continue;
                }
            } while (rlen > 0);
        });
        pollHandle->start(pollEvent);
        for (int i = 0; i < req.totalCount; ++i)
        {
            // prepare echo request packet
            icmp _icmp_request;
            memset(&_icmp_request, 0, sizeof(_icmp_request));
            _icmp_request.icmp_type = ICMP_ECHO;
            _icmp_request.icmp_hun.ih_idseq.icd_id = 0; // SOCK_DGRAM & 0 => id will be set by kernel
            _icmp_request.icmp_hun.ih_idseq.icd_seq = seq++;
            _icmp_request.icmp_cksum = ping_checksum(reinterpret_cast<char *>(&_icmp_request), sizeof(_icmp_request));
            int n;
            timeval start;
            gettimeofday(&start, nullptr);
            startTimevals.push_back(start);
            do
            {
                n = ::sendto(socketId, &_icmp_request, sizeof(icmp), 0, (struct sockaddr *) &storage, sizeof(struct sockaddr));
            } while (n < 0 && errno == EINTR);
        }
    }
    ICMPPing::~ICMPPing()
    {
        deinit();
    }
} // namespace Qv2ray::components::latency::icmping
#endif
