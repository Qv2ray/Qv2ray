#pragma once
#include <QPair>
#include <QString>
namespace Qv2ray::components::port
{
    bool CheckTCPPortStatus(const QString &addr, int port);
    inline bool CheckTCPPortStatus(QPair<QString, int> config)
    {
        return CheckTCPPortStatus(config.first, config.second);
    }
} // namespace Qv2ray::components::port
