#pragma once
#include "QJsonStruct.hpp"
#include "base/models/CoreObjectModels.hpp"

namespace Qv2ray::base::config
{
    struct QvRouteConfig_Impl
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvRouteConfig_Impl)
        QJS_PROP(QList<QString>, direct);
        QJS_PROP(QList<QString>, block)
        QJS_PROP(QList<QString>, proxy)
        QvRouteConfig_Impl(const QList<QString> &_direct, const QList<QString> &_block, const QList<QString> &_proxy)
            : direct(_direct), block(_block), proxy(_proxy){};
        QJS_FUNCTION(F(proxy, block, direct))
    };

    struct QvConfig_Route
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvConfig_Route)
        QJS_PROP(QString, domainStrategy, REQUIRED);
        QJS_PROP_D(QString, domainMatcher, "mph", REQUIRED);
        QJS_PROP(QvRouteConfig_Impl, domains, REQUIRED);
        QJS_PROP(QvRouteConfig_Impl, ips, REQUIRED);
        QvConfig_Route(const QvRouteConfig_Impl &_domains, const QvRouteConfig_Impl &_ips, const QString &ds)
            : domainStrategy(ds), domains(_domains), ips(_ips){};
        QJS_FUNCTION(F(domainStrategy, domainMatcher, domains, ips))
    };

    using QvConfig_DNS = objects::DNSObject;
    using QvConfig_FakeDNS = objects::FakeDNSObject;

    struct QvConfig_Outbounds
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvConfig_Outbounds)
        QJS_PROP_D(int, mark, 255);
        QJS_FUNCTION(F(mark))
    };

    struct QvConfig_ForwardProxy
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvConfig_ForwardProxy)
        QJS_PROP_D(bool, enableForwardProxy, false);
        QJS_PROP_D(QString, type, "http");
        QJS_PROP(QString, serverAddress);
        QJS_PROP_D(int, port, 0)
        QJS_PROP_D(bool, useAuth, false)
        QJS_PROP(QString, username)
        QJS_PROP(QString, password)
        QJS_FUNCTION(F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };

    struct QvConfig_Connection
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvConfig_Connection)
        QJS_PROP_D(bool, enableProxy, true);
        QJS_PROP_D(bool, bypassCN, true);
        QJS_PROP_D(bool, bypassBT, false);
        QJS_PROP_D(bool, bypassLAN, true);
        QJS_PROP_D(bool, v2rayFreedomDNS, false);
        QJS_PROP_D(bool, dnsIntercept, false);
        QJS_FUNCTION(F(bypassCN, bypassBT, bypassLAN, enableProxy, v2rayFreedomDNS, dnsIntercept))
    };

    struct QvConfig_SystemProxy
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvConfig_SystemProxy)
        QJS_PROP_D(bool, setSystemProxy, true);
        QJS_FUNCTION(F(setSystemProxy))
    };

    struct Qv2rayConfig_ProtocolInboundBase
    {
        Q_GADGET
        const QList<QString> default_destOverride{ "http", "tls" };
        QJS_CONSTRUCTOR(Qv2rayConfig_ProtocolInboundBase)
        QJS_PROP_D(int, port, 0)
        QJS_PROP_D(bool, useAuth, false);
        QJS_PROP_D(bool, sniffing, false);
        QJS_PROP_D(QList<QString>, destOverride, default_destOverride);
        QJS_PROP(objects::AccountObject, account);
        QJS_PROP(bool, metadataOnly);
        QJS_FUNCTION(F(port, useAuth, sniffing, destOverride, account, metadataOnly))
    };

    struct QvConfig_SocksInbound : Qv2rayConfig_ProtocolInboundBase
    {
        Q_GADGET
        typedef QvConfig_SocksInbound this_type_t;
        QJS_PROP_D(bool, enableUDP, true);
        QJS_PROP_D(QString, localIP, "127.0.0.1");

      public:
        QvConfig_SocksInbound(const this_type_t &another)
        {
            *this = another;
        }
        void operator=(const this_type_t &another)
        {
            loadJson(another.toJson());
        }
        QvConfig_SocksInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            set_port(1089);
        }
        QJS_FUNCTION(B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
    };

    struct QvConfig_HttpInbound : Qv2rayConfig_ProtocolInboundBase
    {
        Q_GADGET
      public:
        typedef QvConfig_HttpInbound this_type_t;
        QvConfig_HttpInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            set_port(8889);
        }
        QvConfig_HttpInbound(const this_type_t &another)
        {
            *this = another;
        }
        void operator=(const this_type_t &another)
        {
            loadJson(another.toJson());
        }
        QJS_FUNCTION(B(Qv2rayConfig_ProtocolInboundBase))
    };

    struct QvConfig_TProxy : Qv2rayConfig_ProtocolInboundBase
    {
        Q_GADGET
        typedef QvConfig_TProxy this_type_t;

      public:
        QvConfig_TProxy() : Qv2rayConfig_ProtocolInboundBase()
        {
            set_port(12345);
            set_sniffing(true);
        }
        QvConfig_TProxy(const this_type_t &another)
        {
            *this = another;
        }
        void operator=(const this_type_t &another)
        {
            loadJson(another.toJson());
        }
        QJS_PROP_D(QString, tProxyIP, "127.0.0.1");
        QJS_PROP_D(QString, tProxyV6IP, "::1");
        QJS_PROP_D(bool, hasTCP, true);
        QJS_PROP_D(bool, hasUDP, true);
        QJS_PROP_D(QString, mode, "tproxy");
        QJS_FUNCTION(B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
    };

    struct QvConfig_Inbounds
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvConfig_Inbounds)

        QJS_PROP_D(QString, listenip, "127.0.0.1");
        QJS_PROP_D(bool, useSocks, true);
        QJS_PROP_D(bool, useHTTP, true);
        QJS_PROP_D(bool, useTPROXY, false);
        //
        QJS_PROP(QvConfig_TProxy, tProxySettings);
        QJS_PROP(QvConfig_HttpInbound, httpSettings);
        QJS_PROP(QvConfig_SocksInbound, socksSettings);
        QJS_PROP(QvConfig_SystemProxy, systemProxySettings);

        QJS_FUNCTION(F(listenip, useSocks, useHTTP, useTPROXY), F(tProxySettings, httpSettings, socksSettings, systemProxySettings))
    };
} // namespace Qv2ray::base::config
