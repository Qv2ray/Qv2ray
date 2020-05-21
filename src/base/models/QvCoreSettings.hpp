#pragma once
#include "base/models/CoreObjectModels.hpp"
#include "libs/QJsonStruct/QJsonStruct.hpp"
namespace Qv2ray::base::config
{
    struct Qv2rayConfig_DNS
    {
        QMap<QString, QString> hosts;
    };
    struct Qv2rayConfig_SystemProxy
    {
        bool setSystemProxy;
        Qv2rayConfig_SystemProxy() : setSystemProxy(true){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_SystemProxy, F(setSystemProxy))
    };

    struct Qv2rayConfig_ProtocolInboundBase
    {
        int port;
        bool useAuth;
        bool sniffing;
        objects::AccountObject account;
        Qv2rayConfig_ProtocolInboundBase(int _port = 0) : port(_port), useAuth(false), sniffing(false), account(){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_ProtocolInboundBase, F(port, useAuth, sniffing, account))
    };

    struct Qv2rayConfig_SocksInbound : Qv2rayConfig_ProtocolInboundBase
    {
        bool enableUDP;
        QString localIP;
        Qv2rayConfig_SocksInbound() : Qv2rayConfig_ProtocolInboundBase(1089), enableUDP(true), localIP("127.0.0.1"){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
    };

    struct Qv2rayConfig_HttpInbound : Qv2rayConfig_ProtocolInboundBase
    {
        Qv2rayConfig_HttpInbound() : Qv2rayConfig_ProtocolInboundBase(8889){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
    };

    struct Qv2rayConfig_tProxy
    {
        QString tProxyIP;
        int port;
        bool hasTCP;
        bool hasUDP;
        bool followRedirect;
        QString mode;
        bool dnsIntercept;
        Qv2rayConfig_tProxy()
            : tProxyIP("127.0.0.1"), port(12345), hasTCP(true), hasUDP(false), followRedirect(true), mode("tproxy"), dnsIntercept(true){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_tProxy, F(tProxyIP, port, hasTCP, hasUDP, followRedirect, mode, dnsIntercept))
    };

    struct Qv2rayConfig_Inbounds
    {
        QString listenip;
        bool useSocks;
        bool useHTTP;
        bool useTPROXY;
        //
        Qv2rayConfig_tProxy tProxySettings;
        Qv2rayConfig_HttpInbound httpSettings;
        Qv2rayConfig_SocksInbound socksSettings;
        Qv2rayConfig_SystemProxy systemProxySettings;
        Qv2rayConfig_Inbounds() : listenip("127.0.0.1"), useSocks(true), useHTTP(true), useTPROXY(false){};

        JSONSTRUCT_REGISTER(Qv2rayConfig_Inbounds,                     //
                            F(listenip, useSocks, useHTTP, useTPROXY), //
                            F(tProxySettings, httpSettings, socksSettings, systemProxySettings))
    };

    struct Qv2rayConfig_Outbounds
    {
        int mark;
        Qv2rayConfig_Outbounds() : mark(255){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Outbounds, F(mark))
    };

} // namespace Qv2ray::base::config
