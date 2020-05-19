#include "ICMPPing.hpp"
#ifdef Q_OS_WIN
//
    #include <WS2tcpip.h>
//
    #include <Windows.h>
//
    #include <iphlpapi.h>
//
    #include <IcmpAPI.h>
//
    #include <QEventLoop>
    #include <QHostInfo>
namespace Qv2ray::components::latency::icmping
{
    ICMPPing::ICMPPing(uint64_t timeout)
    {

        // remember the timeout
        // UNUSED
        this->timeout = timeout;
    }

    ICMPPing ::~ICMPPing()
    {
    }
    QPair<long, QString> ICMPPing::ping(const QString &ipAddr)
    {
        HANDLE hIcmpFile;
        // create icmp handle
        if ((hIcmpFile = IcmpCreateFile()) == INVALID_HANDLE_VALUE)
        {
            throw "IcmpCreateFile failed";
        }
        QList<QHostAddress> addresses;
        {
            QEventLoop loop;
            QHostInfo::fromName(ipAddr).lookupHost(ipAddr, &loop, [&](const QHostInfo &h) {
                for (const auto &addr : h.addresses())
                {
                    if (addr.protocol() == QAbstractSocket::IPv4Protocol)
                    {
                        addresses << addr;
                    }
                }
                loop.quit();
            });
            loop.exec();
        }
        if (addresses.isEmpty())
        {
            return { 0, QObject::tr("DNS lookup failed.") };
        }
        // Parse the destination IP address.
        IN_ADDR dest_ip{};
        if (1 != InetPtonA(AF_INET, addresses.first().toString().toStdString().c_str(), &dest_ip))
        {
            return { 255, "Cannot convert IP address: " + addresses.first().toString() };
        }

        // Payload to send.
        constexpr WORD payload_size = 1;
        unsigned char payload[payload_size]{ 42 };

        // Reply buffer for exactly 1 echo reply, payload data, and 8 bytes for ICMP error message.
        constexpr DWORD reply_buf_size = sizeof(ICMP_ECHO_REPLY) + payload_size + 8;
        unsigned char reply_buf[reply_buf_size]{};

        // Make the echo request.
        DWORD reply_count = IcmpSendEcho(hIcmpFile, dest_ip.S_un.S_addr, payload, payload_size, NULL, reply_buf, reply_buf_size, 10000);

        // Return value of 0 indicates failure, try to get error info.
        if (reply_count == 0)
        {
            auto e = GetLastError();
            DWORD buf_size = 1000;
            TCHAR buf[1000];
            GetIpErrorString(e, buf, &buf_size);
            return { 255, "IcmpSendEcho returned error (" + QString::fromStdWString(buf) + ")" };
        }
        // release the handle on destruction
        IcmpCloseHandle(hIcmpFile);
        const ICMP_ECHO_REPLY *r = (const ICMP_ECHO_REPLY *) reply_buf;
        return QPair<long, QString>(r->RoundTripTime * 1000, QString{});
    }
} // namespace Qv2ray::components::latency::icmping
#endif
