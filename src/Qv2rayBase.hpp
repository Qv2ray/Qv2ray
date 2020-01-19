#ifndef QV2RAYBASE_H
#define QV2RAYBASE_H
#include <QtCore>
#include <QtGui>
#include <QMap>
#include <vector>
#include <algorithm>
#include <ctime>
#include "QvTinyLog.hpp"
#include "QvCoreConfigObjects.hpp"

const int QV2RAY_CONFIG_VERSION = 6;

// Linux users and DEs should handle the darkMode UI themselves.
#ifndef QV2RAY_USE_BUILTIN_DARKTHEME
# ifndef Q_OS_LINUX
#  define QV2RAY_USE_BUILTIN_DARKTHEME
# endif
#endif

extern bool isDebug;
// Base folder suffix.
#ifdef QT_DEBUG
# define QV2RAY_CONFIG_DIR_SUFFIX "_debug/"
#else
# define QV2RAY_CONFIG_DIR_SUFFIX "/"
#endif

// Get Configured Config Dir Path
#define QV2RAY_CONFIG_DIR (Qv2ray::Utils::GetConfigDirPath())
#define QV2RAY_CONFIG_FILE (QV2RAY_CONFIG_DIR + "Qv2ray.conf")
#define QV2RAY_SUBSCRIPTION_DIR (QV2RAY_CONFIG_DIR + "subscriptions/")

// Get GFWList and PAC file path.
#define QV2RAY_RULES_DIR (QV2RAY_CONFIG_DIR + "rules/")
#define QV2RAY_RULES_GFWLIST_PATH (QV2RAY_RULES_DIR + "gfwList.txt")
#define QV2RAY_RULES_PAC_PATH (QV2RAY_RULES_DIR + "pac.txt")

#define QV2RAY_CONFIG_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_DIR (QV2RAY_CONFIG_DIR + "generated/")
#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_GENERATED_DIR + "config.gen.json")

#if ! defined (QV2RAY_DEFAULT_VCORE_PATH) && ! defined (QV2RAY_DEFAULT_VASSETS_PATH)
#   define QV2RAY_DEFAULT_VASSETS_PATH (QV2RAY_CONFIG_DIR + "vcore/")
#   ifdef Q_OS_WIN
#       define QV2RAY_DEFAULT_VCORE_PATH  (QV2RAY_CONFIG_DIR + "vcore/v2ray.exe")
#   else
#       define QV2RAY_DEFAULT_VCORE_PATH  (QV2RAY_CONFIG_DIR + "vcore/v2ray")
#   endif
#elif defined (QV2RAY_DEFAULT_VCORE_PATH) && defined (QV2RAY_DEFAULT_VASSETS_PATH)
// ---- Using user-specified VCore and VAssets path
#else
#   error Both QV2RAY_DEFAULT_VCORE_PATH and QV2RAY_DEFAULT_VASSETS_PATH need to present when specifying the paths.
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

#define QV2RAY_UI_RESOURCES_ROOT (QV2RAY_IS_DARKTHEME ? QStringLiteral(":/assets/icons/ui_dark/") : QStringLiteral(":/assets/icons/ui_light/"))
#define QICON_R(file) QIcon(QV2RAY_UI_RESOURCES_ROOT + file)

#define QSTRN(num) QString::number(num)

#define NEWLINE "\r\n"

using namespace std;
using namespace std::chrono;

namespace Qv2ray
{
    //
    // Extra header for QvConfigUpgrade.cpp
    QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root);

    struct QvBarLine {
        QString         Family;
        bool            Bold;
        bool            Italic;
        int             ColorA;
        int             ColorR;
        int             ColorG;
        int             ColorB;
        int             ContentType;
        double          Size;
        QString         Message;
        QvBarLine()
            : Family("Consolas")
            , Bold(true)
            , Italic(false)
            , ColorA(255), ColorR(255), ColorG(255), ColorB(255)
            , ContentType(0)
            , Size(9),
              Message("") { }
        XTOSTRUCT(O(Bold, Italic, ColorA, ColorR, ColorG, ColorB, Size, Family, Message, ContentType))
    };

    struct QvBarPage {
        int OffsetYpx;
        QList<QvBarLine> Lines;
        QvBarPage() : OffsetYpx(5) { }
        XTOSTRUCT(O(OffsetYpx, Lines))
    };

    struct Qv2rayToolBarConfig {
        QList<QvBarPage> Pages;
        XTOSTRUCT(O(Pages))
    };

    namespace QvConfigModels
    {
        struct Qv2raySubscriptionConfig {
            time_t lastUpdated;
            float updateInterval;
            QString address;
            Qv2raySubscriptionConfig() : lastUpdated(system_clock::to_time_t(system_clock::now())), updateInterval(5), address("") { }
            XTOSTRUCT(O(lastUpdated, updateInterval, address))
        };

        struct Qv2rayPACConfig {
            bool enablePAC;
            int port;
            QString localIP;
            bool useSocksProxy;
            Qv2rayPACConfig() : enablePAC(false), port(8989), useSocksProxy(false) { }
            XTOSTRUCT(O(enablePAC, port, localIP, useSocksProxy))
        };

        struct Qv2rayForwardProxyConfig {
            bool enableForwardProxy;
            QString type;
            QString serverAddress;
            int port;
            bool useAuth;
            QString username;
            QString password;

            XTOSTRUCT(O(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
        };

        struct Qv2rayInboundsConfig {
            QString listenip;
            bool setSystemProxy;
            Qv2rayPACConfig pacConfig;

            // SOCKS
            bool useSocks;
            int socks_port;
            bool socks_useAuth;
            bool socksUDP;
            QString socksLocalIP;
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
            QString theme;
            QString language;
            bool useDarkTheme;
            bool useDarkTrayIcon;
            int maximumLogLines;
            Qv2rayUIConfig() : theme("Fusion"), language("en-US"), useDarkTheme(false), useDarkTrayIcon(true), maximumLogLines(500) { }
            XTOSTRUCT(O(theme, language, useDarkTheme, useDarkTrayIcon, maximumLogLines))
        };

        struct Qv2rayConnectionConfig {
            bool bypassCN;
            bool enableProxy;
            bool withLocalDNS;
            QList<QString> dnsList;
            int statsPort;
            Qv2rayForwardProxyConfig forwardProxyConfig;

            Qv2rayConnectionConfig() : bypassCN(true), enableProxy(true), withLocalDNS(false), dnsList(QStringList() << "8.8.4.4" << "1.1.1.1"), statsPort(15490) { }
            XTOSTRUCT(O(bypassCN, enableProxy, withLocalDNS, dnsList, statsPort, forwardProxyConfig))
        };

        struct Qv2rayConfig {
            int config_version;
            bool tProxySupport;
            int logLevel;
            //
            QString v2CorePath;
            QString v2AssetsPath;
            QvConfigIdentifier autoStartConfig;
            QString ignoredVersion;
            //
            QList<QString> configs;
            QMap<QString, Qv2raySubscriptionConfig> subscriptions;
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
                subscriptions(),
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
                        subscriptions, inboundConfig, connectionConfig, toolBarConfig))
        };

    }
}

using namespace std;
using namespace Qv2ray;
using namespace Qv2ray::V2ConfigModels;
using namespace Qv2ray::QvConfigModels;

#endif // QV2RAYBASE_H
