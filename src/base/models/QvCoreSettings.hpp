#pragma once
#include "QJsonStruct.hpp"
#include "base/models/CoreObjectModels.hpp"

namespace Qv2ray::base::config
{
    struct QvRouteConfig_Impl : public QObject
    {
        Q_OBJECT
      public:
        QvRouteConfig_Impl(const QList<QString> &_direct, const QList<QString> &_block, const QList<QString> &_proxy)
        {
            direct = _direct;
            block = _block;
            proxy = _proxy;
        }
        QJS_PROP(QList<QString>, direct)
        QJS_PROP(QList<QString>, block)
        QJS_PROP(QList<QString>, proxy)
        QJS_FUNCTION(QvRouteConfig_Impl, F(proxy, block, direct))
    };

    struct QvConfig_Route : public QObject
    {
        Q_OBJECT
      public:
        QvConfig_Route(const QvRouteConfig_Impl &_domains, const QvRouteConfig_Impl &_ips, const QString &ds)
        {
            domainStrategy = ds;
            domains = _domains;
            ips = _ips;
        }
        QJS_PROP(QString, domainStrategy, REQUIRED)
        QJS_PROP_D(QString, domainMatcher, "mph", REQUIRED)
        QJS_PROP(QvRouteConfig_Impl, domains, REQUIRED)
        QJS_PROP(QvRouteConfig_Impl, ips, REQUIRED)
        QJS_FUNCTION(QvConfig_Route, F(domainStrategy, domainMatcher, domains, ips))
    };

    using QvConfig_DNS = objects::DNSObject;
    using QvConfig_FakeDNS = objects::FakeDNSObject;

    struct QvConfig_Outbounds : public QObject
    {
        Q_OBJECT
      public:
        QJS_PROP_D(int, mark, 255)
        QJS_FUNCTION(QvConfig_Outbounds, F(mark))
    };

    struct QvConfig_ForwardProxy : public QObject
    {
        Q_OBJECT
      public:
        QJS_PROP_D(bool, enableForwardProxy, false)
        QJS_PROP_D(QString, type, "http")
        QJS_PROP(QString, serverAddress)
        QJS_PROP_D(int, port, 0)
        QJS_PROP_D(bool, useAuth, false)
        QJS_PROP(QString, username)
        QJS_PROP(QString, password)
        QJS_FUNCTION(QvConfig_ForwardProxy, F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };

    struct QvConfig_Connection : public QObject
    {
        Q_OBJECT
      public:
        QJS_PROP_D(bool, enableProxy, true)
        QJS_PROP_D(bool, bypassCN, true)
        QJS_PROP_D(bool, bypassBT, false)
        QJS_PROP_D(bool, bypassLAN, true)
        QJS_PROP_D(bool, v2rayFreedomDNS, false)
        QJS_PROP_D(bool, dnsIntercept, false)
        QJS_FUNCTION(QvConfig_Connection, F(bypassCN, bypassBT, bypassLAN, enableProxy, v2rayFreedomDNS, dnsIntercept))
    };

    struct QvConfig_SystemProxy : public QObject
    {
        Q_OBJECT
      public:
        QJS_PROP_D(bool, setSystemProxy, true)
        QJS_FUNCTION(QvConfig_SystemProxy, F(setSystemProxy))
    };

    struct Qv2rayConfig_ProtocolInboundBase : public QObject
    {
        Q_OBJECT
      public:
        const QList<QString> default_destOverride{ "http", "tls" };
        QJS_PROP_D(int, port, 0)
        QJS_PROP_D(bool, useAuth, false)
        QJS_PROP_D(bool, sniffing, false)
        QJS_PROP_D(QList<QString>, destOverride, default_destOverride)
        QJS_PROP(objects::AccountObject, account)
        QJS_PROP(bool, metadataOnly)
        QJS_FUNCTION(Qv2rayConfig_ProtocolInboundBase, F(port, useAuth, sniffing, destOverride, account, metadataOnly))
    };

    struct QvConfig_SocksInbound : public Qv2rayConfig_ProtocolInboundBase
    {
        Q_OBJECT
      public:
        QvConfig_SocksInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 1089;
        }
        QJS_PROP_D(bool, enableUDP, true)
        QJS_PROP_D(QString, localIP, "127.0.0.1")
        QJS_FUNC_COMP(QvConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
        QJS_FUNC_COPY(QvConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
        QJS_FUNC_JSON(QvConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
    };

    struct QvConfig_HttpInbound : public Qv2rayConfig_ProtocolInboundBase
    {
        Q_OBJECT
        using this_type_t = QvConfig_HttpInbound;

      public:
        QvConfig_HttpInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 8889;
        }
        QJS_FUNC_COMP(QvConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
        QJS_FUNC_COPY(QvConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
        QJS_FUNC_JSON(QvConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
    };

    struct QvConfig_TProxy : public Qv2rayConfig_ProtocolInboundBase
    {
        Q_OBJECT
      public:
        QvConfig_TProxy() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 12345;
            sniffing = true;
        }
        QJS_PROP_D(QString, tProxyIP, "127.0.0.1")
        QJS_PROP_D(QString, tProxyV6IP, "::1")
        QJS_PROP_D(bool, hasTCP, true)
        QJS_PROP_D(bool, hasUDP, true)
        QJS_PROP_D(QString, mode, "tproxy")
        QJS_FUNC_COMP(QvConfig_TProxy, B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
        QJS_FUNC_COPY(QvConfig_TProxy, B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
        QJS_FUNC_JSON(QvConfig_TProxy, B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
    };

    struct QvConfig_Inbounds : public QObject
    {
        Q_OBJECT
        QJS_PROP_D(QString, listenip, "127.0.0.1")
        QJS_PROP_D(bool, useSocks, true)
        QJS_PROP_D(bool, useHTTP, true)
        QJS_PROP_D(bool, useTPROXY, false)
        QJS_PROP(QvConfig_TProxy, tProxySettings)
        QJS_PROP(QvConfig_HttpInbound, httpSettings)
        QJS_PROP(QvConfig_SocksInbound, socksSettings)
        QJS_PROP(QvConfig_SystemProxy, systemProxySettings)
        QJS_FUNCTION(QvConfig_Inbounds, F(listenip, useSocks, useHTTP, useTPROXY),
                     F(tProxySettings, httpSettings, socksSettings, systemProxySettings))
    };
} // namespace Qv2ray::base::config
