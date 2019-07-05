#ifndef HCONFIGOBJECTS_HPP
#define HCONFIGOBJECTS_HPP
#include <QDir>
#include <map>

#include "V2ConfigObjects.hpp"
#include "vinteract.hpp"

// Macros
#define QV2RAY_CONFIG_DIR_NAME "/.qv2ray/"
using namespace std;
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
            QvInbondSetting httpSetting;
            QvInbondSetting socksSetting;
            list<QvConfigList> configs;
            Qv2Config(): language(), runAsRoot(), logLevel(), httpSetting(), socksSetting(), configs() { }
            Qv2Config(string lang, string log, QvInbondSetting httpIn, QvInbondSetting socksIN): Qv2Config()
            {
                language = lang;
                logLevel = log;
                httpSetting = httpIn;
                socksSetting = socksIN;
                runAsRoot = false;
            }
            XTOSTRUCT(O(language, runAsRoot, logLevel, httpSetting, socksSetting, configs))
        };
    }

    /// ConfigGlobalConfig is platform-independent as it's solved to be in the best
    /// place at first in main.cpp
    static QDir ConfigDir;
}
#endif // HCONFIGOBJECTS_HPP
