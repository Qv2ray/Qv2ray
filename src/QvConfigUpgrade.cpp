#include "Qv2rayBase.h"

#define XConfLog(oldVersion, newVersion) LOG(MODULE_CONFIG, "Migrating config from version " \
                                                + to_string(oldVersion) + " to " + to_string(newVersion))

namespace Qv2ray {
    namespace QvConfigModels {
        // Secret member
        QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root) {
            XConfLog(fromVersion, fromVersion + 1);
            switch (fromVersion) {
            case 1:
                break;
            }
            return root;
        }

        // Exported function
        QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root) {
            for (int i = fromVersion; i < toVersion; i++) {
                root = UpgradeConfig_Inc(i, root);
            }
            return root;
        }

    }
}
