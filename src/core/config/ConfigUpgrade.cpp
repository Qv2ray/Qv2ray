//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "QvUtils.hpp"

#define UPDATELOG(msg) LOG(MODULE_CONFIG, "  [" + QSTRN(fromVersion) + "-" + QSTRN(fromVersion + 1) + "] --> " + msg)

namespace Qv2ray
{
    // Private member
    QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root)
    {
        switch (fromVersion) {
            case 1: {
                auto v1_oldConfigVersion = root["config_version"].toString();
                // From 1 to 2, we changed the config_version from 'string' to 'int'
                root.remove("config_version");
                root["config_version"] = 2;
                UPDATELOG("Upgrading config_version from old value " + v1_oldConfigVersion + " to 2")
                break;
            }

            case 2: {
                // We copied those files.
                auto vCoreFilePath = root["v2CorePath"].toString();
                auto vCoreDestPath = QString(QV2RAY_DEFAULT_VCORE_PATH);
                // We also need v2ctl
                auto v2CtlFilePath = QFileInfo(vCoreFilePath).dir().path() + "/v2ctl";
                auto v2CtlDestPath = QFileInfo(vCoreDestPath).dir().path() + "/v2ctl";
#ifdef Q_OS_WIN
                v2CtlFilePath = v2CtlFilePath.append(".exe");
                v2CtlDestPath = v2CtlDestPath.append(".exe");
#endif
                QFile::copy(vCoreFilePath, vCoreDestPath);
                QFile::copy(v2CtlFilePath, v2CtlDestPath);
                root.remove("v2CorePath");
                UPDATELOG("v2CorePath value from: " + vCoreFilePath + " to " +  vCoreDestPath)
                UPDATELOG("v2CtlFilePath value from: " + v2CtlFilePath + " to " + v2CtlDestPath)
                break;
            }

            case 3: {
                // We changed a key name in the config file.
                //proxyDefault
                auto oldProxyDefault = root["proxyDefault"].toBool();
                root.remove("proxyDefault");
                root["enableProxy"] = oldProxyDefault;
                //enableProxy
                UPDATELOG("key: proxyDefault->enableProxy, value from: " + QSTRN(oldProxyDefault) + " to " + QSTRN(oldProxyDefault))
                break;
            }

            // --------------------------------------------------------------------------------------
            // Below is for Qv2ray version 2
            case 4: {
                // We changed the "proxyCN" to "bypassCN" as it's easier to understand....
                auto v2_oldProxyCN = root["proxyCN"].toBool();
                //
                // From 3 to 4, we changed 'runAsRoot' to 'tProxySupport'
                auto v3_oldrunAsRoot = root["runAsRoot"].toBool();
                root.insert("tProxySupport", v3_oldrunAsRoot);
                UPDATELOG("Upgrading runAsRoot to tProxySupport, the value is not changed: " + QSTRN(v3_oldrunAsRoot))
                //
                QString path;
                path = QV2RAY_DEFAULT_VCORE_PATH;
                root["v2CorePath"] = path;
                UPDATELOG("Added v2CorePath to the config file.")
                //
                QJsonObject uiSettings;
                uiSettings["language"] = root["language"].toString("en-US").replace("-", "_");
                root["uiConfig"] = uiSettings;
                //
                root["inboundConfig"] = root["inBoundSettings"];
                root.remove("inBoundSettings");
                UPDATELOG("Renamed inBoundSettings to inboundConfig.")
                //
                //connectionConfig
                QJsonObject o;
                o["dnsList"] = root["dnsList"];
                o["withLocalDNS"] = root["withLocalDNS"];
                o["enableProxy"] = root["enableProxy"];
                o["bypassCN"] = !v2_oldProxyCN;
                o["enableStats"] = true;
                o["statsPort"] = 13459;
                UPDATELOG("Default statistics enabled.")
                root["connectionConfig"] = o;
                UPDATELOG("Renamed some connection configs to connectionConfig.")
                //
                // Do we need renaming here?
                // //auto inbound = root["inboundConfig"].toObject();
                // //auto pacConfig = inbound["pacConfig"].toObject();
                // //pacConfig["enablePAC"] = pacConfig["usePAC"].toBool();
                // //inbound["pacConfig"] = pacConfig;
                // //root["inboundConfig"] = inbound;
                // //UPDATELOG("Renamed usePAC to enablePAC.")
                //
                QvConfigIdentifier i;
                i.connectionName = root["autoStartConfig"].toString();
                root["autoStartConfig"] = GetRootObject(i);
                UPDATELOG("Added subscription feature to autoStartConfig.")
                break;
            }

            // Qv2ray version 2, RC 2
            case 5: {
                // Added subscription auto update
                auto subs = root["subscribes"].toObject();
                root.remove("subscribes");
                QJsonObject newSubscriptions;

                for (auto item = subs.begin(); item != subs.end(); item++) {
                    auto key = item.key();
                    Qv2raySubscriptionConfig _conf;
                    _conf.address = item.value().toString();
                    _conf.lastUpdated = system_clock::to_time_t(system_clock::now());
                    _conf.updateInterval = 5;
                    auto value = GetRootObject(_conf);
                    newSubscriptions[key] = value;
                }

                root["subscriptions"] = newSubscriptions;
                UPDATELOG("Added subscription renewal options.")
                break;
            }

            // Qv2ray version 2, RC 4
            case 6: {
                // Moved API Stats port from connectionConfig to apiConfig
                QJsonObject apiConfig;
                apiConfig["enableAPI"] = true;
                apiConfig["statsPort"] = root["connectionConfig"].toObject()["statsPort"].toInt();
                root["apiConfig"] = apiConfig;
                break;
            }

            case 7: {
                auto lang = root["uiConfig"].toObject()["language"].toString().replace("-", "_");
                auto uiConfig = root["uiConfig"].toObject();
                uiConfig["language"] = lang;
                root["uiConfig"] = uiConfig;
                UPDATELOG("Changed language: " + lang)
                break;
            }
        }

        root["config_version"] = root["config_version"].toInt() + 1;
        return root;
    }

    // Exported function
    QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root)
    {
        LOG(MODULE_CONFIG, "Migrating config from version " + QSTRN(fromVersion) + " to " + QSTRN(toVersion))

        for (int i = fromVersion; i < toVersion; i++) {
            root = UpgradeConfig_Inc(i, root);
        }

        return root;
    }
}
