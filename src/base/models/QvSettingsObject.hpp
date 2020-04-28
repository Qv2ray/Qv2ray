#pragma once
#include "3rdparty/x2struct/x2struct.hpp"
#include "base/models/CoreObjectModels.hpp"
#include "base/models/QvConfigIdentifier.hpp"

#include <chrono>

const int QV2RAY_CONFIG_VERSION = 11;

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
              Message("")
        {
        }
        XTOSTRUCT(O(Bold, Italic, ColorA, ColorR, ColorG, ColorB, Size, Family, Message, ContentType))
    };

    struct QvBarPage
    {
        int OffsetYpx;
        QList<QvBarLine> Lines;
        QvBarPage() : OffsetYpx(5)
        {
        }
        XTOSTRUCT(O(OffsetYpx, Lines))
    };

    struct Qv2rayToolBarConfig
    {
        QList<QvBarPage> Pages;
        XTOSTRUCT(O(Pages))
    };

    struct Qv2rayForwardProxyConfig
    {
        bool enableForwardProxy;
        QString type;
        QString serverAddress;
        int port;
        bool useAuth;
        QString username;
        QString password;
        Qv2rayForwardProxyConfig()
            : enableForwardProxy(false), type("http"), serverAddress("127.0.0.1"), port(8008), useAuth(false), username(), password()
        {
        }
        XTOSTRUCT(O(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
    };

    struct Qv2rayInboundsConfig
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

        Qv2rayInboundsConfig()
            : listenip("127.0.0.1"), setSystemProxy(true), useSocks(true), socks_port(1088), socks_useAuth(false), socksUDP(true),
              socksLocalIP("127.0.0.1"), socksAccount(), socksSniffing(false), useHTTP(true), http_port(8888), http_useAuth(false),
              httpAccount(), httpSniffing(false), useTPROXY(false), tproxy_ip("127.0.0.1"), tproxy_port(12345), tproxy_use_tcp(true),
              tproxy_use_udp(false), tproxy_followRedirect(true), tproxy_mode("tproxy"), dnsIntercept(true)
        {
        }

        XTOSTRUCT(O(setSystemProxy, listenip, useSocks, useHTTP, socks_port, socks_useAuth, socksAccount, socksSniffing, socksUDP, socksLocalIP,
                    http_port, http_useAuth, httpAccount, httpSniffing, useTPROXY, tproxy_ip, tproxy_port, tproxy_use_tcp, tproxy_use_udp,
                    tproxy_followRedirect, tproxy_mode, dnsIntercept))
    };

    struct Qv2rayOutboundsConfig
    {
        int mark;
        Qv2rayOutboundsConfig() : mark(255)
        {
        }
        XTOSTRUCT(O(mark))
    };

    struct Qv2rayUIConfig
    {
        QString theme;
        QString language;
        QList<QString> recentConnections;
        bool quietMode;
        bool useDarkTheme;
        bool useDarkTrayIcon;
        int maximumLogLines;
        int maxJumpListCount;
        Qv2rayUIConfig()
            : theme("Fusion"), language("en_US"), useDarkTheme(false), useDarkTrayIcon(true), maximumLogLines(500), maxJumpListCount(20)
        {
        }
        XTOSTRUCT(O(theme, language, quietMode, useDarkTheme, useDarkTrayIcon, maximumLogLines, maxJumpListCount, recentConnections))
    };

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
        XTOSTRUCT(O(proxy, block, direct))
    };

    struct Qv2rayRouteConfig
    {
        QString domainStrategy;
        Qv2rayRouteConfig_Impl domains;
        Qv2rayRouteConfig_Impl ips;
        friend bool operator==(const Qv2rayRouteConfig &left, const Qv2rayRouteConfig &right)
        {
            return left.domainStrategy == right.domainStrategy && left.domains == right.domains && left.ips == right.ips;
        }
        Qv2rayRouteConfig(){};
        Qv2rayRouteConfig(const Qv2rayRouteConfig_Impl &_domains, const Qv2rayRouteConfig_Impl &_ips, const QString &ds)
            : domainStrategy(ds), domains(_domains), ips(_ips){};
        XTOSTRUCT(O(domainStrategy, domains, ips))
    };

    struct Qv2rayPluginConfig
    {
        QMap<QString, bool> pluginStates;
        bool v2rayIntegration;
        int portAllocationStart;
        Qv2rayPluginConfig() : pluginStates(), v2rayIntegration(true), portAllocationStart(15000){};
        XTOSTRUCT(O(pluginStates, v2rayIntegration))
    };

    struct Qv2rayConnectionConfig
    {
        bool bypassCN;
        bool bypassBT;
        bool enableProxy;
        bool v2rayFreedomDNS;
        bool withLocalDNS;
        Qv2rayRouteConfig routeConfig;
        QList<QString> dnsList;
        Qv2rayForwardProxyConfig forwardProxyConfig;
        Qv2rayConnectionConfig()
            : bypassCN(true), bypassBT(false), enableProxy(true), v2rayFreedomDNS(false), withLocalDNS(false), routeConfig(),
              dnsList(QStringList{ "8.8.4.4", "1.1.1.1" })
        {
        }
        XTOSTRUCT(O(bypassCN, bypassBT, enableProxy, v2rayFreedomDNS, withLocalDNS, dnsList, forwardProxyConfig, routeConfig))
    };

    struct Qv2rayAPIConfig
    {
        bool enableAPI;
        int statsPort;
        Qv2rayAPIConfig() : enableAPI(true), statsPort(15490)
        {
        }
        XTOSTRUCT(O(enableAPI, statsPort))
    };

    struct Qv2rayKernelConfig
    {
        QString v2CorePath_linux;
        QString v2AssetsPath_linux;
        QString v2CorePath_macx;
        QString v2AssetsPath_macx;
        QString v2CorePath_win;
        QString v2AssetsPath_win; //
        Qv2rayKernelConfig()
            : v2CorePath_linux(), v2AssetsPath_linux(), //
              v2CorePath_macx(), v2AssetsPath_macx(),   //
              v2CorePath_win(), v2AssetsPath_win()      //
        {
        }
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

        XTOSTRUCT(O(v2CorePath_linux, v2AssetsPath_linux, v2CorePath_macx, v2AssetsPath_macx, v2CorePath_win, v2AssetsPath_win))
    };

    struct Qv2rayUpdateConfig
    {
        QString ignoredVersion;
        ///
        /// \brief updateChannel
        /// 0: Stable
        /// 1: Testing
        int updateChannel;
        XTOSTRUCT(O(ignoredVersion, updateChannel))
    };

    struct Qv2rayAdvancedConfig
    {
        bool setAllowInsecure;
        bool setAllowInsecureCiphers;
        bool testLatencyPeriodcally;
        XTOSTRUCT(O(setAllowInsecure, setAllowInsecureCiphers, testLatencyPeriodcally))
    };

    struct Qv2rayNetworkConfig
    {
        enum Qv2rayProxyType
        {
            QVPROXY_NONE,
            QVPROXY_SYSTEM,
            QVPROXY_CUSTOM
        } proxyType;

        QString address;
        QString type;
        int port;
        QString userAgent;
        Qv2rayNetworkConfig()
            : proxyType(QVPROXY_NONE), //
              address("127.0.0.1"),    //
              type("http"),            //
              port(8000),              //
              userAgent("Qv2ray/$VERSION WebRequestHelper"){};
        XTOSTRUCT(O(proxyType, type, address, port, userAgent))
    };

    struct Qv2rayConfig
    {
        int config_version;
        bool tProxySupport;
        int logLevel;
        //
        QString autoStartId;
        //
        // Key = groupId, connectionId
        QMap<QString, GroupObject_Config> groups;
        QMap<QString, SubscriptionObject_Config> subscriptions;
        /// Connections are used privately.
        QMap<QString, ConnectionObject_Config> connections;
        //
        Qv2rayUIConfig uiConfig;
        Qv2rayAPIConfig apiConfig;
        Qv2rayPluginConfig pluginConfig;
        Qv2rayKernelConfig kernelConfig;
        Qv2rayUpdateConfig updateConfig;
        Qv2rayNetworkConfig networkConfig;
        Qv2rayToolBarConfig toolBarConfig;
        Qv2rayInboundsConfig inboundConfig;
        Qv2rayOutboundsConfig outboundConfig;
        Qv2rayAdvancedConfig advancedConfig;
        Qv2rayConnectionConfig connectionConfig;

        Qv2rayConfig()
            : config_version(QV2RAY_CONFIG_VERSION), //
              tProxySupport(false),                  //
              logLevel(),                            //
              autoStartId("null"),                   //
              groups(),                              //
              subscriptions(),                       //
              connections(),                         //
              uiConfig(),                            //
              apiConfig(),                           //
              pluginConfig(),                        //
              kernelConfig(),                        //
              updateConfig(),                        //
              networkConfig(),                       //
              toolBarConfig(),                       //
              inboundConfig(),                       //
              outboundConfig(),                      //
              advancedConfig(),                      //
              connectionConfig()
        {
        }

        XTOSTRUCT(O(config_version,   //
                    tProxySupport,    //
                    logLevel,         //
                    uiConfig,         //
                    pluginConfig,     //
                    updateConfig,     //
                    kernelConfig,     //
                    networkConfig,    //
                    groups,           //
                    connections,      //
                    subscriptions,    //
                    autoStartId,      //
                    inboundConfig,    //
                    outboundConfig,   //
                    connectionConfig, //
                    toolBarConfig,    //
                    advancedConfig,   //
                    apiConfig         //
                    ))
    };
} // namespace Qv2ray::base::config
