#pragma once
#include <QtGlobal>
#include <optional>
#if 0
/**
 * ICMPPinger - An Implementation of ICMPPing on Windows Platform
 * Required Windows Version: 2000 / XP / 7 / Vista+
 * License: WTFPL
 */

    #include <icmpapi.h>
    #include <iphlpapi.h>
    #include <memory>
    #include <optional>
    #include <utility>
    #include <winsock2.h>

class ICMPPinger
{
  public:
    ICMPPinger(UINT64 timeout = DEFAULT_TIMEOUT);
    ~ICMPPinger();

  public:
    static const UINT64 DEFAULT_TIMEOUT = 10000U;

  public:
    std::pair<std::optional<UINT64>, std::optional<std::string>> ping(const std::string &ipAddr);

  private:
    HANDLE hIcmpFile;
    UINT64 timeout = DEFAULT_TIMEOUT;
};
#endif
