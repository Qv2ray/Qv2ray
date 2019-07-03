#ifndef HCONFIGOBJECTS_HPP
#define HCONFIGOBJECTS_HPP
#include <QDir>
#include <map>

#include "V2ConfigObjects.hpp"
#include "vinteract.hpp"

// Macros
#define HV2RAY_CONFIG_DIR_NAME "/.hv2ray/"
using namespace std;
namespace Hv2ray
{
    namespace HConfigModels
    {
        struct HInbondSetting {
            bool enabled;
            string ip;
            int port;
            bool useAuthentication;
            string authUsername;
            string authPassword;
            HInbondSetting(): enabled(), ip(), port(), useAuthentication(), authUsername(), authPassword() {}
            HInbondSetting(bool _e, string _ip, int _p):  HInbondSetting()
            {
                enabled = _e;
                port = _p;
                ip = _ip;
            }
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(enabled, ip, port, useAuthentication, authUsername, authPassword))
#endif
        };

        struct HvConfigList {
            string alias;
            string fileName;
            int index;
            HvConfigList(): alias(), fileName(), index() {}
            XTOSTRUCT(O(alias, fileName, index))
        };

        struct Hv2Config {
            string language;
            bool runAsRoot;
            string logLevel;
            //Hv2ray::V2ConfigModels::MuxObject muxSetting;
            HInbondSetting httpSetting;
            HInbondSetting socksSetting;
            list<HvConfigList> configs;
            Hv2Config(): language(), runAsRoot(), logLevel(), httpSetting(), socksSetting(), configs() { }
            Hv2Config(string lang, string log, HInbondSetting httpIn, HInbondSetting socksIN): Hv2Config()
            {
                language = lang;
                logLevel = log;
                httpSetting = httpIn;
                socksSetting = socksIN;
                runAsRoot = false;
            }
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(language, runAsRoot, logLevel, httpSetting, socksSetting, configs))
#endif
        };
    }

    /// ConfigGlobalConfig is platform-independent as it's solved to be in the best
    /// place at first in main.cpp
    static QDir ConfigDir;
}


#if USE_TODO_FEATURES
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::HConfigModels::Hv2Config, language, runAsRoot, logLevel, httpSetting, socksSetting)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::HConfigModels::HInbondSetting, enabled, ip, port, useAuthentication, authUsername, authPassword)
#endif
#endif // HCONFIGOBJECTS_HPP
