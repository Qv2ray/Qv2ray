#ifndef QV2RAYBASE_H
#define QV2RAYBASE_H

#include <QtCore>
#include "QvTinyLog.h"
#include "QvCoreConfigObjects.h"
#include "QvNetSpeedPlugin.h"
#include "QObjectMessageProxy.h"

#define QV2RAY_VERSION_STRING "v" QV_MAJOR_VERSION

#define QV2RAY_CONFIG_VERSION 7
// Base folder.
#define QV2RAY_CONFIG_DIR_PATH (Qv2ray::Utils::GetConfigDirPath() + "/")
#define QV2RAY_CONFIG_FILE_PATH (QV2RAY_CONFIG_DIR_PATH + "Qv2ray.conf")

// We need v2ray.exe/v2ray executables here!
#define QV2RAY_V2RAY_CORE_DIR_PATH (QV2RAY_CONFIG_DIR_PATH + "vcore/")

#ifdef __WIN32
#define QV2RAY_V2RAY_CORE_PATH (QV2RAY_V2RAY_CORE_DIR_PATH + "v2ray.exe")
#else
#define QV2RAY_V2RAY_CORE_PATH (QV2RAY_V2RAY_CORE_DIR_PATH + "v2ray")
#endif

#define QV2RAY_CONNECTION_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_CONFIG_DIR_PATH + "generated/config.gen.json")

#define QV2RAY_VCORE_LOG_DIRNAME "logs/"
#define QV2RAY_VCORE_ACCESS_LOG_FILENAME "access.log"
#define QV2RAY_VCORE_ERROR_LOG_FILENAME "error.log"

// GUI TOOLS
#define RED(obj)                               \
    auto _temp = ui->obj->palette();           \
    _temp.setColor(QPalette::Text, Qt::red);   \
    ui->obj->setPalette(_temp);

#define BLACK(obj)                             \
    auto _temp = ui->obj->palette();           \
    _temp.setColor(QPalette::Text, Qt::black); \
    ui->obj->setPalette(_temp);

#define QSTRING(std_string) QString::fromStdString(std_string)

#ifdef __WIN32
#define NEWLINE "\r\n"
#else
#define NEWLINE "\r"
#endif

namespace Qv2ray
{
    namespace QvConfigModels
    {
        enum QvConfigType {
            CONFIGTYPE_CONFIG,
            CONFIGTYPE_SUBSCRIPTION
        };
        struct Qv2rayBasicInboundsConfig {
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
            Qv2rayBasicInboundsConfig(): listenip(), socks_port(), socks_useAuth(), socksAccount(), http_port(), http_useAuth(), httpAccount() {}
            Qv2rayBasicInboundsConfig(string listen, int socksPort, int httpPort): Qv2rayBasicInboundsConfig()
            {
                socks_port = socksPort;
                http_port = httpPort;
                listenip = listen;
                socksLocalIP = "0.0.0.0";
                socksUDP = true;
            }
            XTOSTRUCT(O(listenip, socks_port, socks_useAuth, socksAccount, socksUDP, socksLocalIP, http_port, http_useAuth, httpAccount))
        };

        struct Qv2rayConfig {
            int config_version;
            bool tProxySupport;
            int logLevel;
            //
            string language;
            string v2AssetsPath;
            string autoStartConfig;
            //
            string ignoredVersion;
            //
            bool bypassCN;
            bool enableProxy;
            bool withLocalDNS;
            MuxObject mux;
            //
            bool enableStats;
            int statsPort;
            //
            list<string> dnsList;
            //
            Qv2rayBasicInboundsConfig inBoundSettings;
#ifdef newFeature
            map<string, QvConfigType> configs;
#else
            list<string> configs;
#endif
            map<string, string> subscribes;

            QvNetSpeedBarConfig speedBarConfig;

            Qv2rayConfig():
                config_version(QV2RAY_CONFIG_VERSION),
                tProxySupport(false),
                logLevel(),
                language(),
                v2AssetsPath(),
                autoStartConfig(),
                ignoredVersion(),
                bypassCN(),
                enableProxy(),
                withLocalDNS(),
                mux(),
                enableStats(),
                statsPort(15934),
                dnsList(),
                inBoundSettings(),
                configs(),
                subscribes(),
                speedBarConfig() { }
            Qv2rayConfig(string lang, string assetsPath, int log, Qv2rayBasicInboundsConfig _inBoundSettings): Qv2rayConfig()
            {
                // These settings below are defaults.
                ignoredVersion = "";
                autoStartConfig = "";
                language = lang;
                v2AssetsPath = assetsPath;
                inBoundSettings = _inBoundSettings;
                logLevel = log;
                tProxySupport = false;
                mux.enabled = false;
                dnsList.push_back("8.8.8.8");
                dnsList.push_back("1.1.1.1");
                dnsList.push_back("4.4.4.4");
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
                        language,
                        autoStartConfig,
                        ignoredVersion,
                        v2AssetsPath,
                        enableProxy,
                        bypassCN,
                        withLocalDNS,
                        dnsList,
                        inBoundSettings,
                        mux,
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
