#pragma once
#include <QFlags>
#include <QHostAddress>
#include <QObject>
#include <QString>
#include <QStringList>
//
namespace Qv2ray::components::proxy
{
    enum ProxyOption {
        OverrideProxyException = 0x01,
        AppendScheme = 0x02,
        SetSocksProxy = 0x04,
        SetHttpProxy = 0x08
    };
    Q_DECLARE_FLAGS(ProxyOptions, ProxyOption)
    void ClearSystemProxy();
    void SetSystemProxy(const QString &httpAddress, const QString &socksAddress, int http_port, int socks_port, const QStringList &proxyException, ProxyOptions flags);
} // namespace Qv2ray::components::proxy

using namespace Qv2ray::components;
using namespace Qv2ray::components::proxy;
