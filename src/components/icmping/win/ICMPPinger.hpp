#pragma once

/**
 * ICMPPinger - An Implementation of ICMPPing on Windows Platform
 * Required Windows Version: 2000 / XP / 7 / Vista+
 * License: WTFPL
 */

#include<winsock2.h>
#include<iphlpapi.h>
#include<icmpapi.h>

#include<utility>
#include<optional>
#include<memory>


class ICMPPinger {
    public:
        ICMPPinger(UINT64 timeout = DEFAULT_TIMEOUT);
        ~ICMPPinger();

    public:
        static const UINT64 DEFAULT_TIMEOUT = 10000U;

    public:
        std::pair<std::optional<UINT64>, std::optional<std::string>> ping(const std::string& ipAddr);

    private:
        HANDLE hIcmpFile;
        UINT64 timeout = DEFAULT_TIMEOUT;
};
