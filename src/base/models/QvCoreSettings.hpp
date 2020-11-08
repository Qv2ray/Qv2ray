#pragma once
#include "3rdparty/QJsonStruct/QJsonStruct.hpp"
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
            friend bool operator==(const QvRouteConfig_Impl &left, const QvRouteConfig_Impl &right)
            {
                return left.direct == right.direct && left.block == right.block && left.proxy == right.proxy;
            }
            QvRouteConfig_Impl(const QList<QString> &_direct, const QList<QString> &_block, const QList<QString> &_proxy)
                : direct(_direct), //
                  block(_block),   //
                  proxy(_proxy){};
            JSONSTRUCT_REGISTER(QvRouteConfig_Impl, F(proxy, block, direct))
        };
        QString domainStrategy;
        QvRouteConfig_Impl domains;
        QvRouteConfig_Impl ips;
        friend bool operator==(const QvConfig_Route &left, const QvConfig_Route &right)
        {
            return left.domainStrategy == right.domainStrategy && left.domains == right.domains && left.ips == right.ips;
        }
        QvConfig_Route(){};
        QvConfig_Route(const QvRouteConfig_Impl &_domains, const QvRouteConfig_Impl &_ips, const QString &ds)
            : domainStrategy(ds), //
              domains(_domains),  //
              ips(_ips){};
        JSONSTRUCT_REGISTER(QvConfig_Route, F(domainStrategy, domains, ips))
    };

    using QvConfig_DNS = objects::DNSObject;

    struct QvConfig_Outbounds
    {
        int mark = 255;
        JSONSTRUCT_REGISTER(QvConfig_Outbounds, F(mark))
    };

    struct QvConfig_ForwardProxy
    {
        bool enableForwardProxy = false;
        QString type = "http";
        QString serverAddress;
        int port;
        bool useAuth = false;
        QString username;
        QString password;
        JSONSTRUCT_REGISTER(QvConfig_ForwardProxy, F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };

    struct QvConfig_Connection
    {
        bool enableProxy = true;
        bool bypassCN = true;
        bool bypassBT = false;
        bool bypassLAN = true;
        bool v2rayFreedomDNS = false;
        bool withLocalDNS = true;
        bool dnsIntercept = false;
        bool fakeDNS = false;
        JSONSTRUCT_REGISTER(QvConfig_Connection, F(bypassCN, bypassBT, bypassLAN, enableProxy, v2rayFreedomDNS, withLocalDNS, dnsIntercept, fakeDNS))
    };

    struct QvConfig_SystemProxy
    {
        bool setSystemProxy = true;
        JSONSTRUCT_REGISTER(QvConfig_SystemProxy, F(setSystemProxy))
    };

    struct __Qv2rayConfig_ProtocolInboundBase
    {
        int port;
        bool useAuth = false;
        bool sniffing = false;
        objects::AccountObject account;
        __Qv2rayConfig_ProtocolInboundBase(int _port = 0) : port(_port){};
        JSONSTRUCT_REGISTER(__Qv2rayConfig_ProtocolInboundBase, F(port, useAuth, sniffing, account))
    };

    struct QvConfig_SocksInbound : __Qv2rayConfig_ProtocolInboundBase
    {
        bool enableUDP = true;
        QString localIP = "127.0.0.1";
        QvConfig_SocksInbound() : __Qv2rayConfig_ProtocolInboundBase(1089){};
        JSONSTRUCT_REGISTER(QvConfig_SocksInbound, B(__Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
    };

    struct QvConfig_HttpInbound : __Qv2rayConfig_ProtocolInboundBase
    {
        QvConfig_HttpInbound() : __Qv2rayConfig_ProtocolInboundBase(8889){};
        JSONSTRUCT_REGISTER(QvConfig_HttpInbound, B(__Qv2rayConfig_ProtocolInboundBase))
    };

    struct QvConfig_TProxy
    {
        QString tProxyIP = "127.0.0.1";
        QString tProxyV6IP;
        int port = 12345;
        bool hasTCP = true;
        bool hasUDP = true;
        bool sniffing = true;
        QString mode = "tproxy";
        JSONSTRUCT_REGISTER(QvConfig_TProxy, F(tProxyIP, tProxyV6IP, port, hasTCP, hasUDP, sniffing, mode))
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
        JSONSTRUCT_REGISTER(QvConfig_Inbounds,                         //
                            F(listenip, useSocks, useHTTP, useTPROXY), //
                            F(tProxySettings, httpSettings, socksSettings, systemProxySettings))
    };
} // namespace Qv2ray::base::config
