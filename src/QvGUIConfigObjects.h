#ifndef HCONFIGOBJECTS_H
#define HCONFIGOBJECTS_H

#include <QtCore>
#include <iostream>
#include "QvCoreConfigObjects.h"

using namespace std;
#define LOG(arg) cout << arg << endl;

#define QV2RAY_VERSION 1
#define QV2RAY_VERSION_STRING "v1.2.0"
#define QV2RAY_CONFIG_PATH (Qv2ray::Utils::GetConfigDirPath() + "/")
#define QV2RAY_GUI_CONFIG_PATH (QV2RAY_CONFIG_PATH + "Qv2ray.conf")
#define QV2RAY_GENERATED_CONFIG_FILE_PATH (QV2RAY_CONFIG_PATH + "generated/config.gen.json")

#define QV2RAY_VCORE_LOG_DIRNAME "logs/"
#define QV2RAY_VCORE_ACCESS_LOG_FILENAME "access.log"
#define QV2RAY_VCORE_ERROR_LOG_FILENAME "error.log"

#define QV2RAY_CONNECTION_FILE_EXTENSION ".qv2ray.json"

// GUI TOOLS
#define RED(obj)   \
    auto p = ui->obj->palette(); \
    p.setColor(QPalette::Text, Qt::red); \
    ui->obj->setPalette(p);

#define BLACK(obj)   \
    auto p = ui->obj->palette(); \
    p.setColor(QPalette::Text, Qt::black); \
    ui->obj->setPalette(p);

#ifdef __WIN32
#define NEWLINE "\r\n"
#else
#define NEWLINE "\r"
#endif


namespace Qv2ray
{
    namespace QvConfigModels
    {
        struct QvBasicInboundSetting {
            string listenip;
            // SOCKS
            int socks_port;
            bool socks_useAuth;
            AccountObject socksAccount;
            // HTTP
            int http_port;
            bool http_useAuth;
            AccountObject httpAccount;
            QvBasicInboundSetting():
                listenip(),
                socks_port(), socks_useAuth(), socksAccount(),
                http_port(), http_useAuth(), httpAccount() {}
            QvBasicInboundSetting(string listen, int socksPort, int httpPort):  QvBasicInboundSetting()
            {
                socks_port = socksPort;
                http_port = httpPort;
                listenip = listen;
            }
            XTOSTRUCT(O(listenip, socks_port, socks_useAuth, socksAccount, http_port, http_useAuth, httpAccount))
        };

        struct Qv2Config {
            string v = "1.1";
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
            QvBasicInboundSetting inBoundSettings;
            list<string> configs;
            map<string, string> subscribes;
            MuxObject mux;
            Qv2Config(): runAsRoot(false), logLevel(), proxyDefault(), proxyCN(), withLocalDNS(), inBoundSettings(), configs(), subscribes(), mux() { }
            Qv2Config(string lang, string exePath, string assetsPath, int log, QvBasicInboundSetting _inBoundSettings): Qv2Config()
            {
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
            XTOSTRUCT(O(v, runAsRoot, logLevel, language, autoStartConfig, ignoredVersion, v2CorePath, v2AssetsPath, proxyDefault, proxyCN, withLocalDNS, dnsList, inBoundSettings, mux, configs, subscribes))
        };
    }
}

// I want to use all namespaces

using namespace std;
using namespace Qv2ray;
using namespace Qv2ray::V2ConfigModels;
using namespace Qv2ray::QvConfigModels;

#endif // QCONFIGOBJECTS_H
