﻿#ifndef QV2RAYBASE_H
#define QV2RAYBASE_H
#include <QtCore>
#include <QMap>
#include "QvTinyLog.hpp"
#include "QvCoreConfigObjects.hpp"
#include "QvNetSpeedPlugin.hpp"
#include "QObjectMessageProxy.hpp"

#define QV2RAY_CONFIG_VERSION 9
#define QV2RAY_USE_BUILTIN_DARKTHEME true

// Base folder suffix.
#ifdef QT_DEBUG
#define QV2RAY_CONFIG_DIR_SUFFIX "_debug/"
#else
#define QV2RAY_CONFIG_DIR_SUFFIX "/"
#endif

// Get Configured Config Dir Path
#define QV2RAY_CONFIG_DIR (Qv2ray::Utils::GetConfigDirPath())
#define QV2RAY_CONFIG_FILE (QV2RAY_CONFIG_DIR + "Qv2ray.conf")
#define QV2RAY_QRCODE_DIR (QV2RAY_CONFIG_DIR + "qr_images/")

#define QV2RAY_CONFIG_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_DIR (QV2RAY_CONFIG_DIR + "generated/")
#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_GENERATED_DIR + "config.gen.json")

#ifndef QV2RAY_DEFAULT_VCORE_PATH
#ifdef Q_OS_WIN
#define QV2RAY_DEFAULT_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray.exe")
#else
#define QV2RAY_DEFAULT_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray")
#endif
#endif

#define QV2RAY_VCORE_LOG_DIRNAME "logs/"
#define QV2RAY_VCORE_ACCESS_LOG_FILENAME "access.log"
#define QV2RAY_VCORE_ERROR_LOG_FILENAME "error.log"

// GUI TOOLS
#define QV2RAY_IS_DARKTHEME GetGlobalConfig().UISettings.useDarkTheme
#define RED(obj)                               \
    auto _temp = obj->palette();               \
    _temp.setColor(QPalette::Text, Qt::red);   \
    obj->setPalette(_temp);

// TODO: Dark mode support.
#define BLACK(obj)                             \
    auto _temp = obj->palette();               \
    _temp.setColor(QPalette::Text, QV2RAY_IS_DARKTHEME ? Qt::white : Qt::black);  \
    obj->setPalette(_temp);

#define UI_COMPONENTS_RESOURCES_ROOT QSTRING(QV2RAY_IS_DARKTHEME ? ":/icons/ui_components/dark/" : ":/icons/ui_components/")

#define QSTRING(std_string) QString::fromStdString(std_string)
#define NEWLINE "\r\n"

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

namespace Qv2ray
{
    namespace QvConfigModels
    {
        struct Qv2rayCoreInboundsConfig {
            string listenip;
            // SOCKS
            int socks_port;
            bool socks_useAuth;
            bool socksUDP;
            string socksLocalIP;
            AccountObject socksAccount;
            // HTTP
            int http_port;
            bool http_useAuth;
            AccountObject httpAccount;
            Qv2rayCoreInboundsConfig(): listenip(), socks_port(), socks_useAuth(), socksAccount(), http_port(), http_useAuth(), httpAccount() {}
            Qv2rayCoreInboundsConfig(string listen, int socksPort, int httpPort): Qv2rayCoreInboundsConfig()
            {
                socks_port = socksPort;
                http_port = httpPort;
                listenip = listen;
                socksLocalIP = "0.0.0.0";
                socksUDP = true;
            }
            XTOSTRUCT(O(listenip, socks_port, socks_useAuth, socksAccount, socksUDP, socksLocalIP, http_port, http_useAuth, httpAccount))
        };

        struct UIConfig {
            string theme;
            string language;
            bool useDarkTheme;
            XTOSTRUCT(O(theme, language, useDarkTheme))
        };

        struct Qv2rayConfig {
            int config_version;
            bool tProxySupport;
            int logLevel;
            //
            UIConfig UISettings;
            //
            string v2CorePath;
            string v2AssetsPath;
            string autoStartConfig;
            //
            string ignoredVersion;
            //
            bool bypassCN;
            bool enableProxy;
            bool withLocalDNS;
            //
            bool enableStats;
            int statsPort;
            //
            list<string> dnsList;
            //
            Qv2rayCoreInboundsConfig inBoundSettings;
#ifdef newFeature
            map<string, QvConfigType> configs;
#else
            list<string> configs;
#endif
            map<string, string> subscribes;
            QvNetSpeedBarConfig speedBarConfig;
            // TODO Change Structure. of SpeedBarConfig

            Qv2rayConfig():
                config_version(QV2RAY_CONFIG_VERSION),
                tProxySupport(false),
                logLevel(),
                UISettings(),
                v2CorePath(),
                v2AssetsPath(),
                autoStartConfig(),
                ignoredVersion(),
                bypassCN(),
                enableProxy(),
                withLocalDNS(),
                enableStats(),
                statsPort(15934),
                dnsList(),
                inBoundSettings(),
                configs(),
                subscribes(),
                speedBarConfig() { }
            Qv2rayConfig(const string &assetsPath, int log, const Qv2rayCoreInboundsConfig &_inBoundSettings): Qv2rayConfig()
            {
                // These settings below are defaults.
                ignoredVersion = "";
                autoStartConfig = "";
                v2AssetsPath = assetsPath;
                inBoundSettings = _inBoundSettings;
                logLevel = log;
                tProxySupport = false;
                dnsList.push_back("8.8.8.8");
                dnsList.push_back("8.8.4.4");
                dnsList.push_back("1.1.1.1");
                bypassCN = true;
                enableProxy = true;
                withLocalDNS = true;
                enableStats = true;
                statsPort = 15934;
            }
            XTOSTRUCT(O(config_version,
                        enableStats,
                        statsPort,
                        tProxySupport,
                        logLevel,
                        UISettings,
                        autoStartConfig,
                        ignoredVersion,
                        v2CorePath,
                        v2AssetsPath,
                        enableProxy,
                        bypassCN,
                        withLocalDNS,
                        dnsList,
                        inBoundSettings,
                        configs,
                        subscribes,
                        speedBarConfig))
        };

        // Extra header for QvConfigUpgrade.cpp
        QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root);
    }
}

using namespace std;
using namespace Qv2ray;
using namespace Qv2ray::V2ConfigModels;
using namespace Qv2ray::QvConfigModels;

#endif // QV2RAYBASE_H
