#pragma once
#include "QJsonStruct.hpp"
#include "base/models/CoreObjectModels.hpp"

namespace Qv2ray::base::config
{
    struct QvConfig_Route
    {
        struct QvRouteConfig_Impl
        {
            QList<QString> direct;
            QList<QString> block;
            QList<QString> proxy;
            QvRouteConfig_Impl(){};
            QvRouteConfig_Impl(const QList<QString> &_direct, const QList<QString> &_block, const QList<QString> &_proxy)
                : direct(_direct), block(_block), proxy(_proxy){};
            JSONSTRUCT_COMPARE(QvRouteConfig_Impl, direct, block, proxy)
            JSONSTRUCT_REGISTER(QvRouteConfig_Impl, F(proxy, block, direct))
        };

        QString domainStrategy;
        QString domainMatcher = "mph";
        QvRouteConfig_Impl domains;
        QvRouteConfig_Impl ips;
        QvConfig_Route(){};
        QvConfig_Route(const QvRouteConfig_Impl &_domains, const QvRouteConfig_Impl &_ips, const QString &ds)
            : domainStrategy(ds), domains(_domains), ips(_ips){};
        JSONSTRUCT_COMPARE(QvConfig_Route, domainStrategy, domainMatcher, domains, ips)
        JSONSTRUCT_REGISTER(QvConfig_Route, A(domainStrategy, domainMatcher, domains, ips))
    };

    using QvConfig_DNS = objects::DNSObject;
    using QvConfig_FakeDNS = objects::FakeDNSObject;

    struct QvConfig_Outbounds
    {
        int mark = 255;
        JSONSTRUCT_COMPARE(QvConfig_Outbounds, mark)
        JSONSTRUCT_REGISTER(QvConfig_Outbounds, F(mark))
    };

    struct QvConfig_ForwardProxy
    {
        bool enableForwardProxy = false;
        QString type = "http";
        QString serverAddress = "";
        int port = 0;
        bool useAuth = false;
        QString username = "";
        QString password = "";
        JSONSTRUCT_COMPARE(QvConfig_ForwardProxy, enableForwardProxy, type, serverAddress, port, useAuth, username, password)
        JSONSTRUCT_REGISTER(QvConfig_ForwardProxy, F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };

    struct QvConfig_Connection
    {
        bool enableProxy = true;
        bool bypassCN = true;
        bool bypassBT = false;
        bool bypassLAN = true;
        bool v2rayFreedomDNS = false;
        bool dnsIntercept = false;
        JSONSTRUCT_COMPARE(QvConfig_Connection, enableProxy, //
                           bypassCN, bypassBT, bypassLAN,    //
                           v2rayFreedomDNS, dnsIntercept)
        JSONSTRUCT_REGISTER(QvConfig_Connection, F(bypassCN, bypassBT, bypassLAN, enableProxy, v2rayFreedomDNS, dnsIntercept))
    };

    struct QvConfig_SystemProxy
    {
        bool setSystemProxy = true;
        JSONSTRUCT_COMPARE(QvConfig_SystemProxy, setSystemProxy)
        JSONSTRUCT_REGISTER(QvConfig_SystemProxy, F(setSystemProxy))
    };

    struct Qv2rayConfig_ProtocolInboundBase
    {
        int port = 0;
        bool useAuth = false;
        bool sniffing = false;
        QList<QString> destOverride = { "http", "tls" };
        objects::AccountObject account;
        bool metadataOnly = true;
        Qv2rayConfig_ProtocolInboundBase(){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_ProtocolInboundBase, F(port, useAuth, sniffing, destOverride, account, metadataOnly))
    };

    struct QvConfig_SocksInbound : Qv2rayConfig_ProtocolInboundBase
    {
        bool enableUDP = true;
        QString localIP = "127.0.0.1";
        QvConfig_SocksInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 1089;
        }
        JSONSTRUCT_COMPARE(QvConfig_SocksInbound, enableUDP, localIP, port, useAuth, sniffing, destOverride, metadataOnly)
        JSONSTRUCT_REGISTER(QvConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
    };

    struct QvConfig_HttpInbound : Qv2rayConfig_ProtocolInboundBase
    {
        QvConfig_HttpInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 8889;
        }
        JSONSTRUCT_COMPARE(QvConfig_HttpInbound, port, useAuth, sniffing, destOverride, metadataOnly)
        JSONSTRUCT_REGISTER(QvConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
    };

    struct QvConfig_TProxy : Qv2rayConfig_ProtocolInboundBase
    {
        QString tProxyIP = "127.0.0.1";
        QString tProxyV6IP = "::1";
        bool hasTCP = true;
        bool hasUDP = true;
        QString mode = "tproxy";
        QvConfig_TProxy() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 12345;
            sniffing = true;
        }
        JSONSTRUCT_COMPARE(QvConfig_TProxy, tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode, port, useAuth, sniffing, destOverride, metadataOnly)
        JSONSTRUCT_REGISTER(QvConfig_TProxy, B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
    };

    struct QvConfig_BrowserForwarder
    {
        QString address = "127.0.0.1";
        int port = 8088;

        QvConfig_BrowserForwarder() {}

        JSONSTRUCT_COMPARE(QvConfig_BrowserForwarder, address, port)
        JSONSTRUCT_REGISTER(QvConfig_BrowserForwarder, F(address, port))
    };

    struct QvConfig_Inbounds
    {
        QString listenip = "127.0.0.1";
        bool useSocks = true;
        bool useHTTP = true;
        bool useTPROXY = false;
        //
        QvConfig_TProxy tProxySettings;
        QvConfig_HttpInbound httpSettings;
        QvConfig_SocksInbound socksSettings;
        QvConfig_SystemProxy systemProxySettings;
        QvConfig_BrowserForwarder browserForwarderSettings;
        //
        JSONSTRUCT_COMPARE(QvConfig_Inbounds, listenip, useSocks, useHTTP, useTPROXY, tProxySettings, httpSettings, socksSettings,
                           systemProxySettings, browserForwarderSettings);
        JSONSTRUCT_REGISTER(QvConfig_Inbounds,                         //
                            A(socksSettings),                          //
                            F(listenip, useSocks, useHTTP, useTPROXY), //
                            F(tProxySettings, httpSettings, systemProxySettings, browserForwarderSettings));
    };
} // namespace Qv2ray::base::config
