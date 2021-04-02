#pragma once
#include "base/models/QvConfigIdentifier.hpp"
#include "base/models/QvCoreSettings.hpp"
#include "base/models/QvSafeType.hpp"

#include <chrono>

constexpr int QV2RAY_CONFIG_VERSION = 15;

namespace Qv2ray::base::config
{
    struct QvGraphPenConfig
    {
        Q_GADGET
        QJS_CONSTRUCTOR(QvGraphPenConfig)
        QJS_PROP_D(int, R, 150)
        QJS_PROP_D(int, G, 150)
        QJS_PROP_D(int, B, 150)
        QJS_PROP_D(float, width, 1.5f);
        QJS_PROP_D(Qt::PenStyle, style, Qt::SolidLine);

        QvGraphPenConfig(int R, int G, int B, float w, Qt::PenStyle s)
        {
            this->R = R;
            this->G = G;
            this->B = B;
            this->width = w;
            this->style = s;
        };
        QJS_FUNCTION(F(R, G, B, width, style))
    };

    struct Qv2rayConfig_Graph
    {
        typedef safetype::QvEnumMap<StatisticsType, safetype::QvPair<QvGraphPenConfig>> colorconfig_t;
        Q_GADGET
        QJS_CONSTRUCTOR(Qv2rayConfig_Graph)
        QJS_PROP_D(bool, useOutboundStats, true);
        QJS_PROP_D(bool, hasDirectStats, true);
        QJS_PROP(colorconfig_t, colorConfig);
        QJS_FUNCTION(F(useOutboundStats, hasDirectStats, colorConfig))
    };

    struct Qv2rayConfig_UI
    {
        Q_GADGET
        QJS_CONSTRUCTOR(Qv2rayConfig_UI)
#ifdef Q_OS_WIN
        QJS_PROP_D(QString, theme, "windowsvista");
#elif defined(Q_OS_MACOS)
        QJS_PROP_D(QString, theme, "macintosh");
#else
        QJS_PROP_D(QString, theme, "Fusion");
#endif
        QJS_PROP_D(QString, language, "en_US");
        QJS_PROP(QList<ConnectionGroupPair>, recentConnections);
        QJS_PROP(Qv2rayConfig_Graph, graphConfig);
        QJS_PROP_D(bool, quietMode, false);
        QJS_PROP_D(bool, useDarkTheme, false);
        QJS_PROP_D(bool, useGlyphTrayIcon, true);
        QJS_PROP_D(bool, useDarkTrayIcon, false);
        QJS_PROP_D(bool, startMinimized, true);
        QJS_PROP_D(int, maximumLogLines, 500);
        QJS_PROP_D(int, maxJumpListCount, 20);
        QJS_FUNCTION(F(theme, language, quietMode, graphConfig, useDarkTheme, useDarkTrayIcon, useGlyphTrayIcon, maximumLogLines, maxJumpListCount,
                       recentConnections, startMinimized))
    };

    struct Qv2rayConfig_Plugin
    {
        typedef QMap<QString, bool> plugin_states_t;
        Q_GADGET
        QJS_CONSTRUCTOR(Qv2rayConfig_Plugin)
        QJS_PROP(plugin_states_t, pluginStates);
        QJS_PROP_D(bool, v2rayIntegration, true);
        QJS_PROP_D(int, portAllocationStart, 15000);
        QJS_FUNCTION(F(pluginStates, v2rayIntegration, portAllocationStart))
    };

    struct Qv2rayConfig_Kernel
    {
        Q_GADGET
        QJS_CONSTRUCTOR(Qv2rayConfig_Kernel)
        QJS_PROP_D(bool, enableAPI, true);
        QJS_PROP_D(int, statsPort, 15490);
        //
        QJS_PROP(QString, v2CorePath_linux);
        QJS_PROP(QString, v2AssetsPath_linux);
        QJS_PROP(QString, v2CorePath_macx);
        QJS_PROP(QString, v2AssetsPath_macx);
        QJS_PROP(QString, v2CorePath_win);
        QJS_PROP(QString, v2AssetsPath_win);

#ifdef Q_OS_LINUX
#define VARNAME_VCOREPATH v2CorePath_linux
#define VARNAME_VASSETSPATH v2AssetsPath_linux
#elif defined(Q_OS_MACOS)
#define VARNAME_VCOREPATH v2CorePath_macx
#define VARNAME_VASSETSPATH v2AssetsPath_macx
#elif defined(Q_OS_WIN)
#define VARNAME_VCOREPATH v2CorePath_win
#define VARNAME_VASSETSPATH v2AssetsPath_win
#endif

