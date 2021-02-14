//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "base/Qv2rayBase.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "SettingsUpgrade"

#define UPGRADELOG(msg) LOG("[" + QSTRN(fromVersion) + "-" + QSTRN(fromVersion + 1) + "] --> " + msg)

namespace Qv2ray
{
    // Private member
    QJsonObject UpgradeConfig_Inc(int fromVersion, const QJsonObject &original)
    {
        auto root = original;
        switch (fromVersion)
        {
            default:
            {
                //
                // Due to technical issue, we cannot maintain all of those upgrade processes anymore. Check
                // https://github.com/Qv2ray/Qv2ray/issues/353#issuecomment-586117507
                // https://github.com/Qv2ray/Qv2ray/issues/1284#issuecomment-778776959
                // for more information, see commit 2f716a9a443b71ddb96aaab081de73c0095cb637
                //
                QvMessageBoxWarn(nullptr, QObject::tr("Configuration Upgrade Failed"),
                                 QObject::tr("Unsupported config version number: ") + QSTRN(fromVersion) + NEWLINE + NEWLINE +
                                     QObject::tr("Please upgrade firstly up to Qv2ray v2.0/v2.1 and try again."));
                LOG("The configuration version of your old Qv2ray installation is out-of-date and that"
                    " version is not supported anymore, please try to update to an intermediate version of Qv2ray first.");
                qApp->exit(1);
            }
        }
        root["config_version"] = root["config_version"].toInt() + 1;
        return root;
    }

    // Exported function
    QJsonObject UpgradeSettingsVersion(int fromVersion, int toVersion, const QJsonObject &original)
    {
        auto root = original;
        LOG("Migrating config from version ", fromVersion, "to", toVersion);

        for (int i = fromVersion; i < toVersion; i++)
        {
            root = UpgradeConfig_Inc(i, root);
        }

        return root;
    }
} // namespace Qv2ray
