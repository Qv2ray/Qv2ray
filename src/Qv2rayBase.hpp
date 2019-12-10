#ifndef QV2RAYBASE_H
#define QV2RAYBASE_H
#include <QtCore>
#include <QtGui>
#include <QMap>
#include <vector>
#include <algorithm>
#include "QvTinyLog.hpp"
#include "QvCoreConfigObjects.hpp"

#define QV2RAY_CONFIG_VERSION 5

// Linux DEs should handle the ui schemes themselves.
// --> Or.. should we change this into a modifyable setting?
#ifdef Q_OS_LINUX
#define QV2RAY_USE_BUILTIN_DARKTHEME false
#else
#define QV2RAY_USE_BUILTIN_DARKTHEME true
#endif

// Base folder suffix.
#ifdef QT_DEBUG
#define QV2RAY_CONFIG_DIR_SUFFIX "_debug/"
#else
#define QV2RAY_CONFIG_DIR_SUFFIX "/"
#endif

// Get Configured Config Dir Path
#define QV2RAY_CONFIG_DIR (Qv2ray::Utils::GetConfigDirPath())
#define QV2RAY_CONFIG_FILE (QV2RAY_CONFIG_DIR + "Qv2ray.conf")
#define QV2RAY_SUBSCRIPTION_DIR (QV2RAY_CONFIG_DIR + "subscriptions/")
#define QV2RAY_QRCODE_DIR (QV2RAY_CONFIG_DIR + "qr_images/")

// Get GFWList and PAC file path.
#define QV2RAY_RULES_DIR (QV2RAY_CONFIG_DIR + "rules/")
#define QV2RAY_RULES_GFWLIST_PATH (QV2RAY_RULES_DIR + "gfwList.txt")
#define QV2RAY_RULES_PAC_PATH (QV2RAY_RULES_DIR + "pac.txt")

#define QV2RAY_CONFIG_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_DIR (QV2RAY_CONFIG_DIR + "generated/")
#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_GENERATED_DIR + "config.gen.json")

#ifndef QV2RAY_DEFAULT_VCORE_PATH
#define QV2RAY_DEFAULT_VASSETS_PATH (QV2RAY_CONFIG_DIR + "vcore/")
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
#define QV2RAY_IS_DARKTHEME (GetGlobalConfig().uiConfig.useDarkTheme)
#define RED(obj)                               \
    auto _temp = obj->palette();               \
    _temp.setColor(QPalette::Text, Qt::red);   \
    obj->setPalette(_temp);

#define BLACK(obj)                             \
    obj->setPalette(this->palette());

#define QV2RAY_UI_RESOURCES_ROOT QSTRING(QV2RAY_IS_DARKTHEME ? ":/icons/ui_dark/" : ":/icons/ui_light/")
#define QICON_R(file) QIcon(QV2RAY_UI_RESOURCES_ROOT + file)

#define QSTRING(std_string) QString::fromStdString(std_string)
#define NEWLINE "\r\n"

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/*
 * Generic function to find if an element of any type exists in list
 */
template<typename T>
bool contains(std::list<T> &listOfElements, const T &element)
{
    // Find the iterator if element in list
    auto it = std::find(listOfElements.begin(), listOfElements.end(), element);
    //return if iterator points to end or not. It points to end then it means element
    // does not exists in list
    return it != listOfElements.end();
}

namespace Qv2ray
{
    //
    // Extra header for QvConfigUpgrade.cpp
    CONFIGROOT UpgradeConfig(int fromVersion, int toVersion, CONFIGROOT root);

    struct QvBarLine {
        string          Family;
        bool            Bold;
        bool            Italic;
        int             ColorA;
        int             ColorR;
        int             ColorG;
        int             ColorB;
        int             ContentType;
        double          Size;
        string          Message;
        QvBarLine()
            : Family("Consolas")
            , Bold(true)
            , Italic(false)
            , ColorA(255), ColorR(255), ColorG(255), ColorB(255)
            , ContentType(0)
            , Size(9),
              Message() { }
        XTOSTRUCT(O(Bold, Italic, ColorA, ColorR, ColorG, ColorB, Size, Family, Message, ContentType))
    };


