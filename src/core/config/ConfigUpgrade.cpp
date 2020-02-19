//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"

#define UPGRADELOG(msg) LOG(MODULE_SETTINGS, "  [" + QSTRN(fromVersion) + "-" + QSTRN(fromVersion + 1) + "] --> " + msg)

namespace Qv2ray
{
    // Private member
    QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root)
    {
        switch (fromVersion) {
            // --------------------------------------------------------------------------------------
            // Below is for Qv2ray version 2
            case 4: {
                // We changed the "proxyCN" to "bypassCN" as it's easier to understand....
                auto v2_oldProxyCN = root["proxyCN"].toBool();
                //
                // From 3 to 4, we changed 'runAsRoot' to 'tProxySupport'
                auto v3_oldrunAsRoot = root["runAsRoot"].toBool();
                root.insert("tProxySupport", v3_oldrunAsRoot);
                UPGRADELOG("Upgrading runAsRoot to tProxySupport, the value is not changed: " + QSTRN(v3_oldrunAsRoot))
                //
                QString path;
                path = QV2RAY_DEFAULT_VCORE_PATH;
                root["v2CorePath"] = path;
                UPGRADELOG("Added v2CorePath to the config file.")
                //
                QJsonObject uiSettings;
                uiSettings["language"] = root["language"].toString("en-US").replace("-", "_");
                root["uiConfig"] = uiSettings;
                //
                root["inboundConfig"] = root["inBoundSettings"];
                root.remove("inBoundSettings");
                UPGRADELOG("Renamed inBoundSettings to inboundConfig.")
                //
                //connectionConfig
                QJsonObject o;
                o["dnsList"] = root["dnsList"];
                o["withLocalDNS"] = root["withLocalDNS"];
                o["enableProxy"] = root["enableProxy"];
                o["bypassCN"] = !v2_oldProxyCN;
                o["enableStats"] = true;
                o["statsPort"] = 13459;
                UPGRADELOG("Default statistics enabled.")
                root["connectionConfig"] = o;
                UPGRADELOG("Renamed some connection configs to connectionConfig.")
                //
                // Do we need renaming here?
                // //auto inbound = root["inboundConfig"].toObject();
                // //auto pacConfig = inbound["pacConfig"].toObject();
                // //pacConfig["enablePAC"] = pacConfig["usePAC"].toBool();
                // //inbound["pacConfig"] = pacConfig;
                // //root["inboundConfig"] = inbound;
                // //UPDATELOG("Renamed usePAC to enablePAC.")
                //
                QJsonObject i;
                i["connectionName"] = root["autoStartConfig"].toString();
                root["autoStartConfig"] = i;
                UPGRADELOG("Added subscription feature to autoStartConfig.")
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
                    SubscriptionObject _conf;
                    _conf.address = item.value().toString();
                    _conf.lastUpdated = system_clock::to_time_t(system_clock::now());
                    _conf.updateInterval = 5;
                    auto value = GetRootObject(_conf);
                    newSubscriptions[key] = value;
                }

                root["subscriptions"] = newSubscriptions;
                UPGRADELOG("Added subscription renewal options.")
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
                UPGRADELOG("Changed language: " + lang)
                break;
            }

            // From version 8 to 9, we introduced a lot of new connection metainfo(s)
            case 8: {
                // Generate a default group
                QJsonObject defaultGroup;
                QStringList defaultGroupConnectionId;
                defaultGroup["displayName"] = QObject::tr("Default Group");
                QString defaultGroupId = "000000000000";

                if (!QDir(QV2RAY_CONNECTIONS_DIR + defaultGroupId).exists()) {
                    QDir().mkpath(QV2RAY_CONNECTIONS_DIR + defaultGroupId);
                }

                QString autoStartId;
                UPGRADELOG("Upgrading connections...")
                QJsonObject rootConnections;

                for (auto config : root["configs"].toArray()) {
                    UPGRADELOG("Migrating: " + config.toString())
                    //
                    // MOVE FILES.
                    // OLD PATH is at QV2RAY_CONFIG_DIR
                    auto filePath = QV2RAY_CONFIG_DIR + config.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
                    auto configFile = QFile(filePath);
                    auto newUuid = GenerateUuid();
                    DEBUG(MODULE_SETTINGS, "Generated new UUID: " + newUuid);

                    if (configFile.exists()) {
                        auto newPath = QV2RAY_CONNECTIONS_DIR + defaultGroupId + "/" + newUuid + QV2RAY_CONFIG_FILE_EXTENSION;
                        configFile.rename(newPath);
                        UPGRADELOG("Moved: " + filePath + " to " + newPath);
                    } else {
                        UPGRADELOG("WARNING! This file is not found, possible loss of data!")
                        continue;
                    }

                    QJsonObject connectionObject;
                    connectionObject["displayName"] = config.toString();
                    defaultGroupConnectionId << newUuid;
                    DEBUG(MODULE_SETTINGS, "Pushed uuid: " + newUuid + " to default group.")
                    rootConnections[newUuid] = connectionObject;
                }

                // Upgrading subscriptions.
                QJsonObject rootSubscriptions = root.take("subscriptions").toObject();
                QJsonObject newSubscriptions;

                for (auto i = 0; i < rootSubscriptions.count(); i++) {
                    auto key = rootSubscriptions.keys()[i];
                    auto value = rootSubscriptions.value(key);
                    //
                    UPGRADELOG("Upgrading subscription: " + key)
                    QString subsUuid = GenerateUuid();
                    QJsonObject subs;
                    QStringList subsConnectionIds;
                    subs["address"] = value["address"].toString();
                    subs["lastUpdated"] = value["lastUpdated"];
                    subs["updateInterval"] = value["updateInterval"];
                    subs["displayName"] = key;
                    //
                    auto baseDirPath = QV2RAY_SUBSCRIPTION_DIR + key;
                    auto newDirPath = QV2RAY_SUBSCRIPTION_DIR + subsUuid;
                    QDir newDir(newDirPath);

                    if (!newDir.exists()) {
                        newDir.mkpath(newDirPath);
                    }

                    // With extensions
                    auto fileList = GetFileList(baseDirPath);

                    // Copy every file within a subscription.
                    for (auto fileName : fileList) {
                        auto subsConnectionId = GenerateUuid();
                        auto baseFilePath = baseDirPath + "/" + fileName;
                        auto newFilePath = newDirPath + "/" + subsConnectionId + QV2RAY_CONFIG_FILE_EXTENSION;
                        //
                        QJsonObject subsConnection;
                        subsConnection["displayName"] = fileName.chopped(QString(QV2RAY_CONFIG_FILE_EXTENSION).count());
                        QFile(baseFilePath).rename(newFilePath);
                        UPGRADELOG("Moved subscription file from: " + baseFilePath + " to: " + newFilePath);
                        subsConnectionIds << subsConnectionId;
                        rootConnections[subsConnectionId] = subsConnection;
                    }

                    subs["connections"] = QJsonArray::fromStringList(subsConnectionIds);
                    newSubscriptions[subsUuid] = subs;
                    QDir().rmdir(baseDirPath);
                }

                defaultGroup["connections"] = QJsonArray::fromStringList(defaultGroupConnectionId);
                QJsonObject groups;
                groups[defaultGroupId] = defaultGroup;
                root["groups"] = groups;
                root["connections"] = rootConnections;
                root["subscriptions"] = newSubscriptions;
                UPGRADELOG("Finished upgrading config, version 8.")
                break;
            }

            default: {
                // Due to technical issue, we cannot maintain all of those upgrade processes anymore.
                // Check https://github.com/Qv2ray/Qv2ray/issues/353#issuecomment-586117507 for more information
                QvMessageBoxWarn(nullptr, QObject::tr("Configuration Upgrade Failed"),
                                 QObject::tr("Unsupported config version number: ") + QSTRN(fromVersion) + NEWLINE + NEWLINE +
                                 QObject::tr("Please upgrade firstly up to Qv2ray v2.0/v2.1 and try again."));
                throw new runtime_error("The configuration version of your old Qv2ray installation is out-of-date and that"
                                        " version is not supported anymore, please try to update to an intermediate version of Qv2ray first.");
            }
        }

        root["config_version"] = root["config_version"].toInt() + 1;
        return root;
    }

    // Exported function
    QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root)
    {
        LOG(MODULE_SETTINGS, "Migrating config from version " + QSTRN(fromVersion) + " to " + QSTRN(toVersion))

        for (int i = fromVersion; i < toVersion; i++) {
            root = UpgradeConfig_Inc(i, root);
        }

        return root;
    }
}
