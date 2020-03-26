#include "ICMPPinger.hpp"
#if 0

ICMPPinger::ICMPPinger(UINT64 timeout = DEFAULT_TIMEOUT)
{
    // create icmp handle
    if ((this->hIcmpFile = IcmpCreateFile()) == INVALID_HANDLE_VALUE)
    {
        throw "IcmpCreateFile failed";
    }

    // remember the timeout
    this->timeout = timeout;
}

ICMPPinger::~ICMPPinger()
{
    // release the handle on destruction
    IcmpCloseHandle(this->hIcmpFile);
}

std::pair<std::optional<UINT64>, std::optional<std::string>> ICMPPinger::ping(const std::string &ipAddr)
{
    // convert network address
    const auto addr = inet_addr(ipAddr.c_str());
    if (addr == INADDR_NONE)
    {
        return std::pair(std::nullopt, "invalid ip address: " + ipAddr);
    }

    // request buffer
    const static char bufRequest[] = "echo test";

    // response buffer
    const auto responseSize = sizeof(ICMP_ECHO_REPLY) + sizeof(bufRequest);
    const std::unique_ptr<char> bufRecv(new (char[responseSize]));

    // send echo
    auto ret = IcmpSendEcho(this->hIcmpFile, addr, (LPVOID) bufRequest, sizeof(bufRequest), NULL, bufRecv.get(), responseSize, this->timeout);

    // ret == 0: failed
    if (ret == 0)
    {
        return std::pair(std::nullopt, "IcmpSendEcho returned error");
    }

    // read round-trip time
    PICMP_ECHO_REPLY pReply = (PICMP_ECHO_REPLY) bufRecv.get();
    return std::pair(pReply->RoundTripTime, std::nullopt);
}

#endif
