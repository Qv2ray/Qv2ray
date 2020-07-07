#include "LatencyTest.hpp"

#include "LatencyTestThread.hpp"
#include "core/handler/ConfigHandler.hpp"


namespace Qv2ray::components::latency
{
    int getSockAddress(std::shared_ptr<uvw::Loop>& loop, const char* host, int port, struct sockaddr_storage* storage, int ipv6first)
    {
        if (uv_ip4_addr(host, port, reinterpret_cast<sockaddr_in*>(storage)) == 0) {
            return 0;
        }
        if (uv_ip6_addr(host, port, reinterpret_cast<sockaddr_in6*>(storage)) == 0) {
            return 0;
        }
        //not an ip
        auto getAddrInfoReq = loop->resource<uvw::GetAddrInfoReq>();
        char digitBuffer[20] = { 0 };
        sprintf(digitBuffer, "%d", port);
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        auto dns_res = getAddrInfoReq->addrInfoSync(host, digitBuffer, &hints);
        int prefer_af = ipv6first ? AF_INET6 : AF_INET;
        if (dns_res.first) {
            struct addrinfo* rp = nullptr;
            for (rp = dns_res.second.get(); rp != nullptr; rp = rp->ai_next)
                if (rp->ai_family == prefer_af) {
                    if (rp->ai_family == AF_INET)
                        memcpy(storage, rp->ai_addr, sizeof(struct sockaddr_in));
                    else if (rp->ai_family == AF_INET6)
                        memcpy(storage, rp->ai_addr, sizeof(struct sockaddr_in6));
                    break;
                }
            if (rp == nullptr) {
                //fallback: if we can't find prefered AF, then we choose alternative.
                for (rp = dns_res.second.get(); rp != nullptr; rp = rp->ai_next) {
                    if (rp->ai_family == AF_INET)
                        memcpy(storage, rp->ai_addr, sizeof(struct sockaddr_in));
                    else if (rp->ai_family == AF_INET6)
                        memcpy(storage, rp->ai_addr, sizeof(struct sockaddr_in6));
                    break;
                }
            }
            if (rp == nullptr) {
                return -1; // dns not resolved
            }
            return 0;
        } else {
            return -1; // dns not resolved
        }
        return -1;
    }
    LatencyTestHost::LatencyTestHost(const int defaultCount, QObject *parent) : QObject(parent)
    {
        totalTestCount = defaultCount;
        auto latencyThread = new LatencyTestThread(this);
        latencyThreads.push_back(latencyThread);
    }

    LatencyTestHost::~LatencyTestHost()
    {
        StopAllLatencyTest();
    }

    void LatencyTestHost::StopAllLatencyTest()
    {
        for(auto& latencyThread:latencyThreads)
        {
            latencyThread->stopLatencyTest();
            latencyThread->wait();
            latencyThread->start();
        }
    }

    void LatencyTestHost::TestLatency(const ConnectionId &id, Qv2rayLatencyTestingMethod method)
    {
        //round-robin scheme
        const auto &[protocol, host, port] = GetConnectionInfo(id);
        auto& latencyThread = latencyThreads[nextLatencyTestThread++];
        latencyThread->pushRequests(id, host,  port, totalTestCount,  method);
        if(nextLatencyTestThread==latencyThreads.size())
            nextLatencyTestThread=0;
    }

} // namespace Qv2ray::components::latency
