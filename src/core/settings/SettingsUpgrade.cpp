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
            // Qv2ray version 2, RC 4
            case 6:
            {
                // Moved API Stats port from connectionConfig to apiConfig
                QJsonObject apiConfig;
                apiConfig["enableAPI"] = true;
                apiConfig["statsPort"] = root["connectionConfig"].toObject()["statsPort"].toInt();
                root["apiConfig"] = apiConfig;
                break;
            }

            case 7:
            {
                auto lang = root["uiConfig"].toObject()["language"].toString().replace("-", "_");
                auto uiConfig = root["uiConfig"].toObject();
                uiConfig["language"] = lang;
                root["uiConfig"] = uiConfig;
                UPGRADELOG("Changed language: " + lang);
                break;
            }

            // From version 8 to 9, we introduced a lot of new connection metainfo(s)
            case 8:
            {
                // Generate a default group
                QJsonObject defaultGroup;
                QStringList defaultGroupConnectionId;
                defaultGroup["displayName"] = QObject::tr("Default Group");
                QString defaultGroupId = "000000000000";

                if (!QDir(QV2RAY_CONNECTIONS_DIR + defaultGroupId).exists())
                {
                    QDir().mkpath(QV2RAY_CONNECTIONS_DIR + defaultGroupId);
                }

                QString autoStartId;
                UPGRADELOG("Upgrading connections...");
                QJsonObject rootConnections;

                for (auto config : root["configs"].toArray())
                {
                    UPGRADELOG("Migrating: " + config.toString());
                    //
                    // MOVE FILES.
                    // OLD PATH is at QV2RAY_CONFIG_DIR
                    auto filePath = QV2RAY_CONFIG_DIR + config.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
                    auto configFile = QFile(filePath);
                    auto newUuid = GenerateUuid();
                    DEBUG("Generated new UUID: " + newUuid);

                    // Check Autostart Id
                    if (root["autoStartConfig"].toObject()["subscriptionName"].toString().isEmpty())
                    {
                        if (root["autoStartConfig"].toObject()["connectionName"].toString() == config.toString())
                        {
                            autoStartId = newUuid;
                        }
                    }

                    if (configFile.exists())
                    {
                        auto newPath = QV2RAY_CONNECTIONS_DIR + defaultGroupId + "/" + newUuid + QV2RAY_CONFIG_FILE_EXTENSION;
                        configFile.rename(newPath);
                        UPGRADELOG("Moved: " + filePath + " to " + newPath);
                    }
                    else
                    {
                        UPGRADELOG("WARNING! This file is not found, possible loss of data!");
                        continue;
                    }

                    QJsonObject connectionObject;
                    connectionObject["displayName"] = config.toString();
                    defaultGroupConnectionId << newUuid;
                    DEBUG("Pushed uuid: " + newUuid + " to default group.");
                    rootConnections[newUuid] = connectionObject;
                }

                // Upgrading subscriptions.
                QJsonObject rootSubscriptions = root.take("subscriptions").toObject();
                QJsonObject newSubscriptions;

                for (auto i = 0; i < rootSubscriptions.count(); i++)
                {
                    auto key = rootSubscriptions.keys().at(i);
                    auto value = rootSubscriptions.value(key);
                    //
                    UPGRADELOG("Upgrading subscription: " + key);
                    QString subsUuid = GenerateUuid();
                    QJsonObject subs;
                    QStringList subsConnectionIds;
                    subs["address"] = value["address"].toString();
                    subs["lastUpdated"] = value["lastUpdated"];
                    subs["updateInterval"] = value["updateInterval"];
                    subs["displayName"] = key;
                    //
                    auto baseDirPath = QV2RAY_CONFIG_DIR + "/subscriptions/" + key;
                    auto newDirPath = QV2RAY_CONFIG_DIR + "/subscriptions/" + subsUuid;
                    QDir newDir(newDirPath);

                    if (!newDir.exists())
                    {
                        newDir.mkpath(newDirPath);
                    }

                    // With extensions
                    auto fileList = GetFileList(baseDirPath);

                    // Copy every file within a subscription.
                    for (const auto &fileName : fileList)
                    {
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

                        //

                        // Check Autostart Id
                        if (root["autoStartConfig"].toObject()["subscriptionName"].toString() == key)
                        {
                            if (root["autoStartConfig"].toObject()["connectionName"].toString() == subsConnection["displayName"].toString())
                            {
                                autoStartId = subsConnectionId;
                            }
                        }
                    }

                    subs["connections"] = QJsonArray::fromStringList(subsConnectionIds);
                    newSubscriptions[subsUuid] = subs;
                    QDir().rmdir(baseDirPath);
                }

                defaultGroup["connections"] = QJsonArray::fromStringList(defaultGroupConnectionId);
                QJsonObject groups;
                groups[defaultGroupId] = defaultGroup;
                root["autoStartId"] = autoStartId;
                root["groups"] = groups;
                root["connections"] = rootConnections;
                root["subscriptions"] = newSubscriptions;
                UPGRADELOG("Finished upgrading config, version 8.");
                break;
            }

                // Added cross-platform vCore and vAssets path support;
            case 9:
            {
                QJsonObject kernelConfig;
#ifdef Q_OS_LINUX
#define _VARNAME_VCOREPATH_ kernelConfig["v2CorePath_linux"]
#define _VARNAME_VASSETSPATH_ kernelConfig["v2AssetsPath_linux"]
                UPGRADELOG("Update kernel and assets paths for linux");
#elif defined(Q_OS_MACOS)
#define _VARNAME_VCOREPATH_ kernelConfig["v2CorePath_macx"]
#define _VARNAME_VASSETSPATH_ kernelConfig["v2AssetsPath_macx"]
                UPGRADELOG("Update kernel and assets paths for macOS");
#elif defined(Q_OS_WIN)
#define _VARNAME_VCOREPATH_ kernelConfig["v2CorePath_win"]
#define _VARNAME_VASSETSPATH_ kernelConfig["v2AssetsPath_win"]
                UPGRADELOG("Update kernel and assets paths for Windows");
#endif
                _VARNAME_VCOREPATH_ = root["v2CorePath"].toString();
                _VARNAME_VASSETSPATH_ = root["v2AssetsPath"].toString();
                //
                root["kernelConfig"] = kernelConfig;
#undef _VARNAME_VCOREPATH_
#undef _VARNAME_VASSETSPATH_
                break;
            }
            case 10:
            {
                // It's worth that use a seperated config number to notify all users about the deprecation of PAC

                if (root["inboundConfig"].toObject()["pacConfig"].toObject()["enablePAC"].toBool(false))
                {
                    QvMessageBoxWarn(
                        nullptr, QObject::tr("Deprecated"),
                        QObject::tr("PAC is now deprecated and is not encouraged to be used anymore.") + NEWLINE +
                            QObject::tr("It will be removed or be provided as a plugin in the future.") + NEWLINE + NEWLINE +
                            QObject::tr("PAC will still work currently, but please switch to the V2Ray built-in routing as soon as possible."));
                }
                else
                {
                    UPGRADELOG("It's a wise choice not to use PAC.");
                }
                break;
            }

            // Splitted Qv2ray.conf,
            case 11:
            {
                // Process AutoStartSettings
                ConnectionGroupPair autoStartIdPair{ ConnectionId{ root["autoStartId"].toString() }, NullGroupId };

                // Process connection entries.
                //
                {
                    // Moved root["connections"] into separated file: $QV2RAY_CONFIG_PATH/connections.json
                    QDir connectionsDir(QV2RAY_CONNECTIONS_DIR);
                    if (!connectionsDir.exists())
                    {
                        connectionsDir.mkpath(QV2RAY_CONNECTIONS_DIR);
                    }
                    const auto connectionsArray = root["connections"].toObject().keys();
                    QJsonObject newConnectionsArray;
                    ///
                    /// Connection.json
                    /// {
                    ///     "connections" : [
                    ///         {ID1, connectionObject 1},
                    ///         {ID2, connectionObject 2},
                    ///         {ID3, connectionObject 3},
                    ///         {ID4, connectionObject 4},
                    ///      ]
                    /// }
                    ///
                    for (const auto &connectionVal : connectionsArray)
                    {
                        // Config file migrations:
                        // Connection Object:
                        //      importDate --> creationDate
                        //      lastUpdatedDate --> now
                        //
                        auto connection = root["connections"].toObject()[connectionVal].toObject();
                        connection["creationDate"] = connection.take("importDate");
                        connection["lastUpdatedDate"] = (qint64) system_clock::to_time_t(system_clock::now());
                        UPGRADELOG("Migrating connection: " + connectionVal + " -- " + connection["displayName"].toString());
                        newConnectionsArray[connectionVal] = connection;
                    }
                    QJsonObject ConnectionJsonObject;
                    root["connections"] = QJsonArray::fromStringList(connectionsArray);
                    //
                    // Store Connection.json
                    StringToFile(JsonToString(newConnectionsArray), QV2RAY_CONFIG_DIR + "connections.json");
                }
                // Merged groups and subscriptions. $QV2RAY_GROUPS_PATH + groupId.json
                {
                    // Susbcription Object
                    //      Doesn't exist anymore, convert into normal group Object.
                    //
                    QMap<QString, QJsonObject> ConnectionsCache;
                    QJsonObject allGroupsObject;
                    const auto subscriptionKeys = root["subscriptions"].toObject().keys();
                    for (const auto &key : subscriptionKeys)
                    {
                        auto aSubscription = root["subscriptions"].toObject()[key].toObject();
                        QJsonObject subscriptionSettings;
                        subscriptionSettings["address"] = aSubscription.take("address");
                        subscriptionSettings["updateInterval"] = aSubscription.take("updateInterval");
                        aSubscription["lastUpdatedDate"] = (qint64) system_clock::to_time_t(system_clock::now());
                        aSubscription["creationDate"] = (qint64) system_clock::to_time_t(system_clock::now());
                        aSubscription["subscriptionOption"] = subscriptionSettings;
                        UPGRADELOG("Migrating subscription: " + key + " -- " + aSubscription["displayName"].toString());
                        //
                        if (autoStartIdPair.groupId != NullGroupId &&
                            aSubscription["connections"].toArray().contains(autoStartIdPair.connectionId.toString()))
                        {
                            autoStartIdPair.groupId = GroupId{ key };
                        }
                        //
                        for (const auto &cid : aSubscription["connections"].toArray())
                        {
                            ConnectionsCache[cid.toString()] = JsonFromString(
                                StringFromFile(QV2RAY_CONFIG_DIR + "subscriptions/" + key + "/" + cid.toString() + QV2RAY_CONFIG_FILE_EXTENSION));
                        }
                        //
                        allGroupsObject[key] = aSubscription;
                    }
                    //
                    root.remove("subscriptions");
                    //
                    const auto groupKeys = root["groups"].toObject().keys();
                    for (const auto &key : groupKeys)
                    {
                        // Group Object
                        //      connections ---> ConnectionID
                        //      idSubscription ---> if the group is a subscription
                        //      subscriptionSettings ---> Originally SubscriptionObject
                        //      creationDate ---> Now
                        //      lastUpdateDate ---> Now
                        auto aGroup = root["groups"].toObject()[key].toObject();
                        aGroup["lastUpdatedDate"] = (qint64) system_clock::to_time_t(system_clock::now());
                        aGroup["creationDate"] = (qint64) system_clock::to_time_t(system_clock::now());
                        UPGRADELOG("Migrating group: " + key + " -- " + aGroup["displayName"].toString());
                        //
                        if (autoStartIdPair.groupId != NullGroupId &&
                            aGroup["connections"].toArray().contains(autoStartIdPair.connectionId.toString()))
                        {
                            autoStartIdPair.groupId = GroupId{ key };
                        }
                        for (const auto &cid : aGroup["connections"].toArray())
                        {
                            ConnectionsCache[cid.toString()] = JsonFromString(
                                StringFromFile(QV2RAY_CONFIG_DIR + "connections/" + key + "/" + cid.toString() + QV2RAY_CONFIG_FILE_EXTENSION));
                        }
                        //
                        allGroupsObject[key] = aGroup;
                    }
                    //
                    StringToFile(JsonToString(allGroupsObject), QV2RAY_CONFIG_DIR + "groups.json");
                    //
                    root.remove("groups"); //
                    UPGRADELOG("Removing unused directory");
                    QDir(QV2RAY_CONFIG_DIR + "subscriptions/").removeRecursively();
                    QDir(QV2RAY_CONFIG_DIR + "connections/").removeRecursively();
                    //
                    QDir().mkpath(QV2RAY_CONFIG_DIR + "connections/");
                    //
                    //
                    // FileSystem Migrations
                    //      Move all files in GROUPS / SUBSCRIPTION subfolders into CONNECTIONS.
                    //      Only Store (connections.json in CONFIG_PATH) and ($groupID.json in GROUP_PATH)
                    for (const auto &cid : ConnectionsCache.keys())
                    {
                        StringToFile(JsonToString(ConnectionsCache[cid]), QV2RAY_CONFIG_DIR + "connections/" + cid + QV2RAY_CONFIG_FILE_EXTENSION);
                    }
                    //
                }

                //
                // Main Object
                //      Drop recentConnections since it's ill-formed and not supported yet.
                //      convert autoStartId into ConnectionGroupPair / instead of QString
                //      Remove subscriptions item.
                root.remove("recentConnections");
                root["autoStartId"] = autoStartIdPair.toJson();
                // 1 here means FIXED
                root["autoStartBehavior"] = 1;

                // Moved apiConfig into kernelConfig
                auto kernelConfig = root["kernelConfig"].toObject();
                kernelConfig["enableAPI"] = root["apiConfig"].toObject()["enableAPI"];
                kernelConfig["statsPort"] = root["apiConfig"].toObject()["statsPort"];
                root["kernelConfig"] = kernelConfig;
                UPGRADELOG("Finished upgrading config file for Qv2ray Group Routing update.");
                break;
            }
            case 12:
            {
                auto inboundConfig = root["inboundConfig"].toObject();
                //
                QJsonObject socksSettings;
                QJsonObject httpSettings;
                QJsonObject tProxySettings;
                QJsonObject systemProxySettings;
                systemProxySettings["setSystemProxy"] = inboundConfig["setSystemProxy"];
                //
                socksSettings["port"] = inboundConfig["socks_port"];
                socksSettings["useAuth"] = inboundConfig["socks_useAuth"];
                socksSettings["enableUDP"] = inboundConfig["socksUDP"];
                socksSettings["localIP"] = inboundConfig["socksLocalIP"];
                socksSettings["account"] = inboundConfig["socksAccount"];
                socksSettings["sniffing"] = inboundConfig["socksSniffing"];
                //
                httpSettings["port"] = inboundConfig["http_port"];
                httpSettings["useAuth"] = inboundConfig["http_useAuth"];
                httpSettings["account"] = inboundConfig["httpAccount"];
                httpSettings["sniffing"] = inboundConfig["httpSniffing"];
                //
                tProxySettings["tProxyIP"] = inboundConfig["tproxy_ip"];
                tProxySettings["port"] = inboundConfig["tproxy_port"];
                tProxySettings["hasTCP"] = inboundConfig["tproxy_use_tcp"];
                tProxySettings["hasUDP"] = inboundConfig["tproxy_use_udp"];
                tProxySettings["followRedirect"] = inboundConfig["tproxy_followRedirect"];
                tProxySettings["mode"] = inboundConfig["tproxy_mode"];
                tProxySettings["dnsIntercept"] = inboundConfig["dnsIntercept"];
                //
                inboundConfig["systemProxySettings"] = systemProxySettings;
                inboundConfig["socksSettings"] = socksSettings;
                inboundConfig["httpSettings"] = httpSettings;
                inboundConfig["tProxySettings"] = tProxySettings;
                //
                root["inboundConfig"] = inboundConfig;
                break;
            }
            case 13:
            {
                const auto dnsList = QJsonIO::GetValue(root, "connectionConfig", "dnsList").toArray();
                auto connectionConfig = root["connectionConfig"].toObject();
                QJsonObject defaultRouteConfig;
                defaultRouteConfig["forwardProxyConfig"] = connectionConfig.take("forwardProxyConfig");
                defaultRouteConfig["routeConfig"] = connectionConfig.take("routeConfig");

                for (auto i = 0; i < dnsList.count(); i++)
                {
                    QJsonIO::SetValue(defaultRouteConfig, dnsList[i], "dnsConfig", "servers", i, "address");
                    QJsonIO::SetValue(defaultRouteConfig, false, "dnsConfig", "servers", i, "QV2RAY_DNS_IS_COMPLEX_DNS");
                }
                root["defaultRouteConfig"] = defaultRouteConfig;
                break;
            }
            default:
            {
                //
                // Due to technical issue, we cannot maintain all of those upgrade processes anymore. Check
                // https://github.com/Qv2ray/Qv2ray/issues/353#issuecomment-586117507
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
