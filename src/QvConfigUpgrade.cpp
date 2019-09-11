//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "Qv2rayBase.h"

#define XConfLog(oldVersion, newVersion) LOG(MODULE_CONFIG, "Migrating config from version " + oldVersion + " to " + newVersion);

namespace Qv2ray
{
    namespace QvConfigModels
    {
        // Secret member
        QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root)
        {
            XConfLog(to_string(fromVersion), to_string(fromVersion + 1))
            //
            auto v1_oldConfigVersion = root["config_version"].toString();
            auto v2_oldProxyCN = root["proxyCN"].toBool();

            //
            switch (fromVersion) {
                case 1:
                    // From 1 to 2, we changed the config_version from 'string' to 'int'
                    root.remove("config_version");
                    root["config_version"] = 2;
                    LOG(MODULE_CONFIG, ("Upgrading config_version from old value " + v1_oldConfigVersion.toStdString() + " to 2"))
                    break;

                case 2:
                    // From 2 to 3, we changed the "proxyCN" to "bypassCN" as it's easier to understand....
                    root.remove("proxyCN");
                    root.insert("bypassCN", !v2_oldProxyCN);
                    LOG(MODULE_CONFIG, ("Upgrading proxyCN to bypassCN and changed the value to " + to_string(!v2_oldProxyCN)))
                    break;
            }

            root.insert("config_version", fromVersion + 1);
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
