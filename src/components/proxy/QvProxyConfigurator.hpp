#pragma once
#include <QHostAddress>
#include <QObject>
#include <QString>
//
namespace Qv2ray::components::proxy
{
    void ClearSystemProxy();
    void SetSystemProxy(const QString &address, int http_port, int socks_port);
} // namespace Qv2ray::components::proxy

using namespace Qv2ray::components;
using namespace Qv2ray::components::proxy;