        inline const QString KernelPath(const QString &path = "")
        {
            if (path.isEmpty())
                return VARNAME_VCOREPATH;

            VARNAME_VCOREPATH = path;
            return path;
        }
        inline const QString AssetsPath(const QString &path = "")
        {
            if (path.isEmpty())
                return VARNAME_VASSETSPATH;

            VARNAME_VASSETSPATH = path;
            return path;
        }

#undef VARNAME_VCOREPATH
#undef VARNAME_VASSETSPATH

        QJS_FUNCTION(F(enableAPI, statsPort),                 //
                     F(v2CorePath_linux, v2AssetsPath_linux), //
                     F(v2CorePath_macx, v2AssetsPath_macx),   //
                     F(v2CorePath_win, v2AssetsPath_win))
    };

    enum Qv2rayUpdateChannel
    {
        CHANNEL_STABLE = 0,
        CHANNEL_TESTING = 1
    };

    struct Qv2rayConfig_Update
    {
        Q_GADGET
        QJS_CONSTRUCTOR(Qv2rayConfig_Update)
        QJS_PROP_D(Qv2rayUpdateChannel, updateChannel, CHANNEL_STABLE);
        QJS_PROP(QString, ignoredVersion);
        QJS_FUNCTION(F(ignoredVersion, updateChannel))
    };

    enum Qv2rayLatencyTestingMethod
    {
        TCPING = 0,
        ICMPING = 1,
        REALPING = 2
    };

    enum Qv2rayProxyType
    {
        QVPROXY_NONE = 0,
        QVPROXY_SYSTEM = 1,
        QVPROXY_CUSTOM = 2
    };

    struct Qv2rayConfig_Network
    {
        Q_GADGET
        QJS_CONSTRUCTOR(Qv2rayConfig_Network)
        QJS_PROP_D(Qv2rayLatencyTestingMethod, latencyTestingMethod, TCPING);
        QJS_PROP_D(QString, latencyRealPingTestURL, "https://www.google.com");
        QJS_PROP_D(Qv2rayProxyType, proxyType, QVPROXY_NONE);
        QJS_PROP_D(QString, address, "127.0.0.1");
        QJS_PROP_D(QString, type, "http");
        QJS_PROP_D(int, port, 8000);
        QJS_PROP_D(QString, userAgent, "Qv2ray/$VERSION WebRequestHelper");
        QJS_FUNCTION(F(latencyTestingMethod, latencyRealPingTestURL, proxyType, type, address, port, userAgent))
    };

    enum Qv2rayAutoConnectionBehavior
    {
        AUTO_CONNECTION_NONE = 0,
        AUTO_CONNECTION_FIXED = 1,
        AUTO_CONNECTION_LAST_CONNECTED = 2
    };

    struct Qv2rayConfigObject
    {
        Q_GADGET
        QJS_CONSTRUCTOR(Qv2rayConfigObject)
        QJS_PROP_D(int, config_version, QV2RAY_CONFIG_VERSION, REQUIRED);
        QJS_PROP_D(int, logLevel, 0, REQUIRED);
        //
        QJS_PROP(ConnectionGroupPair, autoStartId, REQUIRED);
        QJS_PROP(ConnectionGroupPair, lastConnectedId, REQUIRED);
        QJS_PROP_D(Qv2rayAutoConnectionBehavior, autoStartBehavior, AUTO_CONNECTION_NONE, REQUIRED);
        //
        QJS_PROP(Qv2rayConfig_UI, uiConfig, REQUIRED);
        QJS_PROP(Qv2rayConfig_Plugin, pluginConfig, REQUIRED);
        QJS_PROP(Qv2rayConfig_Kernel, kernelConfig, REQUIRED);
        QJS_PROP(Qv2rayConfig_Update, updateConfig, REQUIRED);
        QJS_PROP(Qv2rayConfig_Network, networkConfig, REQUIRED);
        QJS_PROP(QvConfig_Inbounds, inboundConfig, REQUIRED);
        QJS_PROP(QvConfig_Outbounds, outboundConfig, REQUIRED);
        QJS_PROP(GroupRoutingConfig, defaultRouteConfig, REQUIRED);

        QJS_FUNCTION(F(uiConfig, config_version, autoStartId, lastConnectedId, autoStartBehavior, logLevel), //
                     F(pluginConfig, updateConfig, kernelConfig, networkConfig),                             //
                     F(inboundConfig, outboundConfig, defaultRouteConfig))
    };
} // namespace Qv2ray::base::config
