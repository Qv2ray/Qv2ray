#include "TCPing.hpp"
#include "uvw.hpp"


namespace Qv2ray::components::latency::tcping
{
    TCPing::TCPing(std::shared_ptr<uvw::Loop> loopin, LatencyTestRequest &reqin, LatencyTestHost *testHost):loop(std::move(loopin)),req(std::move(reqin)),testHost(testHost)
    {
        data.totalCount = 0;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        if(isAddr(req.host.toStdString().data(), req.port, &storage, 0)!=0)
        {
        getAddrHandle=loop->resource<uvw::GetAddrInfoReq>();
        sprintf(digitBuffer, "%d", req.port);
        }
    }
    TCPing::~TCPing()
    {
            if(getAddrHandle)
                getAddrHandle->clear();
    }
    int TCPing::getAddrInfoRes(uvw::AddrInfoEvent& e)
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
        if(rp)
            return 0;
        return -1;
    }

    void TCPing::start()
    {
        start(0,0);
    }
    void TCPing::notifyTestHost()
    {
        if (data.failedCount + successCount == req.totalCount)
        {
            if (data.failedCount == req.totalCount)
                data.avg = LATENCY_TEST_VALUE_ERROR;
            else
                data.errorMessage.clear(), data.avg = data.avg / successCount;
            testHost->OnLatencyTestCompleted(req.id, data);
        }
    }
} // namespace Qv2ray::components::latency::tcping
