#pragma once
#include "base/models/CoreObjectModels.hpp"
#include "base/models/QvConfigIdentifier.hpp"
#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <chrono>

const int QV2RAY_CONFIG_VERSION = 12;

namespace Qv2ray::base::config
{
    struct QvBarLine
    {
        QString Family;
        bool Bold, Italic;
        int ColorA, ColorR, ColorG, ColorB;
        int ContentType;
        double Size;
        QString Message;
        QvBarLine()
            : Family("Consolas"), Bold(true), Italic(false), ColorA(255), ColorR(255), ColorG(255), ColorB(255), ContentType(0), Size(9),
              Message(""){};
        JSONSTRUCT_REGISTER(QvBarLine, F(Bold, Italic, ColorA, ColorR, ColorG, ColorB, Size, Family, Message, ContentType))
    };

    struct QvBarPage
    {
        int OffsetYpx;
        QList<QvBarLine> Lines;
        QvBarPage() : OffsetYpx(5){};
        JSONSTRUCT_REGISTER(QvBarPage, F(OffsetYpx, Lines))
    };

    struct Qv2rayConfig_ToolBar
    {
        QList<QvBarPage> Pages;
        JSONSTRUCT_REGISTER(Qv2rayConfig_ToolBar, F(Pages))
    };

    struct Qv2rayConfig_ForwardProxy
    {
        bool enableForwardProxy;
        QString type;
        QString serverAddress;
        int port;
        bool useAuth;
        QString username;
        QString password;
        Qv2rayConfig_ForwardProxy()
            : enableForwardProxy(false), type("http"), serverAddress("127.0.0.1"), port(8008), useAuth(false), username(), password(){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_ForwardProxy, F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };

    struct Qv2rayConfig_Inbounds
    {
        QString listenip;
        bool setSystemProxy;

        // SOCKS
        bool useSocks;
        int socks_port;
        bool socks_useAuth;
        bool socksUDP;
        QString socksLocalIP;
        objects::AccountObject socksAccount;
        bool socksSniffing;
        // HTTP
        bool useHTTP;
        int http_port;
        bool http_useAuth;
        objects::AccountObject httpAccount;
        bool httpSniffing;

        // dokodemo-door transparent proxy
        bool useTPROXY;
        QString tproxy_ip;
        int tproxy_port;
        bool tproxy_use_tcp;
        bool tproxy_use_udp;
        bool tproxy_followRedirect;
        /*redirect or tproxy way, and tproxy need cap_net_admin*/
        QString tproxy_mode;
        bool dnsIntercept;

        Qv2rayConfig_Inbounds()
            : listenip("127.0.0.1"), setSystemProxy(true), useSocks(true), socks_port(1089), socks_useAuth(false), socksUDP(true),
              socksLocalIP("127.0.0.1"), socksAccount(), socksSniffing(false), useHTTP(true), http_port(8889), http_useAuth(false),
              httpAccount(), httpSniffing(false), useTPROXY(false), tproxy_ip("127.0.0.1"), tproxy_port(12345), tproxy_use_tcp(true),
              tproxy_use_udp(false), tproxy_followRedirect(true), tproxy_mode("tproxy"), dnsIntercept(true){};

        JSONSTRUCT_REGISTER(Qv2rayConfig_Inbounds,
                            F(setSystemProxy, listenip, useSocks, useHTTP, socks_port, socks_useAuth, socksAccount, socksSniffing, socksUDP,
                              socksLocalIP, http_port, http_useAuth, httpAccount, httpSniffing, useTPROXY),
                            F(tproxy_ip, tproxy_port, tproxy_use_tcp, tproxy_use_udp, tproxy_followRedirect, tproxy_mode, dnsIntercept))
    };

    struct Qv2rayConfig_Outbounds
    {
        int mark;
        Qv2rayConfig_Outbounds() : mark(255){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Outbounds, F(mark))
    };

    struct Qv2rayConfig_UI
    {
        QString theme;
        QString language;
        QList<ConnectionGroupPair> recentConnections;
        bool quietMode;
        bool useDarkTheme;
        bool useDarkTrayIcon;
        int maximumLogLines;
        int maxJumpListCount;
        Qv2rayConfig_UI()
            : theme("Fusion"), language("en_US"), useDarkTheme(false), useDarkTrayIcon(true), maximumLogLines(500), maxJumpListCount(20){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_UI,
                            F(theme, language, quietMode, useDarkTheme, useDarkTrayIcon, maximumLogLines, maxJumpListCount, recentConnections))
    };

