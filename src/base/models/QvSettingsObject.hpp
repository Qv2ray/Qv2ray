#pragma once
#include "base/models/QvConfigIdentifier.hpp"
#include "base/models/QvCoreSettings.hpp"
#include "base/models/QvSafeType.hpp"

#include <chrono>

constexpr int QV2RAY_CONFIG_VERSION = 14;

namespace Qv2ray::base::config
{
    struct QvGraphPenConfig
    {
        int R, G, B;
        float width;
        Qt::PenStyle style;
        QvGraphPenConfig() : R(150), G(150), B(150), width(1.5f), style(Qt::SolidLine){};
        QvGraphPenConfig(int R, int G, int B, float w, Qt::PenStyle s)
        {
            this->R = R;
            this->G = G;
            this->B = B;
            this->width = w;
            this->style = s;
        };
        JSONSTRUCT_REGISTER(QvGraphPenConfig, F(R, G, B, width, style))
    };

    struct Qv2rayConfig_Graph
    {
        bool useOutboundStats;
        bool hasDirectStats;
        Qv2rayConfig_Graph() : useOutboundStats(false), hasDirectStats(false){};
        safetype::QvEnumMap<StatisticsType, safetype::QvPair<QvGraphPenConfig>> colorConfig;
        JSONSTRUCT_REGISTER(Qv2rayConfig_Graph, F(useOutboundStats, hasDirectStats, colorConfig))
    };

    struct Qv2rayConfig_UI
    {
        QString theme;
        QString language;
        QList<ConnectionGroupPair> recentConnections;
        Qv2rayConfig_Graph graphConfig;
        bool quietMode;
        bool useDarkTheme;
        bool useDarkTrayIcon;
        int maximumLogLines;
        int maxJumpListCount;
        bool useOldShareLinkFormat;
        Qv2rayConfig_UI()
            : theme("Fusion"),            //
              language("en_US"),          //
              useDarkTheme(false),        //
              useDarkTrayIcon(true),      //
              maximumLogLines(500),       //
              maxJumpListCount(20),       //
              useOldShareLinkFormat(true) //
              {};
        JSONSTRUCT_REGISTER(Qv2rayConfig_UI, F(theme, language, quietMode, graphConfig, useDarkTheme, useDarkTrayIcon, maximumLogLines,
                                               maxJumpListCount, recentConnections, useOldShareLinkFormat))
    };

    struct Qv2rayConfig_Plugin
    {
        QMap<QString, bool> pluginStates;
        bool v2rayIntegration;
        int portAllocationStart;
        Qv2rayConfig_Plugin() : pluginStates(), v2rayIntegration(true), portAllocationStart(15000){};
        JSONSTRUCT_REGISTER(Qv2rayConfig_Plugin, F(pluginStates, v2rayIntegration, portAllocationStart))
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
            : enableAPI(true), statsPort(15490),        //
              v2CorePath_linux(), v2AssetsPath_linux(), //
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
        bool setSessionResumption;
        bool testLatencyPeriodcally;
        JSONSTRUCT_REGISTER(Qv2rayConfig_Advanced, F(setAllowInsecure, setSessionResumption, testLatencyPeriodcally))
    };

    enum Qv2rayLatencyTestingMethod
    {
        TCPING,
        ICMPING,
        REALPING
    };

    struct Qv2rayConfig_Network
    {
        Qv2rayLatencyTestingMethod latencyTestingMethod;
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
        JSONSTRUCT_REGISTER(Qv2rayConfig_Network, F(latencyTestingMethod, proxyType, type, address, port, userAgent))
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
        ConnectionGroupPair lastConnectedId;
        Qv2rayAutoConnectionBehavior autoStartBehavior;
        //
        Qv2rayConfig_UI uiConfig;
        Qv2rayConfig_Plugin pluginConfig;
        Qv2rayConfig_Kernel kernelConfig;
        Qv2rayConfig_Update updateConfig;
        Qv2rayConfig_Network networkConfig;
        QvConfig_Inbounds inboundConfig;
        QvConfig_Outbounds outboundConfig;
        Qv2rayConfig_Advanced advancedConfig;
        GroupRoutingConfig defaultRouteConfig;

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
              inboundConfig(),                       //
              outboundConfig(),                      //
              advancedConfig(),                      //
              defaultRouteConfig(){};

        JSONSTRUCT_REGISTER(Qv2rayConfigObject,                                                                          //
                            F(config_version, tProxySupport, autoStartId, lastConnectedId, autoStartBehavior, logLevel), //
                            F(uiConfig, advancedConfig, pluginConfig, updateConfig, kernelConfig, networkConfig),        //
                            F(inboundConfig, outboundConfig, defaultRouteConfig))
    };
} // namespace Qv2ray::base::config
