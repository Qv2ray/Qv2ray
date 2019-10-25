#ifndef QV2RAYBASE_H
#define QV2RAYBASE_H
#include <QtCore>
#include "QvTinyLog.h"
#include "QvCoreConfigObjects.h"
#include "QvNetSpeedPlugin.h"
#include "QObjectMessageProxy.h"

#define QV2RAY_CONFIG_VERSION 8

// Base folder suffix.
#ifdef QT_DEBUG
#define QV2RAY_CONFIG_DIR_SUFFIX "_debug/"
#else
#define QV2RAY_CONFIG_DIR_SUFFIX "/"
#endif

// Get Configured Config Dir Path
#define QV2RAY_CONFIG_DIR (Qv2ray::Utils::GetConfigDirPath())
#define QV2RAY_CONFIG_FILE (QV2RAY_CONFIG_DIR + "Qv2ray.conf")

#define QV2RAY_CONFIG_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_DIR (QV2RAY_CONFIG_DIR + "generated/")
#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_GENERATED_DIR + "config.gen.json")

#ifndef QV2RAY_DEFAULT_VCORE_PATH
#ifdef _WIN32
#define QV2RAY_DEFAULT_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray.exe")
#else
#define QV2RAY_DEFAULT_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray")
#endif
#endif

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

#define NEWLINE "\r\n"

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

namespace Qv2ray
{
    namespace QvConfigModels
    {
        enum QvConfigType {
            CONFIGTYPE_CONFIG,
            CONFIGTYPE_SUBSCRIPTION
        };
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

        struct Qv2rayConfig {
            int config_version;
            bool tProxySupport;
            int logLevel;
            //
            string language;
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

            Qv2rayConfig():
                config_version(QV2RAY_CONFIG_VERSION),
                tProxySupport(false),
                logLevel(),
                language(),
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
            Qv2rayConfig(string lang, string assetsPath, int log, Qv2rayCoreInboundsConfig _inBoundSettings): Qv2rayConfig()
            {
                // These settings below are defaults.
                ignoredVersion = "";
                autoStartConfig = "";
                language = lang;
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
                        language,
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