    struct Qv2rayConfig_Routing
    {
        struct Qv2rayRouteConfig_Impl
        {
            QList<QString> direct;
            QList<QString> block;
            QList<QString> proxy;
            Qv2rayRouteConfig_Impl(){};
            friend bool operator==(const Qv2rayRouteConfig_Impl &left, const Qv2rayRouteConfig_Impl &right)
            {
                return left.direct == right.direct && left.block == right.block && left.proxy == left.proxy;
            }
            Qv2rayRouteConfig_Impl(const QList<QString> &_direct, const QList<QString> &_block, const QList<QString> &_proxy)
                : direct(_direct), block(_block), proxy(_proxy){};
            JSONSTRUCT_REGISTER(Qv2rayRouteConfig_Impl, F(proxy, block, direct))
        };
        QString domainStrategy;
        Qv2rayRouteConfig_Impl domains;
        Qv2rayRouteConfig_Impl ips;
        friend bool operator==(const Qv2rayConfig_Routing &left, const Qv2rayConfig_Routing &right)
        {
            return left.domainStrategy == right.domainStrategy && left.domains == right.domains && left.ips == right.ips;
        }
        Qv2rayConfig_Routing(){};
        Qv2rayConfig_Routing(const Qv2rayRouteConfig_Impl &_domains, const Qv2rayRouteConfig_Impl &_ips, const QString &ds)
            : domainStrategy(ds), domains(_domains), ips(_ips){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Routing, F(domainStrategy, domains, ips))
    };

    struct Qv2rayConfig_Plugin
    {
        QMap<QString, bool> pluginStates;
        bool v2rayIntegration;
        int portAllocationStart;
        Qv2rayConfig_Plugin() : pluginStates(), v2rayIntegration(true), portAllocationStart(15000){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Plugin, F(pluginStates, v2rayIntegration, portAllocationStart))
    };

    struct Qv2rayConfig_Connection
    {
        bool bypassCN;
        bool bypassBT;
        bool enableProxy;
        bool v2rayFreedomDNS;
        bool withLocalDNS;
        Qv2rayConfig_Routing routeConfig;
        QList<QString> dnsList;
        Qv2rayConfig_ForwardProxy forwardProxyConfig;
        Qv2rayConfig_Connection()
            : bypassCN(true), bypassBT(false), enableProxy(true), v2rayFreedomDNS(false), withLocalDNS(false), routeConfig(),
              dnsList(QStringList{ "8.8.4.4", "1.1.1.1" }){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Connection,
                            F(bypassCN, bypassBT, enableProxy, v2rayFreedomDNS, withLocalDNS, dnsList, forwardProxyConfig, routeConfig))
    };
    struct Qv2rayConfig_Kernel
    {
        bool enableAPI;
        int statsPort;
        //
        QString v2CorePath_linux;
        QString v2AssetsPath_linux;
        QString v2CorePath_macx;
        QString v2AssetsPath_macx;
        QString v2CorePath_win;
        QString v2AssetsPath_win;
        Qv2rayConfig_Kernel()
            : v2CorePath_linux(), v2AssetsPath_linux(), //
              v2CorePath_macx(), v2AssetsPath_macx(),   //
              v2CorePath_win(), v2AssetsPath_win()      //
              {};
        //
#ifdef Q_OS_LINUX
    #define _VARNAME_VCOREPATH_ v2CorePath_linux
    #define _VARNAME_VASSETSPATH_ v2AssetsPath_linux
#elif defined(Q_OS_MACOS)
    #define _VARNAME_VCOREPATH_ v2CorePath_macx
    #define _VARNAME_VASSETSPATH_ v2AssetsPath_macx
#elif defined(Q_OS_WIN)
    #define _VARNAME_VCOREPATH_ v2CorePath_win
    #define _VARNAME_VASSETSPATH_ v2AssetsPath_win
#endif

