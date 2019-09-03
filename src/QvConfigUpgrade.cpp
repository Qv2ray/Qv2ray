//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "Qv2rayBase.h"

#define UPGRADELOG(item, old, _new) LOG(MODULE_CONFIG, "Upgrading " item " from old value " + old + " to " + _new);
#define XConfLog(oldVersion, newVersion) LOG(MODULE_CONFIG, "Migrating config from version " + oldVersion + " to " + newVersion);

namespace Qv2ray
{
    namespace QvConfigModels
    {
        // Secret member
        QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root)
        {
            XConfLog(to_string(fromVersion), to_string(fromVersion + 1));

            switch (fromVersion) {
                case 1:
                    // From 1 to 2, we changed the config_version from 'string' to 'int'
                    auto old_config_version = root["config_version"].toString();
                    root.remove("config_version");
                    root["config_version"] = 2;
                    UPGRADELOG("config_version", old_config_version.toStdString(), "2")
                    break;
            }

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
