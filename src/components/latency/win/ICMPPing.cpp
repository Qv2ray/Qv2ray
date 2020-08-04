#include "ICMPPing.hpp"
#ifdef Q_OS_WIN
typedef struct _IO_STATUS_BLOCK
{
    union
    {
        NTSTATUS Status;
        PVOID Pointer;
    } DUMMYUNIONNAME;
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
typedef VOID(NTAPI *PIO_APC_ROUTINE)(IN PVOID ApcContext, IN PIO_STATUS_BLOCK IoStatusBlock, IN ULONG Reserved);
    #define PIO_APC_ROUTINE_DEFINED
    #include <WS2tcpip.h>
    //
    #include <Windows.h>
    //
    #include <iphlpapi.h>
    //
    #include <IcmpAPI.h>
    //
    #include <QString>

namespace Qv2ray::components::latency::icmping
{
    ICMPPing ::~ICMPPing()
    {
    }
    void ICMPPing::ping()
    {
        waitHandleTimer = loop->resource<uvw::TimerHandle>();
        waitHandleTimer->on<uvw::TimerEvent>([ptr = shared_from_this(), this](auto &&, auto &&h) {
            SleepEx(0, TRUE);
            if (data.failedCount + successCount == data.totalCount)
            {
                waitHandleTimer->stop();
                waitHandleTimer->close();
                waitHandleTimer->clear();
            }
        });
        for (; data.totalCount < req.totalCount; ++data.totalCount)
        {
            pingImpl();
        }
        waitHandleTimer->start(uvw::TimerHandle::Time{ 500 }, uvw::TimerHandle::Time{ 500 });
    }
    void ICMPPing::pingImpl()
    {
        constexpr WORD payload_size = 1;
        constexpr DWORD reply_buf_size = sizeof(ICMP_ECHO_REPLY) + payload_size + 8;
        struct ICMPReply
        {
            ICMPReply(std::function<void(bool, long, const QString &, HANDLE)> f) : whenIcmpFailed(std::move(f))
            {
                hIcmpFile = IcmpCreateFile();
            }
            unsigned char reply_buf[reply_buf_size]{};
            unsigned char payload[payload_size]{ 42 };
            HANDLE hIcmpFile;
            std::function<void(bool, long, const QString &, HANDLE)> whenIcmpFailed;
            ~ICMPReply()
            {
                if (hIcmpFile != INVALID_HANDLE_VALUE)
                    IcmpCloseHandle(hIcmpFile);
            }
        };
        auto icmpReply = new ICMPReply{ [this, id = req.id](bool isSuccess, long res, const QString &message, HANDLE h) {
            if (!isSuccess)
            {
                data.errorMessage = message;
                data.failedCount++;
            }
            else
            {
                data.avg += res;
                data.best = std::min(res, data.best);
                data.worst = std::max(res, data.worst);
                successCount++;
            }
            notifyTestHost(testHost, id);
        } };
        if (icmpReply->hIcmpFile == INVALID_HANDLE_VALUE)
        {
            data.errorMessage = "IcmpCreateFile failed";
            data.failedCount++;
            notifyTestHost(testHost, req.id);
            delete icmpReply;
            return;
        }
        IcmpSendEcho2(
            icmpReply->hIcmpFile, NULL,
            [](PVOID ctx, PIO_STATUS_BLOCK b, ULONG r) {
                static int i = 1;
                LOG(MODULE_NETWORK, "hit" + QSTRN(i++))
                auto replyPtr = reinterpret_cast<ICMPReply *>(ctx);
                auto isSuccess = (NTSTATUS(b->Status)) >= 0;
                long res = 0;
                QString message;
                if (isSuccess)
                {
                    const ICMP_ECHO_REPLY *r = (const ICMP_ECHO_REPLY *) replyPtr->reply_buf;
                    res = r->RoundTripTime * 1000;
                }
                else
                {
                    auto e = GetLastError();
                    DWORD buf_size = 1000;
                    TCHAR buf[1000];
                    GetIpErrorString(e, buf, &buf_size);
                    message = "IcmpSendEcho returned error (" + QString::fromStdWString(buf) + ")";
                }
                replyPtr->whenIcmpFailed(isSuccess, res, message, replyPtr->hIcmpFile);
                delete replyPtr;
            },
            icmpReply, reinterpret_cast<IPAddr &>(reinterpret_cast<sockaddr_in &>(storage).sin_addr), icmpReply->payload, payload_size, NULL,
            icmpReply->reply_buf, reply_buf_size, 10000);
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
    void ICMPPing::start()
    {
        data.totalCount = 0;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        af = isAddr();
        if (af == -1)
        {
            getAddrHandle = loop->resource<uvw::GetAddrInfoReq>();
            sprintf(digitBuffer, "%d", req.port);
        }
        async_DNS_lookup(0, 0);
    }
} // namespace Qv2ray::components::latency::icmping
#endif
