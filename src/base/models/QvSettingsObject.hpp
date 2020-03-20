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

    struct Qv2rayPACConfig
    {
        bool enablePAC;
        int port;
        QString localIP;
        bool useSocksProxy;
        Qv2rayPACConfig() : enablePAC(false), port(8989), useSocksProxy(false)
        {
        }
        XTOSTRUCT(O(enablePAC, port, localIP, useSocksProxy))
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
        Qv2rayPACConfig pacConfig;

        // SOCKS
        bool useSocks;
        int socks_port;
        bool socks_useAuth;
        bool socksUDP;
        QString socksLocalIP;
        objects::AccountObject socksAccount;
        // HTTP
        bool useHTTP;
        int http_port;
        bool http_useAuth;
        objects::AccountObject httpAccount;

        Qv2rayInboundsConfig()
            : listenip("127.0.0.1"), setSystemProxy(true), pacConfig(), useSocks(true), socks_port(1088), socks_useAuth(false), socksUDP(true),
              socksLocalIP("127.0.0.1"), socksAccount(), useHTTP(true), http_port(8888), http_useAuth(false), httpAccount()
        {
        }

        XTOSTRUCT(O(setSystemProxy, pacConfig, listenip, useSocks, useHTTP, socks_port, socks_useAuth, socksAccount, socksUDP, socksLocalIP,
                    http_port, http_useAuth, httpAccount))
    };

    struct Qv2rayUIConfig
    {
        QString theme;
        QString language;
        bool useDarkTheme;
        bool useDarkTrayIcon;
        int maximumLogLines;
        Qv2rayUIConfig() : theme("Fusion"), language("en_US"), useDarkTheme(false), useDarkTrayIcon(true), maximumLogLines(500)
        {
        }
        XTOSTRUCT(O(theme, language, useDarkTheme, useDarkTrayIcon, maximumLogLines))
    };

    struct Qv2rayRouteConfig_Impl
    {
        QList<QString> proxy;
        QList<QString> block;
        QList<QString> direct;
        XTOSTRUCT(O(proxy, block, direct))
    };

    struct Qv2rayRouteConfig
    {
        Qv2rayRouteConfig_Impl domains;
        Qv2rayRouteConfig_Impl ips;
        XTOSTRUCT(O(domains, ips))
    };

    struct Qv2rayConnectionConfig
    {
        bool bypassCN;
        bool enableProxy;
        bool withLocalDNS;
        Qv2rayRouteConfig routeConfig;
        QList<QString> dnsList;
        Qv2rayForwardProxyConfig forwardProxyConfig;
        Qv2rayConnectionConfig()
            : bypassCN(true), enableProxy(true), withLocalDNS(false), routeConfig(), dnsList(QStringList{ "8.8.4.4", "1.1.1.1" })
        {
        }
        XTOSTRUCT(O(bypassCN, enableProxy, withLocalDNS, dnsList, forwardProxyConfig, routeConfig))
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

    struct Qv2rayConfig
    {
        int config_version;
        bool tProxySupport;
        int logLevel;
        //
        QString ignoredVersion;
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
        Qv2rayKernelConfig kernelConfig;
        Qv2rayToolBarConfig toolBarConfig;
        Qv2rayInboundsConfig inboundConfig;
        Qv2rayConnectionConfig connectionConfig;

        Qv2rayConfig()
            : config_version(QV2RAY_CONFIG_VERSION), tProxySupport(false), logLevel(), ignoredVersion(), autoStartId("null"), groups(),
              subscriptions(), connections(), uiConfig(), apiConfig(), kernelConfig(), toolBarConfig(), inboundConfig(), connectionConfig()
        {
        }

        XTOSTRUCT(O(config_version, ignoredVersion, tProxySupport, logLevel, uiConfig, kernelConfig, groups, connections, subscriptions,
                    autoStartId, inboundConfig, connectionConfig, toolBarConfig, apiConfig))
    };
} // namespace Qv2ray::base::config
