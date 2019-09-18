//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "Qv2rayBase.h"

#define UPDATELOG(msg) LOG(MODULE_CONFIG, "[" + to_string(fromVersion) + "-" + to_string(fromVersion + 1) + "] --> " msg)

namespace Qv2ray
{
    namespace QvConfigModels
    {
        // Secret member
        QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root)
        {
            //
            auto v1_oldConfigVersion = root["config_version"].toString();
            auto v2_oldProxyCN = root["proxyCN"].toBool();
            auto v3_oldrunAsRoot = root["runAsRoot"].toBool();

            //
            switch (fromVersion) {
                case 1:
                    // From 1 to 2, we changed the config_version from 'string' to 'int'
                    root.remove("config_version");
                    root["config_version"] = 2;
                    UPDATELOG("Upgrading config_version from old value " + v1_oldConfigVersion.toStdString() + " to 2")
                    break;

                case 2:
                    // From 2 to 3, we changed the "proxyCN" to "bypassCN" as it's easier to understand....
                    root.remove("proxyCN");
                    root.insert("bypassCN", !v2_oldProxyCN);
                    UPDATELOG("Upgrading proxyCN to bypassCN and changed the value to " + to_string(!v2_oldProxyCN))
                    break;

                case 3:
                    // From 3 to 4, we changed 'runAsRoot' to 'tProxySupport'
                    root.remove("runAsRoot");
                    root.insert("tProxySupport", v3_oldrunAsRoot);
                    UPDATELOG("Upgrading runAsRoot to tProxySupport, the value is not changed: " + to_string(v3_oldrunAsRoot))
                    break;
            }

            root.insert("config_version", fromVersion + 1);
            return root;
        }

        // Exported function
        QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root)
        {
            LOG(MODULE_CONFIG, "Migrating config from version " + to_string(fromVersion) + " to " + to_string(toVersion))

            for (int i = fromVersion; i < toVersion; i++) {
                root = UpgradeConfig_Inc(i, root);
            }

            return root;
        }

    }
}
