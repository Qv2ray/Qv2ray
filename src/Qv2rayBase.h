#ifndef QV2RAYBASE_H
#define QV2RAYBASE_H

#include <QtCore>
#include "QvTinyLog.h"
#include "QvCoreConfigObjects.h"

#define QV2RAY_VERSION_STRING "v" QV_MAJOR_VERSION ".0.0"

#define QV2RAY_CONFIG_VERSION 2
#define QV2RAY_CONFIG_DIR_PATH (Qv2ray::Utils::GetConfigDirPath() + "/")
#define QV2RAY_CONFIG_FILE_PATH (QV2RAY_CONFIG_DIR_PATH + "Qv2ray.conf")

#define QV2RAY_CONNECTION_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_CONFIG_DIR_PATH + "generated/config.gen.json")

#define QV2RAY_VCORE_LOG_DIRNAME "logs/"
#define QV2RAY_VCORE_ACCESS_LOG_FILENAME "access.log"
#define QV2RAY_VCORE_ERROR_LOG_FILENAME "error.log"

// These is for early-2.0 version, final 2.0 will move these content into global config.
#define QV2RAY_CONFIG_TYPE_FILE "File"
#define QV2RAY_CONFIG_TYPE_MANUAL "Manual"
#define QV2RAY_CONFIG_TYPE_CONNECTIONSTRING "ConnectionString"
#define QV2RAY_CONFIG_TYPE_SUBSCRIPTION "Subscription"
#define QV2RAY_CONFIG_TYPE_JSON_KEY "_qv2ray.configSource"



// GUI TOOLS
#define RED(obj) \
    auto _temp = ui->obj->palette(); \
    _temp.setColor(QPalette::Text, Qt::red); \
    ui->obj->setPalette(_temp);

#define BLACK(obj) \
    auto _temp = ui->obj->palette(); \
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
        enum QvConfigType { CONFIGTYPE_CONFIG, CONFIGTYPE_SUBSCRIPTION };
        struct Qv2rayBasicInboundsConfig {
            string listenip;
            // SOCKS
            int socks_port;
            bool socks_useAuth;
            AccountObject socksAccount;
            // HTTP
            int http_port;
            bool http_useAuth;
            AccountObject httpAccount;
            Qv2rayBasicInboundsConfig(): listenip(), socks_port(), socks_useAuth(), socksAccount(), http_port(), http_useAuth(), httpAccount() {}
            Qv2rayBasicInboundsConfig(string listen, int socksPort, int httpPort):  Qv2rayBasicInboundsConfig()
            {
                socks_port = socksPort;
                http_port = httpPort;
                listenip = listen;
            }
            XTOSTRUCT(O(listenip, socks_port, socks_useAuth, socksAccount, http_port, http_useAuth, httpAccount))
        };

        struct Qv2rayConfig {
            int config_version;
            bool runAsRoot;
            int logLevel;
            //
            string language;
            string v2CorePath;
            string v2AssetsPath;
            string autoStartConfig;
            //
            string ignoredVersion;
            //
            bool proxyDefault;
            bool proxyCN;
            bool withLocalDNS;
            list<string> dnsList;
            //
            Qv2rayBasicInboundsConfig inBoundSettings;
#ifdef newFeature
            map<string, QvConfigType> configs;
#else
            list<string> configs;
#endif
            map<string, string> subscribes;
            MuxObject mux;
            Qv2rayConfig(): config_version(QV2RAY_CONFIG_VERSION), runAsRoot(false), logLevel(), proxyDefault(), proxyCN(), withLocalDNS(), inBoundSettings(), configs(), subscribes(), mux() { }
            Qv2rayConfig(string lang, string exePath, string assetsPath, int log, Qv2rayBasicInboundsConfig _inBoundSettings): Qv2rayConfig()
            {
                // These settings below are defaults.
                ignoredVersion = "";
                autoStartConfig = "";
                language = lang;
                v2CorePath = exePath;
                v2AssetsPath = assetsPath;
                logLevel = log;
                inBoundSettings = _inBoundSettings;
                runAsRoot = false;
                mux.enabled = false;
                dnsList.push_back("8.8.8.8");
                dnsList.push_back("1.1.1.1");
                dnsList.push_back("4.4.4.4");
                proxyCN = false;
                proxyDefault = true;
                withLocalDNS = true;
            }
            XTOSTRUCT(O(config_version, runAsRoot, logLevel, language, autoStartConfig, ignoredVersion, v2CorePath, v2AssetsPath, proxyDefault, proxyCN, withLocalDNS, dnsList, inBoundSettings, mux, configs, subscribes))
        };

        QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root);
    }
}

using namespace std;
using namespace Qv2ray;
using namespace Qv2ray::V2ConfigModels;
using namespace Qv2ray::QvConfigModels;

#endif // QV2RAYBASE_H
