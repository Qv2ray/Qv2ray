//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "QvUtils.h"

#define UPGRADELOG(item, old, _new) LOG(MODULE_CONFIG, "Upgrading " item " from old value " + old + " to " + _new);
#define XConfLog(oldVersion, newVersion) LOG(MODULE_CONFIG, "Migrating config from version " + oldVersion + " to " + newVersion);

namespace Qv2ray
{
    namespace QvConfigModels
    {
        // Secret member
        QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root)
        {
            XConfLog(to_string(fromVersion), to_string(fromVersion + 1))

            switch (fromVersion) {
                case 1: {
                    // From 1 to 2, we changed the config_version from 'string' to 'int'
                    auto old_config_version = root["config_version"].toString();
                    root.remove("config_version");
                    root["config_version"] = 2;
                    UPGRADELOG("config_version", old_config_version.toStdString(), "2")
                    break;
                }

                case 2 : {
                    // We copied those files.
                    auto vCoreFilePath = root["v2CorePath"].toString();
                    auto vCoreDestPath = QV2RAY_V2RAY_CORE_PATH;
                    // We also need v2ctl
                    auto v2CtlFilePath = QFileInfo(vCoreFilePath).dir().path() + "/v2ctl";
                    auto v2CtlDestPath = QFileInfo(vCoreDestPath).dir().path() + "/v2ctl";
#ifdef __WIN32
                    v2CtlFilePath = v2CtlFilePath.append(".exe");
                    v2CtlDestPath = v2CtlDestPath.append(".exe");
#endif
                    QFile::copy(vCoreFilePath, vCoreDestPath);
                    QFile::copy(v2CtlFilePath, v2CtlDestPath);
                    root.remove("v2CorePath");
                    UPGRADELOG("v2CorePath", vCoreFilePath.toStdString(), vCoreDestPath.toStdString())
                    UPGRADELOG("v2CtlFilePath", v2CtlFilePath.toStdString(), v2CtlDestPath.toStdString())
                    break;
                }

                case 3 : {
                    // We changed a key name in the config file.
                    //proxyDefault
                    auto oldProxyDefault = root["proxyDefault"].toBool();
                    root.remove("proxyDefault");
                    root["enableProxy"] = oldProxyDefault;
                    //enableProxy
                    UPGRADELOG("key: proxyDefault->enableProxy", to_string(oldProxyDefault), to_string(oldProxyDefault))
                }
            }

            root["config_version"] = root["config_version"].toInt() + 1;
            return root;
        }

        // Exported function
        QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root)
        {
            for (int i = fromVersion; i < toVersion; i++) {
                root = UpgradeConfig_Inc(i, root);
            }

            return root;
        }

    }
}
