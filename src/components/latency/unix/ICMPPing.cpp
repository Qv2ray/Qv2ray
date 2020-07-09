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
    #include "uvw.hpp"

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
        if (((socketId = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0))
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

    void ICMPPing::start(std::shared_ptr<uvw::Loop> loop, LatencyTestRequest &req, LatencyTestHost *testHost)
    {
        if (!initialized)
        {
            data.errorMessage = initErrorMessage;
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        struct sockaddr_storage storage;
        data.totalCount = req.totalCount;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        if (getSockAddress(loop, req.host.toStdString().data(), req.port, &storage, 0) != 0)
        {
            data.errorMessage = QObject::tr("DNS not resolved");
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        uvw::OSSocketHandle osSocketHandle{ socketId };
        auto pollHandle = loop->resource<uvw::PollHandle>(osSocketHandle);
        pollHandle->init();
        auto pollEvent = uvw::Flags<uvw::PollHandle::Event>::from<uvw::PollHandle::Event::READABLE>();
        pollHandle->on<uvw::PollEvent>([this, testHost, id = req.id, ptr = shared_from_this()](uvw::PollEvent &, uvw::PollHandle &h) {
            timeval end;
            sockaddr_in remove_addr;
            socklen_t slen = sizeof(remove_addr);
            int rlen = 0;
            icmp resp;
            do
            {
                do
                {
                    rlen = recvfrom(socketId, &resp, sizeof(icmp), 0, (struct sockaddr *) &remove_addr, &slen);
                } while (rlen == -1 && errno == EINTR);
                gettimeofday(&end, NULL);

                // skip malformed
                if (rlen != sizeof(icmp))
                    continue;

                // skip the ones we didn't send
                auto cur_seq = resp.icmp_hun.ih_idseq.icd_seq;
                if (cur_seq >= seq)
                    continue;

                switch (resp.icmp_type)
                {
                    case ICMP_ECHOREPLY:
                        data.avg =
                            1000000 * (end.tv_sec - startTimevals[cur_seq - 1].tv_sec) + (end.tv_usec - startTimevals[cur_seq - 1].tv_usec);
                        successCount++;
                        notifyTestHost(testHost, id);
                        continue;
                    case ICMP_UNREACH:
                        data.errorMessage = "EPING_DST: " + QObject::tr("Destination unreachable");
                        data.failedCount++;
                        if (notifyTestHost(testHost, id))
                        {
                            h.clear();
                            h.close();
                            return;
                        }
                        continue;
                    case ICMP_TIMXCEED:
                        data.errorMessage = "EPING_TIME: " + QObject::tr("Timeout");
                        data.failedCount++;
                        if (notifyTestHost(testHost, id))
                        {
                            h.clear();
                            h.close();
                            return;
                        }
                        continue;
                    default:
                        data.errorMessage = "EPING_UNK: " + QObject::tr("Unknown error");
                        data.failedCount++;
                        if (notifyTestHost(testHost, id))
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
    bool ICMPPing::notifyTestHost(LatencyTestHost *testHost, const ConnectionId &id)
    {
        if (data.failedCount + successCount == data.totalCount)
        {
            if (data.failedCount == data.totalCount)
                data.avg = LATENCY_TEST_VALUE_ERROR;
            else
                data.errorMessage.clear(), data.avg = data.avg / successCount / 1000;
            testHost->OnLatencyTestCompleted(id, data);
            return true;
        }
        return false;
    }
} // namespace Qv2ray::components::latency::icmping
#endif
