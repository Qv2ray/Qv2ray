#include "ConfigHandler.hpp"

#include "common/QvHelpers.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "core/connection/Serialization.hpp"
#include "core/settings/SettingsBackend.hpp"

namespace Qv2ray::core::handlers
{
    QvConfigHandler::QvConfigHandler(QObject *parent) : QObject(parent)
    {
        DEBUG(MODULE_CORE_HANDLER, "ConnectionHandler Constructor.")
        const auto connectionJson = JsonFromString(StringFromFile(QV2RAY_CONFIG_DIR + "connections.json"));
        const auto groupJson = JsonFromString(StringFromFile(QV2RAY_CONFIG_DIR + "groups.json"));
        //
        for (const auto &connectionId : connectionJson.keys())
        {
            connections.insert(ConnectionId{ connectionId }, ConnectionObject::fromJson(connectionJson.value(connectionId).toObject()));
        }
        //
        for (const auto &groupId : groupJson.keys())
        {
            const auto groupObject = GroupObject::fromJson(groupJson.value(groupId).toObject());
            groups.insert(GroupId{ groupId }, groupObject);
            for (const auto &connId : groupObject.connections)
            {
                connections[connId].__qvConnectionRefCount++;
            }
        }
        //
        for (const auto &id : connections.keys())
        {
            auto const &connectionObject = connections.value(id);
            if (connectionObject.__qvConnectionRefCount == 0)
            {
                QFile connectionFile(QV2RAY_CONNECTIONS_DIR + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
                if (connectionFile.exists())
                {
                    if (!connectionFile.remove())
                        LOG(MODULE_CONNECTION, "Failed to remove connection config file")
                }
                connections.remove(id);
                LOG(MODULE_CORE_HANDLER, "Dropped connection id: " + id.toString() + " since it's not in a group")
            }
            else
            {
                const auto connectionFilePath = QV2RAY_CONNECTIONS_DIR + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
                connectionRootCache[id] = CONFIGROOT(JsonFromString(StringFromFile(connectionFilePath)));
                DEBUG(MODULE_CORE_HANDLER, "Loaded connection id: " + id.toString() + " into cache.")
            }
        }

        // Force default group name.
        if (!groups.contains(DefaultGroupId))
        {
            groups.insert(DefaultGroupId, {});
            groups[DefaultGroupId].displayName = tr("Default Group");
            groups[DefaultGroupId].isSubscription = false;
        }
        //
        kernelHandler = new KernelInstanceHandler(this);
        connect(kernelHandler, &KernelInstanceHandler::OnCrashed, this, &QvConfigHandler::OnKernelCrashed_p);
        connect(kernelHandler, &KernelInstanceHandler::OnStatsDataAvailable, this, &QvConfigHandler::OnStatsDataArrived_p);
        connect(kernelHandler, &KernelInstanceHandler::OnKernelLogAvailable, this, &QvConfigHandler::OnKernelLogAvailable);
        connect(kernelHandler, &KernelInstanceHandler::OnConnected, this, &QvConfigHandler::OnConnected);
        connect(kernelHandler, &KernelInstanceHandler::OnDisconnected, this, &QvConfigHandler::OnDisconnected);
        //
        tcpingHelper = new LatencyTestHost(5, this);
        connect(tcpingHelper, &LatencyTestHost::OnLatencyTestCompleted, this, &QvConfigHandler::OnLatencyDataArrived_p);
        //
        // Save per 1 minutes.
        saveTimerId = startTimer(1 * 60 * 1000);
        // Do not ping all...
        pingConnectionTimerId = startTimer(60 * 1000);
    }

    void QvConfigHandler::SaveConnectionConfig()
    {
        QJsonObject connectionsObject;
        for (const auto &key : connections.keys())
        {
            connectionsObject[key.toString()] = connections[key].toJson();
        }
        StringToFile(JsonToString(connectionsObject), QV2RAY_CONFIG_DIR + "connections.json");
        //
        QJsonObject groupObject;
        for (const auto &key : groups.keys())
        {
            groupObject[key.toString()] = groups[key].toJson();
        }
        StringToFile(JsonToString(groupObject), QV2RAY_CONFIG_DIR + "groups.json");
        SaveGlobalSettings();
    }

    void QvConfigHandler::timerEvent(QTimerEvent *event)
    {
        if (event->timerId() == saveTimerId)
        {
            SaveConnectionConfig();
        }
        else if (event->timerId() == pingAllTimerId)
        {
            StartLatencyTest();
        }
        else if (event->timerId() == pingConnectionTimerId)
        {
            auto id = kernelHandler->CurrentConnection();
            if (!id.isEmpty() && GlobalConfig.advancedConfig.testLatencyPeriodcally)
            {
                StartLatencyTest(id.connectionId);
            }
        }
    }

    void QvConfigHandler::StartLatencyTest()
    {
        for (const auto &connection : connections.keys())
        {
            StartLatencyTest(connection);
        }
    }

    void QvConfigHandler::StartLatencyTest(const GroupId &id)
    {
        for (const auto &connection : groups[id].connections)
        {
            StartLatencyTest(connection);
        }
    }

    void QvConfigHandler::StartLatencyTest(const ConnectionId &id)
    {
        emit OnLatencyTestStarted(id);
        tcpingHelper->TestLatency(id, GlobalConfig.networkConfig.latencyTestingMethod);
    }

    const QList<GroupId> QvConfigHandler::Subscriptions() const
    {
        QList<GroupId> subsList;

        for (const auto &group : groups.keys())
        {
            if (groups[group].isSubscription)
            {
                subsList.push_back(group);
            }
        }

        return subsList;
    }

    void QvConfigHandler::ClearGroupUsage(const GroupId &id)
    {
        for (const auto &conn : groups[id].connections)
        {
            ClearConnectionUsage({ conn, id });
        }
    }
    void QvConfigHandler::ClearConnectionUsage(const ConnectionGroupPair &id)
    {
        CheckConnectionExistanceEx(id.connectionId, nothing);
        connections[id.connectionId].upLinkData = 0;
        connections[id.connectionId].downLinkData = 0;
        emit OnStatsAvailable(id, 0, 0, 0, 0);
        PluginHost->Send_ConnectionStatsEvent({ GetDisplayName(id.connectionId), 0, 0, 0, 0 });
        return;
    }

    const QList<GroupId> QvConfigHandler::GetGroupId(const ConnectionId &connId) const
    {
        CheckConnectionExistanceEx(connId, {});
        QList<GroupId> grps;
        for (const auto &groupId : groups.keys())
        {
            const auto &group = groups[groupId];
            if (group.connections.contains(connId))
            {
                grps.push_back(groupId);
            }
        }
        return grps;
    }

    const std::optional<QString> QvConfigHandler::RenameConnection(const ConnectionId &id, const QString &newName)
    {
        CheckConnectionExistance(id);
        OnConnectionRenamed(id, connections[id].displayName, newName);
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::Renamed, newName, connections[id].displayName });
        connections[id].displayName = newName;
        SaveConnectionConfig();
        return {};
    }

    bool QvConfigHandler::RemoveConnectionFromGroup(const ConnectionId &id, const GroupId &gid)
    {
        CheckConnectionExistanceEx(id, false);
        LOG(MODULE_CONNECTION, "Removing connection : " + id.toString())
        if (groups[gid].connections.contains(id))
        {
            auto removedEntries = groups[gid].connections.removeAll(id);
            if (removedEntries > 1)
            {
                LOG(MODULE_CONNECTION, "Found same connection occured multiple times in a group.")
            }
            // Decrease reference count.
            connections[id].__qvConnectionRefCount -= removedEntries;
        }

        if (GlobalConfig.autoStartId == ConnectionGroupPair{ id, gid })
        {
            GlobalConfig.autoStartId.clear();
        }
        //
        // Emit everything first then clear the connection map.
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::RemovedFromGroup, GetDisplayName(id), "" });

        emit OnConnectionRemovedFromGroup({ id, gid });

        //
        if (connections[id].__qvConnectionRefCount <= 0)
        {
            LOG(MODULE_CONNECTION, "Fully removing a connection from cache.")
            connectionRootCache.remove(id);
            //
            QFile connectionFile(QV2RAY_CONNECTIONS_DIR + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
            if (connectionFile.exists())
            {
                if (!connectionFile.remove())
                    LOG(MODULE_CONNECTION, "Failed to remove connection config file")
            }
            connections.remove(id);
        }
        return true;
    }

    bool QvConfigHandler::LinkConnectionWithGroup(const ConnectionId &id, const GroupId &newGroupId)
    {
        CheckConnectionExistanceEx(id, false);
        if (groups[newGroupId].connections.contains(id))
        {
            LOG(MODULE_CONNECTION, "Connection not linked since " + id.toString() + " is already in the group " + newGroupId.toString())
            return false;
        }
        groups[newGroupId].connections.append(id);
        connections[id].__qvConnectionRefCount++;
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::LinkedWithGroup, connections[id].displayName, "" });
        emit OnConnectionLinkedWithGroup({ id, newGroupId });
        return true;
    }

    bool QvConfigHandler::MoveConnectionFromToGroup(const ConnectionId &id, const GroupId &sourceGid, const GroupId &targetGid)
    {
        CheckConnectionExistanceEx(id, false);
        CheckGroupExistanceEx(targetGid, false);
        CheckGroupExistanceEx(sourceGid, false);
        //
        if (!groups[sourceGid].connections.contains(id))
        {
            LOG(MODULE_CONNECTION, "Trying to move a connection away from a group it does not belong to.")
            return false;
        }
        if (groups[targetGid].connections.contains(id))
        {
            LOG(MODULE_CONNECTION, "The connection: " + id.toString() + " has already been in the target group: " + targetGid.toString())
            auto removedCount = groups[sourceGid].connections.removeAll(id);
            connections[id].__qvConnectionRefCount -= removedCount;
        }
        else
        {
            // If the target group does not contain this connection.
            auto removedCount = groups[sourceGid].connections.removeAll(id);
            connections[id].__qvConnectionRefCount -= removedCount;
            //
            groups[targetGid].connections.append(id);
            connections[id].__qvConnectionRefCount++;
        }

        emit OnConnectionRemovedFromGroup({ id, sourceGid });
        emit OnConnectionLinkedWithGroup({ id, targetGid });

        return true;
    }

    const std::optional<QString> QvConfigHandler::DeleteGroup(const GroupId &id)
    {
        CheckGroupExistance(id);
        if (!groups.contains(id) || id == NullGroupId)
        {
            return tr("Group does not exist");
        }

        // Copy construct
        auto list = groups[id].connections;
        for (const auto &conn : list)
        {
            MoveConnectionFromToGroup(conn, id, DefaultGroupId);
        }
        //
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::FullyRemoved, groups[id].displayName, "" });
        //
        groups.remove(id);
        SaveConnectionConfig();
        emit OnGroupDeleted(id, list);
        if (id == DefaultGroupId)
        {
            groups[id].displayName = tr("Default Group");
        }
        return {};
    }

    bool QvConfigHandler::StartConnection(const ConnectionGroupPair &identifier)
    {
        CheckConnectionExistanceEx(identifier.connectionId, false);
        connections[identifier.connectionId].lastConnected = system_clock::to_time_t(system_clock::now());
        CONFIGROOT root = GetConnectionRoot(identifier.connectionId);
        auto errMsg = kernelHandler->StartConnection(identifier, root);
        if (errMsg)
        {
            QvMessageBoxWarn(nullptr, tr("Failed to start connection"), *errMsg);
            return false;
        }
        GlobalConfig.lastConnectedId = identifier;
        return true;
    }

    void QvConfigHandler::RestartConnection() // const ConnectionId &id
    {
        kernelHandler->StopConnection();
        kernelHandler->StartConnection(GlobalConfig.lastConnectedId, GetConnectionRoot(GlobalConfig.lastConnectedId.connectionId));
    }

    void QvConfigHandler::StopConnection() // const ConnectionId &id
    {
        kernelHandler->StopConnection();
        SaveConnectionConfig();
    }

    void QvConfigHandler::OnKernelCrashed_p(const ConnectionGroupPair &id, const QString &errMessage)
    {
        LOG(MODULE_CORE_HANDLER, "Kernel crashed: " + errMessage)
        emit OnDisconnected(id);
        PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), {}, Events::Connectivity::Disconnected });
        emit OnKernelCrashed(id, errMessage);
    }

    QvConfigHandler::~QvConfigHandler()
    {
        LOG(MODULE_CORE_HANDLER, "Triggering save settings from destructor")
        tcpingHelper->StopAllLatencyTest();
        delete kernelHandler;
        SaveConnectionConfig();
    }

    const CONFIGROOT QvConfigHandler::GetConnectionRoot(const ConnectionId &id) const
    {
        CheckConnectionExistanceEx(id, CONFIGROOT());
        return connectionRootCache.value(id);
    }

    void QvConfigHandler::OnLatencyDataArrived_p(const ConnectionId &id, const LatencyTestResult &result)
    {
        CheckConnectionExistanceEx(id, nothing);
        connections[id].latency = result.avg;
        emit OnLatencyTestFinished(id, result.avg);
    }

    bool QvConfigHandler::UpdateConnection(const ConnectionId &id, const CONFIGROOT &root, bool skipRestart)
    {
        CheckConnectionExistanceEx(id, false);
        //
        auto path = QV2RAY_CONNECTIONS_DIR + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        auto content = JsonToString(root);
        bool result = StringToFile(content, path);
        //
        connectionRootCache[id] = root;
        //
        emit OnConnectionModified(id);
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::Edited, connections[id].displayName, "" });
        if (!skipRestart && kernelHandler->CurrentConnection().connectionId == id)
        {
            emit RestartConnection();
        }
        return result;
    }

    const GroupId QvConfigHandler::CreateGroup(const QString &displayName, bool isSubscription)
    {
        GroupId id(GenerateRandomString());
        groups[id].displayName = displayName;
        groups[id].isSubscription = isSubscription;
        groups[id].creationDate = system_clock::to_time_t(system_clock::now());
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::Created, displayName, "" });
        emit OnGroupCreated(id, displayName);
        SaveConnectionConfig();
        return id;
    }

    const std::optional<QString> QvConfigHandler::RenameGroup(const GroupId &id, const QString &newName)
    {
        CheckGroupExistance(id);
        if (!groups.contains(id))
        {
            return tr("Group does not exist");
        }
        OnGroupRenamed(id, groups[id].displayName, newName);
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::Renamed, newName, groups[id].displayName });
        groups[id].displayName = newName;
        return {};
    }

    bool QvConfigHandler::SetSubscriptionData(const GroupId &id, std::optional<bool> isSubscription, const std::optional<QString> &address,
                                              std::optional<float> updateInterval)
    {
        CheckGroupExistanceEx(id, false);

        if (isSubscription.has_value())
            groups[id].isSubscription = ACCESS_OPTIONAL_VALUE(isSubscription);

        if (address.has_value())
            groups[id].subscriptionOption.address = ACCESS_OPTIONAL_VALUE(address);

        if (updateInterval.has_value())
            groups[id].subscriptionOption.updateInterval = ACCESS_OPTIONAL_VALUE(updateInterval);

        return true;
    }

    bool QvConfigHandler::SetSubscriptionIncludeKeywords(const GroupId &id, const QStringList &Keywords)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups.contains(id))
        {
            return false;
        }
        groups[id].subscriptionOption.IncludeKeywords.clear();

        for (const auto &keyword : Keywords)
        {
            if (keyword != "" && keyword != "\r")
            {
                // Not empty, so we save.
                groups[id].subscriptionOption.IncludeKeywords.push_back(keyword);
            }
        }
        return true;
    }

    bool QvConfigHandler::SetSubscriptionIncludeRelation(const GroupId &id, const QString &Relation)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups.contains(id))
        {
            return false;
        }
        groups[id].subscriptionOption.IncludeRelation = Relation;

        return true;
    }

    bool QvConfigHandler::SetSubscriptionExcludeKeywords(const GroupId &id, const QStringList &Keywords)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups.contains(id))
        {
            return false;
        }
        groups[id].subscriptionOption.ExcludeKeywords.clear();

        for (const auto &keyword : Keywords)
        {
            if (keyword != "" && keyword != "\r")
            {
                // Not empty, so we save.
                groups[id].subscriptionOption.ExcludeKeywords.push_back(keyword);
            }
        }
        return true;
    }

    bool QvConfigHandler::SetSubscriptionExcludeRelation(const GroupId &id, const QString &Relation)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups.contains(id))
        {
            return false;
        }
        groups[id].subscriptionOption.ExcludeRelation = Relation;

        return true;
    }

    bool QvConfigHandler::UpdateSubscription(const GroupId &id)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups[id].isSubscription)
        {
            return false;
        }
        return CHUpdateSubscription_p(id, groups[id].subscriptionOption.address);
    }

    bool QvConfigHandler::CHUpdateSubscription_p(const GroupId &id, const QString &url)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups.contains(id))
        {
            return false;
        }
        //
        // ====================================================================================== Begin reading subscription
        auto _newConnections = GetConnectionConfigFromSubscription(url, GetDisplayName(id));
        if (_newConnections.count() < 5)
        {
            LOG(MODULE_SUBSCRIPTION, "Find a subscription with less than 5 connections.")
            if (QvMessageBoxAsk(
                    nullptr, tr("Update Subscription"),
                    tr("%1 entrie(s) have been found from the subscription source, do you want to continue?").arg(_newConnections.count())) !=
                QMessageBox::Yes)

                return false;
        }
        //
        // ====================================================================================== Begin Connection Data Storage
        // Anyway, we try our best to preserve the connection id.
        QMultiMap<QString, ConnectionId> nameMap;
        QMultiMap<std::tuple<QString, QString, int>, ConnectionId> typeMap;
        {
            // Store connection type metadata into map.
            for (const auto &conn : groups[id].connections)
            {
                nameMap.insert(GetDisplayName(conn), conn);
                const auto [protocol, host, port] = GetConnectionInfo(conn);
                if (port != 0)
                {
                    typeMap.insert({ protocol, host, port }, conn);
                }
            }
        }
        // ====================================================================================== End Connection Data Storage
        //
        bool hasErrorOccured = false;
        // Copy construct here.
        auto originalConnectionIdList = groups[id].connections;
        groups[id].connections.clear();
        //
        int filteredconnections = 0;
        for (const auto &config : _newConnections)
        {
            const auto _alias = _newConnections.key(config);
            QString errMessage;

            if (!errMessage.isEmpty())
            {
                LOG(MODULE_SUBSCRIPTION, "Processing a subscription with following error: " + errMessage)
                hasErrorOccured = true;
                continue;
            }
            bool canGetOutboundData = false;
            // Should not have complex connection we assume.
            auto outboundData = GetConnectionInfo(config, &canGetOutboundData);

            // filter connections
            int i;
            bool includeconfig;
            i = 0;
            if (groups[id].subscriptionOption.IncludeRelation == "And")
            {
                includeconfig = true;
                for (const auto &key : groups[id].subscriptionOption.IncludeKeywords)
                {
                    auto str = key.trimmed();
                    if (!str.isEmpty())
                    {
                        i++;
                        if (_alias.indexOf(str, 0) == -1)
                        {
                            includeconfig = false;
                            break;
                        }
                    }
                }
            }
            else
            { // Or relation
                includeconfig = false;
                for (const auto &key : groups[id].subscriptionOption.IncludeKeywords)
                {
                    auto str = key.trimmed();
                    if (!str.isEmpty())
                    {
                        i++;
                        if (_alias.indexOf(str, 0) != -1)
                        {
                            includeconfig = true;
                            break;
                        }
                    }
                }
            }
            if (i == 0)
                includeconfig = true; // If includekeywords is empty then include all configs.

            if (includeconfig == true)
            {
                i = 0;
                if (groups[id].subscriptionOption.ExcludeRelation == "Or")
                {
                    includeconfig = true;
                    for (const auto &key : groups[id].subscriptionOption.ExcludeKeywords)
                    {
                        auto str = key.trimmed();
                        if (!str.isEmpty())
                        {
                            i++;
                            if (_alias.indexOf(str, 0) != -1)
                            {
                                includeconfig = false;
                                break;
                            }
                        }
                    }
                }
                else
                { // And relation
                    includeconfig = false;
                    for (const auto &key : groups[id].subscriptionOption.ExcludeKeywords)
                    {
                        auto str = key.trimmed();
                        if (!str.isEmpty())
                        {
                            i++;
                            if (_alias.indexOf(str, 0) == -1)
                            {
                                includeconfig = true;
                                break;
                            }
                        }
                    }
                }
                if (i == 0)
                    includeconfig = true; // If excludekeywords is empty then don't exclude any configs.
            }

            if (includeconfig == true)
            {
                filteredconnections++;
                //
                // ====================================================================================== Begin guessing new ConnectionId
                if (nameMap.contains(_alias))
                {
                    // Just go and save the connection...
                    LOG(MODULE_CORE_HANDLER, "Reused connection id from name: " + _alias)
                    auto _conn = nameMap.take(_alias);
                    groups[id].connections << _conn;
                    UpdateConnection(_conn, config, true);
                    // Remove Connection Id from the list.
                    originalConnectionIdList.removeAll(_conn);
                    typeMap.remove(typeMap.key(_conn));
                }
                else if (canGetOutboundData && typeMap.contains(outboundData))
                {
                    LOG(MODULE_CORE_HANDLER, "Reused connection id from protocol/host/port pair for connection: " + _alias)
                    auto _conn = typeMap.take(outboundData);
                    groups[id].connections << _conn;
                    // Update Connection Properties
                    UpdateConnection(_conn, config, true);
                    RenameConnection(_conn, _alias);
                    // Remove Connection Id from the list.
                    originalConnectionIdList.removeAll(_conn);
                    nameMap.remove(nameMap.key(_conn));
                }
                else
                {
                    // New connection id is required since nothing matched found...
                    LOG(MODULE_CORE_HANDLER, "Generated new connection id for connection: " + _alias)
                    CreateConnection(config, _alias, id, true);
                }
                // ====================================================================================== End guessing new ConnectionId
            }
        }
        if (filteredconnections < 5)
        {
            LOG(MODULE_SUBSCRIPTION, "Filtered out less than 5 connections.")
            if (QvMessageBoxAsk(nullptr, tr("Update Subscription"),
                                tr("%1 out of %2 entrie(s) have been filtered out, do you want to continue?")
                                    .arg(filteredconnections)
                                    .arg(_newConnections.count())) != QMessageBox::Yes)
            {
                for (const auto &config : _newConnections)
                {
                    const auto _alias = _newConnections.key(config);
                    QString errMessage;

                    if (!errMessage.isEmpty())
                    {
                        LOG(MODULE_SUBSCRIPTION, "Processing a subscription with following error: " + errMessage)
                        hasErrorOccured = true;
                        continue;
                    }
                    bool canGetOutboundData = false;
                    // Should not have complex connection we assume.
                    auto outboundData = GetConnectionInfo(config, &canGetOutboundData);

                    //
                    // ====================================================================================== Begin guessing new ConnectionId
                    if (nameMap.contains(_alias))
                    {
                        // Just go and save the connection...
                        LOG(MODULE_CORE_HANDLER, "Reused connection id from name: " + _alias)
                        auto _conn = nameMap.take(_alias);
                        groups[id].connections << _conn;
                        UpdateConnection(_conn, config, true);
                        // Remove Connection Id from the list.
                        originalConnectionIdList.removeAll(_conn);
                        typeMap.remove(typeMap.key(_conn));
                    }
                    else if (canGetOutboundData && typeMap.contains(outboundData))
                    {
                        LOG(MODULE_CORE_HANDLER, "Reused connection id from protocol/host/port pair for connection: " + _alias)
                        auto _conn = typeMap.take(outboundData);
                        groups[id].connections << _conn;
                        // Update Connection Properties
                        UpdateConnection(_conn, config, true);
                        RenameConnection(_conn, _alias);
                        // Remove Connection Id from the list.
                        originalConnectionIdList.removeAll(_conn);
                        nameMap.remove(nameMap.key(_conn));
                    }
                    else
                    {
                        // New connection id is required since nothing matched found...
                        LOG(MODULE_CORE_HANDLER, "Generated new connection id for connection: " + _alias)
                        CreateConnection(config, _alias, id, true);
                    }
                    // ====================================================================================== End guessing new ConnectionId
                }
            }
        }

        // Check if anything left behind (not being updated or changed significantly)
        LOG(MODULE_CORE_HANDLER, "Removed old connections not have been matched.")
        for (const auto &conn : originalConnectionIdList)
        {
            LOG(MODULE_CORE_HANDLER, "Removing connections not in the new subscription: " + conn.toString())
            RemoveConnectionFromGroup(conn, id);
        }

        // Update the time
        groups[id].lastUpdatedDate = system_clock::to_time_t(system_clock::now());

        return hasErrorOccured;
    }

    void QvConfigHandler::OnStatsDataArrived_p(const ConnectionGroupPair &id, const quint64 uploadSpeed, const quint64 downloadSpeed)
    {
        if (id.isEmpty())
            return;
        const auto &connectionId = id.connectionId;
        connections[connectionId].upLinkData += uploadSpeed;
        connections[connectionId].downLinkData += downloadSpeed;
        emit OnStatsAvailable(id, uploadSpeed, downloadSpeed,       //
                              connections[connectionId].upLinkData, //
                              connections[connectionId].downLinkData);
        PluginHost->Send_ConnectionStatsEvent({ GetDisplayName(connectionId),         //
                                                uploadSpeed, downloadSpeed,           //
                                                connections[connectionId].upLinkData, //
                                                connections[connectionId].downLinkData });
    }

    const ConnectionGroupPair QvConfigHandler::CreateConnection(const CONFIGROOT &root, const QString &displayName, const GroupId &groupId,
                                                                bool skipSaveConfig)
    {
        LOG(MODULE_CORE_HANDLER, "Creating new connection: " + displayName)
        ConnectionId newId(GenerateUuid());
        groups[groupId].connections << newId;
        connections[newId].creationDate = system_clock::to_time_t(system_clock::now());
        connections[newId].displayName = displayName;
        connections[newId].__qvConnectionRefCount = 1;
        emit OnConnectionCreated({ newId, groupId }, displayName);
        PluginHost->Send_ConnectionEvent({ Events::ConnectionEntry::Created, displayName, "" });
        UpdateConnection(newId, root);
        if (!skipSaveConfig)
        {
            SaveConnectionConfig();
        }
        return { newId, groupId };
    }

} // namespace Qv2ray::core::handlers

#undef CheckIdExistance
#undef CheckGroupExistanceEx
#undef CheckGroupExistance
#undef CheckConnectionExistanceEx
#undef CheckConnectionExistance