    struct QvBarPage {
        int OffsetYpx;
        vector<QvBarLine> Lines;
        QvBarPage() : OffsetYpx(5) { }
        XTOSTRUCT(O(OffsetYpx, Lines))
    };

    struct Qv2rayToolBarConfig {
        vector<QvBarPage> Pages;
        XTOSTRUCT(O(Pages))
    };

    namespace QvConfigModels
    {
        struct Qv2rayPACConfig {
            bool enablePAC;
            int port;
            string localIP;
            bool useSocksProxy;
            Qv2rayPACConfig() : enablePAC(false), port(8989), useSocksProxy(false) { }
            XTOSTRUCT(O(enablePAC, port, localIP, useSocksProxy))
        };
        struct Qv2rayInboundsConfig {
            string listenip;
            bool setSystemProxy;
            Qv2rayPACConfig pacConfig;

            // SOCKS
            bool useSocks;
            int socks_port;
            bool socks_useAuth;
            bool socksUDP;
            string socksLocalIP;
            AccountObject socksAccount;
            // HTTP
            bool useHTTP;
            int http_port;
            bool http_useAuth;
            AccountObject httpAccount;
            Qv2rayInboundsConfig():
                listenip("127.0.0.1"), setSystemProxy(false), pacConfig(),
                useSocks(true), socks_port(1088), socks_useAuth(false), socksUDP(true), socksLocalIP("127.0.0.1"), socksAccount(),
                useHTTP(true), http_port(8888), http_useAuth(false), httpAccount() {}

            XTOSTRUCT(O(setSystemProxy, pacConfig, listenip, useSocks, useHTTP, socks_port, socks_useAuth, socksAccount, socksUDP, socksLocalIP, http_port, http_useAuth, httpAccount))
        };

        struct Qv2rayUIConfig {
            string theme;
            string language;
            bool useDarkTheme;
            bool useDarkTrayIcon;
            Qv2rayUIConfig() : theme("Fusion"), language("en-US"), useDarkTheme(false), useDarkTrayIcon(true) { }
            XTOSTRUCT(O(theme, language, useDarkTheme, useDarkTrayIcon))
        };

        struct Qv2rayConnectionConfig {
            bool bypassCN;
            bool enableProxy;
            bool withLocalDNS;
            list<string> dnsList;
            bool enableStats;
            int statsPort;
            Qv2rayConnectionConfig() : bypassCN(true), enableProxy(true), withLocalDNS(true), dnsList(), enableStats(true), statsPort(15490) { }
            XTOSTRUCT(O(bypassCN, enableProxy, withLocalDNS, dnsList, enableStats, statsPort))
        };

        struct Qv2rayConfig {
            int config_version;
            bool tProxySupport;
            int logLevel;
            //
            string v2CorePath;
            string v2AssetsPath;
            ConfigIdentifier autoStartConfig;
            string ignoredVersion;
            //
            list<string> configs;
            map<string, string> subscribes;
            //
            Qv2rayUIConfig uiConfig;
            Qv2rayInboundsConfig inboundConfig;
            Qv2rayConnectionConfig connectionConfig;
            Qv2rayToolBarConfig toolBarConfig;

            Qv2rayConfig():
                config_version(QV2RAY_CONFIG_VERSION),
                tProxySupport(false),
                logLevel(),
                v2CorePath(),
                v2AssetsPath(),
                autoStartConfig(),
                ignoredVersion(),
                configs(),
                subscribes(),
                uiConfig(),
                inboundConfig(),
                connectionConfig(),
                toolBarConfig() { }

            XTOSTRUCT(O(config_version,
                        ignoredVersion,
                        tProxySupport,
                        logLevel,
                        autoStartConfig,
                        v2CorePath, v2AssetsPath,
                        configs,
                        uiConfig,
                        subscribes, inboundConfig, connectionConfig, toolBarConfig))
        };

    }
}

using namespace std;
using namespace Qv2ray;
using namespace Qv2ray::V2ConfigModels;
using namespace Qv2ray::QvConfigModels;

#endif // QV2RAYBASE_H
