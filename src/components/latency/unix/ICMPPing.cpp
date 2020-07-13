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

    ICMPPing::ICMPPing(int ttl,std::shared_ptr<uvw::Loop> loop_in,LatencyTestRequest& req_in,LatencyTestHost* testHost):
          loop(std::move(loop_in)),
          req(std::move(req_in)),
          testHost(testHost)
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
        data.totalCount = req.totalCount;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        if (isAddr(req.host.toStdString().data(), req.port, &storage, 0) == -1)
        {
            getAddrHandle = loop->resource<uvw::GetAddrInfoReq>();
            sprintf(digitBuffer, "%d", req.port);
        }
    }

    int ICMPPing::getAddrInfoRes(uvw::AddrInfoEvent &e)
    {
        struct addrinfo *rp = nullptr;
        for (rp = e.data.get(); rp != nullptr; rp = rp->ai_next)
            if (rp->ai_family == AF_INET)
            {
                if (rp->ai_family == AF_INET)
                    memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in));
                else if (rp->ai_family == AF_INET6)
                    memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in6));
                break;
            }
        if (rp == nullptr)
        {
            // fallback: if we can't find prefered AF, then we choose alternative.
            for (rp = e.data.get(); rp != nullptr; rp = rp->ai_next)
            {
                if (rp->ai_family == AF_INET)
                    memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in));
                else if (rp->ai_family == AF_INET6)
                    memcpy(&storage, rp->ai_addr, sizeof(struct sockaddr_in6));
                break;
            }
        }
        if (rp)
            return 0;
        return -1;
    }
    void ICMPPing::start()
    {
        if (!initialized)
        {
            data.errorMessage = initErrorMessage;
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        if (socketId < 0)
        {
            data.errorMessage="EPING_SOCK:" + QObject::tr("Socket creation failed");
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        async_DNS_lookup(0,0);
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
            return true;
        }
        return false;
    }

    void ICMPPing::ping()
    {
        uvw::OSSocketHandle osSocketHandle{ socketId };
        auto pollHandle = loop->resource<uvw::PollHandle>(osSocketHandle);
        pollHandle->init();
        auto pollEvent = uvw::Flags<uvw::PollHandle::Event>::from<uvw::PollHandle::Event::READABLE>();
        pollHandle->on<uvw::PollEvent>([this, ptr = shared_from_this()](uvw::PollEvent &, uvw::PollHandle &h) {
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
                          1000 * (end.tv_sec - startTimevals[cur_seq - 1].tv_sec) + (end.tv_usec - startTimevals[cur_seq - 1].tv_usec)/1000;
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
} // namespace Qv2ray::components::latency::icmping
#endif
