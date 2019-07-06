#ifndef HCONFIGOBJECTS_HPP
#define HCONFIGOBJECTS_HPP
#include <QDir>
#include <map>

#include "V2ConfigObjects.hpp"

// Macros
#define QV2RAY 1
#define QV2RAY_CONFIG_PATH (ConfigDir.path() + "/")
#define QV2RAY_MAIN_CONFIG_FILE_PATH (QV2RAY_CONFIG_PATH + "Qv2ray.conf")
#define QV2RAY_GENERATED_CONFIG_DIRPATH (QV2RAY_CONFIG_PATH + "genrerated/")
#define QV2RAY_FIRSTRUN_IDENTIFIER (QV2RAY_CONFIG_PATH + ".initialised")

#define LOG(arg) cout << arg << endl;

namespace Qv2ray
{
    namespace QvConfigModels
    {
        struct QvInbondSetting {
            bool enabled;
            string ip;
            int port;
            bool useAuthentication;
            string authUsername;
            string authPassword;
            QvInbondSetting(): enabled(), ip(), port(), useAuthentication(), authUsername(), authPassword() {}
            QvInbondSetting(bool _e, string _ip, int _p):  QvInbondSetting()
            {
                enabled = _e;
                port = _p;
                ip = _ip;
            }
            XTOSTRUCT(O(enabled, ip, port, useAuthentication, authUsername, authPassword))
        };

        struct QvConfigList {
            string alias;
            string fileName;
            int index;
            QvConfigList(): alias(), fileName(), index() {}
            XTOSTRUCT(O(alias, fileName, index))
        };

        struct Qv2Config {
            string language;
            bool runAsRoot;
            string logLevel;
            string v2Path;
            QvInbondSetting httpSetting;
            QvInbondSetting socksSetting;
            list<QvConfigList> configs;
            Qv2Config(): language(), runAsRoot(), logLevel(), httpSetting(), socksSetting(), configs() { }
            Qv2Config(string lang, string exePath, string log, QvInbondSetting httpIn, QvInbondSetting socksIN): Qv2Config()
            {
                v2Path = exePath;
                language = lang;
                logLevel = log;
                httpSetting = httpIn;
                socksSetting = socksIN;
                runAsRoot = false;
            }
            XTOSTRUCT(O(language, v2Path, runAsRoot, logLevel, httpSetting, socksSetting, configs))
        };
    }

    /// ConfigGlobalConfig is platform-independent as it's solved to be in the best
    /// place at first in main.cpp
    static QDir ConfigDir;
}

// I want to use all namespaces

using namespace std;
using namespace Qv2ray;
using namespace Qv2ray::V2ConfigModels;
using namespace Qv2ray::QvConfigModels;

#endif // HCONFIGOBJECTS_HPP