        inline const QString KernelPath(const QString &path = "")
        {
            return path.isEmpty() ? _VARNAME_VCOREPATH_ : _VARNAME_VCOREPATH_ = path;
        }
        inline const QString AssetsPath(const QString &path = "")
        {
            return path.isEmpty() ? _VARNAME_VASSETSPATH_ : _VARNAME_VASSETSPATH_ = path;
        }

#undef _VARNAME_VCOREPATH_
#undef _VARNAME_VASSETSPATH_

        JSONSTRUCT_REGISTER(Qv2rayConfig_Kernel,                     //
                            F(enableAPI, statsPort),                 //
                            F(v2CorePath_linux, v2AssetsPath_linux), //
                            F(v2CorePath_macx, v2AssetsPath_macx),   //
                            F(v2CorePath_win, v2AssetsPath_win))
    };

    struct Qv2rayConfig_Update
    {
        QString ignoredVersion;
        ///
        /// \brief updateChannel
        /// 0: Stable
        /// 1: Testing
        int updateChannel;
        JSONSTRUCT_REGISTER(Qv2rayConfig_Update, F(ignoredVersion, updateChannel))
    };

    struct Qv2rayConfig_Advanced
    {
        bool setAllowInsecure;
        bool setAllowInsecureCiphers;
        bool testLatencyPeriodcally;
        JSONSTRUCT_REGISTER(Qv2rayConfig_Advanced, F(setAllowInsecure, setAllowInsecureCiphers, testLatencyPeriodcally))
    };

    struct Qv2rayConfig_Network
    {
        enum Qv2rayProxyType : int
        {
            QVPROXY_NONE = 0,
            QVPROXY_SYSTEM = 1,
            QVPROXY_CUSTOM = 2
        } proxyType;

        QString address;
        QString type;
        int port;
        QString userAgent;
        Qv2rayConfig_Network()
            : proxyType(QVPROXY_NONE), //
              address("127.0.0.1"),    //
              type("http"),            //
              port(8000),              //
              userAgent("Qv2ray/$VERSION WebRequestHelper"){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Network, F(proxyType, type, address, port, userAgent))
    };

    enum Qv2rayAutoConnectionBehavior
    {
        AUTO_CONNECTION_NONE = 0,
        AUTO_CONNECTION_FIXED = 1,
        AUTO_CONNECTION_LAST_CONNECTED = 2
    };

    struct Qv2rayConfigObject
    {
        int config_version;
        bool tProxySupport;
        int logLevel;
        //
        ConnectionGroupPair autoStartId;
        Qv2rayAutoConnectionBehavior autoStartBehavior;
        //
        // Key = groupId, connectionId
        //        QList<GroupId> groups;
        //        QList<ConnectionId> connections;
        //
        Qv2rayConfig_UI uiConfig;
        Qv2rayConfig_Plugin pluginConfig;
        Qv2rayConfig_Kernel kernelConfig;
        Qv2rayConfig_Update updateConfig;
        Qv2rayConfig_Network networkConfig;
        Qv2rayConfig_ToolBar toolBarConfig;
        Qv2rayConfig_Inbounds inboundConfig;
        Qv2rayConfig_Outbounds outboundConfig;
        Qv2rayConfig_Advanced advancedConfig;
        Qv2rayConfig_Connection connectionConfig;

        Qv2rayConfigObject()
            : config_version(QV2RAY_CONFIG_VERSION), //
              tProxySupport(false),                  //
              logLevel(),                            //
              autoStartId(),                         //
              autoStartBehavior(),                   //
              uiConfig(),                            //
              pluginConfig(),                        //
              kernelConfig(),                        //
              updateConfig(),                        //
              networkConfig(),                       //
              toolBarConfig(),                       //
              inboundConfig(),                       //
              outboundConfig(),                      //
              advancedConfig(),                      //
              connectionConfig(){};

        JSONSTRUCT_REGISTER(Qv2rayConfigObject,                                                                   //
                            F(config_version, tProxySupport, autoStartId, autoStartBehavior, logLevel),           //
                            F(uiConfig, advancedConfig, pluginConfig, updateConfig, kernelConfig, networkConfig), //
                            F(inboundConfig, outboundConfig, connectionConfig),                                   //
                            F(toolBarConfig))
    };
} // namespace Qv2ray::base::config
