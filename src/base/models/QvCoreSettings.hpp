#pragma once
#include "base/models/CoreObjectModels.hpp"
#include "libs/QJsonStruct/QJsonStruct.hpp"
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
        int mark;
        QvConfig_Outbounds() : mark(255){};
        JSONSTRUCT_REGISTER(QvConfig_Outbounds, F(mark))
    };

    struct QvConfig_ForwardProxy
    {
        bool enableForwardProxy;
        QString type;
        QString serverAddress;
        int port;
        bool useAuth;
        QString username;
        QString password;
        QvConfig_ForwardProxy()
            : enableForwardProxy(false), type("http"), serverAddress("127.0.0.1"), port(8008), useAuth(false), username(), password(){};
        JSONSTRUCT_REGISTER(QvConfig_ForwardProxy, F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };

    struct QvConfig_Connection
    {
        bool enableProxy;
        bool bypassCN;
        bool bypassBT;
        bool v2rayFreedomDNS;
        bool withLocalDNS;
        bool dnsIntercept;
        QvConfig_Connection()
            : enableProxy(true), bypassCN(true), bypassBT(false), v2rayFreedomDNS(false), withLocalDNS(false), dnsIntercept(false){};
        JSONSTRUCT_REGISTER(QvConfig_Connection, F(bypassCN, bypassBT, enableProxy, v2rayFreedomDNS, withLocalDNS, dnsIntercept))
    };

    struct QvConfig_SystemProxy
    {
        bool setSystemProxy;
        QvConfig_SystemProxy() : setSystemProxy(true){};
        JSONSTRUCT_REGISTER(QvConfig_SystemProxy, F(setSystemProxy))
    };

    struct __Qv2rayConfig_ProtocolInboundBase
    {
        int port;
        bool useAuth;
        bool sniffing;
        objects::AccountObject account;
        __Qv2rayConfig_ProtocolInboundBase(int _port = 0) : port(_port), useAuth(false), sniffing(false), account(){};
        JSONSTRUCT_REGISTER(__Qv2rayConfig_ProtocolInboundBase, F(port, useAuth, sniffing, account))
    };

    struct QvConfig_SocksInbound : __Qv2rayConfig_ProtocolInboundBase
    {
        bool enableUDP;
        QString localIP;
        QvConfig_SocksInbound() : __Qv2rayConfig_ProtocolInboundBase(1089), enableUDP(true), localIP("127.0.0.1"){};
        JSONSTRUCT_REGISTER(QvConfig_SocksInbound, B(__Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
    };

    struct QvConfig_HttpInbound : __Qv2rayConfig_ProtocolInboundBase
    {
        QvConfig_HttpInbound() : __Qv2rayConfig_ProtocolInboundBase(8889){};
        JSONSTRUCT_REGISTER(QvConfig_HttpInbound, B(__Qv2rayConfig_ProtocolInboundBase))
    };

    struct QvConfig_TProxy
    {
        QString tProxyIP;
        QString tProxyV6IP;
        int port;
        bool hasTCP;
        bool hasUDP;
        QString mode;
        QvConfig_TProxy()
            : tProxyIP("127.0.0.1"), //
              tProxyV6IP(""),        //
              port(12345),           //
              hasTCP(true),          //
              hasUDP(false),         //
              mode("tproxy")         //
              {};
        JSONSTRUCT_REGISTER(QvConfig_TProxy, F(tProxyIP, tProxyV6IP, port, hasTCP, hasUDP, mode))
    };

    struct QvConfig_Inbounds
    {
        QString listenip;
        bool useSocks;
        bool useHTTP;
        bool useTPROXY;
        //
        QvConfig_TProxy tProxySettings;
        QvConfig_HttpInbound httpSettings;
        QvConfig_SocksInbound socksSettings;
        QvConfig_SystemProxy systemProxySettings;
        QvConfig_Inbounds() : listenip("127.0.0.1"), useSocks(true), useHTTP(true), useTPROXY(false){};

        JSONSTRUCT_REGISTER(QvConfig_Inbounds,                         //
                            F(listenip, useSocks, useHTTP, useTPROXY), //
                            F(tProxySettings, httpSettings, socksSettings, systemProxySettings))
    };
} // namespace Qv2ray::base::config
