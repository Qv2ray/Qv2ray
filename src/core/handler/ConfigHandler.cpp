#include "ConfigHandler.hpp"

#include "common/QvHelpers.hpp"
#include "core/connection/Serialization.hpp"
#include "core/settings/SettingsBackend.hpp"

namespace Qv2ray::core::handlers
{

    QvConfigHandler::QvConfigHandler()
    {
        DEBUG(MODULE_CORE_HANDLER, "ConnectionHandler Constructor.")

        // Do we need to check how many of them are loaded?
        for (auto i = 0; i < GlobalConfig.connections.count(); i++)
        {
            auto id = ConnectionId(GlobalConfig.connections.keys()[i]);
            connections[id] = GlobalConfig.connections.values()[i];
        }

        for (auto key : GlobalConfig.subscriptions.keys())
        {
            GroupId gkey(key);
            if (gkey == NullGroupId)
            {
                LOG(MODULE_CORE_HANDLER, "Removed a null subscription id")
                continue;
            }
            auto val = GlobalConfig.subscriptions[key];
            groups[gkey] = val;

            for (auto conn : val.connections)
            {
                connections[ConnectionId(conn)].groupId = GroupId(key);
            }
        }

        for (auto key : GlobalConfig.groups.keys())
        {
            GroupId gkey(key);
            if (gkey == NullGroupId)
            {
                LOG(MODULE_CORE_HANDLER, "Removed a null group id")
                continue;
            }
            auto val = GlobalConfig.groups[key];
            groups[gkey] = val;

            for (auto conn : val.connections)
            {
                connections[ConnectionId(conn)].groupId = GroupId(key);
            }
        }

        //
        // Force default group name.
        groups[DefaultGroupId].displayName = tr("Default Group");
        groups[DefaultGroupId].isSubscription = false;
        //

        vCoreInstance = new V2rayKernelInstance();
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessErrored, this, &QvConfigHandler::OnVCoreCrashed);
        connect(vCoreInstance, &V2rayKernelInstance::OnNewStatsDataArrived, this, &QvConfigHandler::OnStatsDataArrived);
        // Directly connected to a signal.
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessOutputReadyRead, this, &QvConfigHandler::OnVCoreLogAvailable);
        //
        //
        tcpingHelper = new QvTCPingHelper(5, this);
        httpHelper = new QvHttpRequestHelper(this);
        connect(tcpingHelper, &QvTCPingHelper::OnLatencyTestCompleted, this, &QvConfigHandler::OnLatencyDataArrived);
        //
        saveTimerId = startTimer(10 * 1000);
        // Do not ping all...
        // pingAllTimerId = startTimer(5 * 60 * 1000);
        pingConnectionTimerId = startTimer(60 * 1000);
    }

    void QvConfigHandler::CHSaveConfigData_p()
    {
        // Copy
        auto newGlobalConfig = GlobalConfig;
        newGlobalConfig.connections.clear();
        newGlobalConfig.groups.clear();
        newGlobalConfig.subscriptions.clear();

        for (auto i = 0; i < connections.count(); i++)
        {
            newGlobalConfig.connections[connections.keys()[i].toString()] = connections.values()[i];
        }

        for (auto i = 0; i < groups.count(); i++)
        {
            QStringList connections = IdListToStrings(groups.values()[i].connections);

            if (groups.values()[i].isSubscription)
            {
                SubscriptionObject_Config o = groups.values()[i];
                o.connections = connections;
                newGlobalConfig.subscriptions[groups.keys()[i].toString()] = o;
            }
            else
            {
                GroupObject_Config o = groups.values()[i];
                o.connections = connections;
                newGlobalConfig.groups[groups.keys()[i].toString()] = o;
            }
        }

        SaveGlobalConfig(newGlobalConfig);
    }

    void QvConfigHandler::timerEvent(QTimerEvent *event)
    {
        if (event->timerId() == saveTimerId)
        {
            CHSaveConfigData_p();
        }
        else if (event->timerId() == pingAllTimerId)
        {
            StartLatencyTest();
        }
        else if (event->timerId() == pingConnectionTimerId)
        {
            if (currentConnectionId != NullConnectionId)
            {
                StartLatencyTest(currentConnectionId);
            }
        }
    }

    void QvConfigHandler::StartLatencyTest()
    {
        for (auto connection : connections.keys())
        {
            StartLatencyTest(connection);
        }
    }

    void QvConfigHandler::StartLatencyTest(const GroupId &id)
    {
        for (auto connection : groups[id].connections)
        {
            StartLatencyTest(connection);
        }
    }

    void QvConfigHandler::StartLatencyTest(const ConnectionId &id)
    {
        emit OnLatencyTestStarted(id);
        tcpingHelper->TestLatency(id);
    }

    const QList<GroupId> QvConfigHandler::Subscriptions() const
    {
        QList<GroupId> subsList;

        for (auto group : groups.keys())
        {
            if (groups[group].isSubscription)
            {
                subsList.push_back(group);
            }
        }

        return subsList;
    }

    const QString QvConfigHandler::GetDisplayName(const ConnectionId &id, int limit) const
    {
        CheckConnectionExistance(id);
        return TruncateString(connections[id].displayName, limit);
    }

    const QString QvConfigHandler::GetDisplayName(const GroupId &id, int limit) const
    {
        CheckGroupExistance(id);
        return TruncateString(groups[id].displayName, limit);
    }

    // Obsolated, Please use:
    // ConnectionId QvConnectionHandler::GetConnectionIdByDisplayName(const QString &displayName, const GroupId &group) const
    //
    // const ConnectionId QvConnectionHandler::GetConnectionIdByDisplayName(const QString &displayName) const
    //{
    //    for (auto conn : connections.keys())
    //    {
    //        if (connections[conn].displayName == displayName)
    //        {
    //            return conn;
    //        }
    //    }
    //
    //    return NullConnectionId;
    //}
    const ConnectionId QvConfigHandler::GetConnectionIdByDisplayName(const QString &displayName, const GroupId &group) const
    {
        CheckGroupExistanceEx(group, NullConnectionId);
        for (auto conn : groups[group].connections)
        {
            if (connections[conn].displayName == displayName)
            {
                return conn;
            }
        }

        return NullConnectionId;
    }
    const GroupId QvConfigHandler::GetGroupIdByDisplayName(const QString &displayName) const
    {
        for (auto group : groups.keys())
        {
            if (groups[group].displayName == displayName)
            {
                return group;
            }
        }

        return NullGroupId;
    }

    const GroupId QvConfigHandler::GetConnectionGroupId(const ConnectionId &id) const
    {
        CheckConnectionExistanceEx(id, NullGroupId);
        if (!connections.contains(id))
        {
            LOG(MODULE_CORE_HANDLER, "Cannot find id: " + id.toString());
        }

        return connections[id].groupId;
    }

    const optional<QString> QvConfigHandler::RenameConnection(const ConnectionId &id, const QString &newName)
    {
        CheckConnectionExistance(id);
        if (!connections.contains(id))
        {
            return tr("Connection doesn't exist");
        }
        OnConnectionRenamed(id, connections[id].displayName, newName);
        connections[id].displayName = newName;
        return {};
    }
    const optional<QString> QvConfigHandler::DeleteConnection(const ConnectionId &id)
    {
        CheckConnectionExistance(id);
        auto groupId = connections[id].groupId;
        QFile connectionFile((groups[groupId].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + groupId.toString() + "/" +
                             id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
        //
        connections.remove(id);
        groups[groupId].connections.removeAll(id);
        emit OnConnectionDeleted(id, groupId);
        //
        bool exists = connectionFile.exists();
        if (exists)
        {
            bool removed = connectionFile.remove();
            if (removed)
            {
                return {};
            }
            return "Failed to remove file";
        }
        return tr("File does not exist.");
    }

    const optional<QString> QvConfigHandler::MoveConnectionGroup(const ConnectionId &id, const GroupId &newGroupId)
    {
        CheckConnectionExistance(id);
        auto const oldgid = connections[id].groupId;
        //
        QString oldPath = (groups[oldgid].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + oldgid.toString() + "/" +
                          id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        //
        auto newDir = (groups[newGroupId].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + newGroupId.toString() + "/";
        QString newPath = newDir + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        //
        if (!QDir(newDir).exists())
        {
            QDir().mkpath(newDir);
        }
        //
        if (!QFile(oldPath).rename(newPath))
        {
            LOG(MODULE_FILEIO, "Cannot rename")
        }
        groups[oldgid].connections.removeAll(id);
        groups[newGroupId].connections.append(id);
        connections[id].groupId = newGroupId;
        //
        emit OnConnectionGroupChanged(id, oldgid, newGroupId);
        //
        return {};
    }

    const optional<QString> QvConfigHandler::DeleteGroup(const GroupId &id)
    {
        CheckGroupExistance(id);
        if (!groups.contains(id) || id == NullGroupId)
        {
            return tr("Group does not exist");
        }

        // Copy construct
        auto list = groups[id].connections;
        for (auto conn : list)
        {
            MoveConnectionGroup(conn, DefaultGroupId);
        }
        //
        if (groups[id].isSubscription)
        {
            QDir(QV2RAY_SUBSCRIPTION_DIR + id.toString()).removeRecursively();
        }
        else
        {
            QDir(QV2RAY_CONNECTIONS_DIR + id.toString()).removeRecursively();
        }
        //
        groups.remove(id);
        emit OnGroupDeleted(id, list);
        if (id == DefaultGroupId)
        {
            groups[id].displayName = tr("Default Group");
        }
        return {};
    }

    const optional<QString> QvConfigHandler::StartConnection(const ConnectionId &id)
    {
        CheckConnectionExistance(id);

        if (currentConnectionId != NullConnectionId)
        {
            StopConnection();
        }

        CONFIGROOT root = GetConnectionRoot(connections[id].groupId, id);
        return CHStartConnection_p(id, root);
    }

    void QvConfigHandler::RestartConnection() // const ConnectionId &id
    {
        auto conn = currentConnectionId;
        if (conn != NullConnectionId)
        {
            StopConnection();
            StartConnection(conn);
        }
    }

    void QvConfigHandler::StopConnection() // const ConnectionId &id
    {
        // Currently just simply stop it.
        //_UNUSED(id)
        // if (currentConnectionId == id) {
        //}
        CHStopConnection_p();
    }

    bool QvConfigHandler::IsConnected(const ConnectionId &id) const
    {
        CheckConnectionExistanceEx(id, false);
        return currentConnectionId == id;
    }

    QvConfigHandler::~QvConfigHandler()
    {
        LOG(MODULE_CORE_HANDLER, "Triggering save settings from destructor")
        CHSaveConfigData_p();

        if (vCoreInstance->KernelStarted)
        {
            vCoreInstance->StopConnection();
            LOG(MODULE_CORE_HANDLER, "Stopped connection from destructor.")
        }

        delete vCoreInstance;
        delete httpHelper;
    }

    const CONFIGROOT QvConfigHandler::GetConnectionRoot(const ConnectionId &id) const
    {
        CheckConnectionExistanceEx(id, CONFIGROOT());
        return connections.contains(id) ? GetConnectionRoot(connections[id].groupId, id) : CONFIGROOT();
    }

    const CONFIGROOT QvConfigHandler::GetConnectionRoot(const GroupId &group, const ConnectionId &id) const
    {
        CheckGroupExistanceEx(group, CONFIGROOT());
        CheckConnectionExistanceEx(id, CONFIGROOT());
        auto path = group.toString() + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        path.prepend(groups[group].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR);
        return CONFIGROOT(JsonFromString(StringFromFile(path)));
    }

    void QvConfigHandler::OnLatencyDataArrived(const QvTCPingResultObject &result)
    {
        if (connections.contains(result.connectionId))
        {
            connections[result.connectionId].latency = result.avg;
            emit OnLatencyTestFinished(result.connectionId, result.avg);
        }
        else
        {
            LOG(MODULE_CORE_HANDLER, "Received a latecy result with non-exist connection id.")
        }
    }

    bool QvConfigHandler::UpdateConnection(const ConnectionId &id, const CONFIGROOT &root)
    {
        CheckConnectionExistanceEx(id, false);
        auto groupId = connections[id].groupId;
        auto path = (groups[groupId].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + groupId.toString() + "/" +
                    id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        auto content = JsonToString(root);
        emit OnConnectionChanged(id);
        return StringToFile(content, path);
    }

    const GroupId QvConfigHandler::CreateGroup(const QString displayName, bool isSubscription)
    {
        GroupId id(GenerateRandomString());
        groups[id].displayName = displayName;
        groups[id].isSubscription = isSubscription;
        groups[id].importDate = system_clock::to_time_t(system_clock::now());
        emit OnGroupCreated(id, displayName);
        return id;
    }

    const optional<QString> QvConfigHandler::RenameGroup(const GroupId &id, const QString &newName)
    {
        CheckGroupExistance(id);
        if (!groups.contains(id))
        {
            return tr("Group does not exist");
        }
        OnGroupRenamed(id, groups[id].displayName, newName);
        groups[id].displayName = newName;
        return {};
    }

    const tuple<QString, int64_t, float> QvConfigHandler::GetSubscriptionData(const GroupId &id) const
    {
        CheckGroupExistance(id);
        tuple<QString, int64_t, float> result;

        if (!groups[id].isSubscription)
        {
            return result;
        }

        return { groups[id].address, groups[id].lastUpdated, groups[id].updateInterval };
    }

    bool QvConfigHandler::SetSubscriptionData(const GroupId &id, const QString &address, float updateInterval)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups.contains(id))
        {
            return false;
        }
        if (!address.isEmpty())
        {
            groups[id].address = address;
        }
        if (updateInterval != -1)
        {
            groups[id].updateInterval = updateInterval;
        }
        return true;
    }

    bool QvConfigHandler::UpdateSubscription(const GroupId &id, bool useSystemProxy)
    {
        CheckGroupExistanceEx(id, false);
        if (isHttpRequestInProgress)
        {
            return false;
        }
        isHttpRequestInProgress = true;
        auto data = httpHelper->syncget(groups[id].address, useSystemProxy);
        isHttpRequestInProgress = false;
        return CHUpdateSubscription_p(id, data);
    }

    bool QvConfigHandler::CHUpdateSubscription_p(const GroupId &id, const QByteArray &subscriptionData)
    {
        CheckGroupExistanceEx(id, false);
        if (!groups.contains(id))
        {
            return false;
        }
        bool isAutoConnectionContainedWithin = groups[id].connections.contains(ConnectionId(GlobalConfig.autoStartId));
        Q_UNUSED(isAutoConnectionContainedWithin)
        //
        // Anyway, we try our best to preserve the connection id.
        QMap<QString, ConnectionId> nameMap;
        QMap<tuple<QString, QString, int>, ConnectionId> typeMap;
        for (auto conn : groups[id].connections)
        {
            nameMap[GetDisplayName(conn)] = conn;
            auto [protocol, host, port] = GetConnectionInfo(conn);
            if (port != 0)
            {
                typeMap[{ protocol, host, port }] = conn;
            }
        }
        //
        // List that is holding connection IDs to be updated.
        // Copy construct here.
        auto connectionsOrig = groups[id].connections;
        groups[id].connections.clear();
        auto str = DecodeSubscriptionString(subscriptionData);
        if (str.isEmpty())
            return false;
        //
        auto subsList = SplitLines(str);
        QDir().mkpath(QV2RAY_SUBSCRIPTION_DIR + id.toString());

        bool hasErrorOccured = false;
        for (auto vmess : subsList)
        {
            QString errMessage;
            QString _alias;
            auto config = ConvertConfigFromString(vmess.trimmed(), &_alias, &errMessage);

            if (!errMessage.isEmpty())
            {
                LOG(MODULE_SUBSCRIPTION, "Processing a subscription with following error: " + errMessage)
                hasErrorOccured = true;
                continue;
            }
            bool canGetOutboundData = false;
            auto outboundData = GetConnectionInfo(config, &canGetOutboundData);
            //
            // Begin guessing new ConnectionId
            if (nameMap.contains(_alias))
            {
                // Just go and save the connection...
                LOG(MODULE_CORE_HANDLER, "Reused connection id from name: " + _alias)
                groups[id].connections << nameMap[_alias];
                UpdateConnection(nameMap[_alias], config);
                // Remove Connection Id from the list.
                connectionsOrig.removeAll(nameMap[_alias]);
            }
            else if (canGetOutboundData && typeMap.contains(outboundData))
            {
                LOG(MODULE_CORE_HANDLER, "Reused connection id from protocol/host/port pair for connection: " + _alias)
                groups[id].connections << typeMap[outboundData];
                UpdateConnection(typeMap[outboundData], config);
                // Update displayName
                connections[typeMap[outboundData]].displayName = _alias;
                // Remove Connection Id from the list.
                connectionsOrig.removeAll(typeMap[outboundData]);
            }
            else
            {
                // New connection id is required since nothing matched found...
                LOG(MODULE_CORE_HANDLER, "Generated new connection id for connection: " + _alias)
                CreateConnection(_alias, id, config);
            }
            // End guessing connectionId
        }

        // Check if anything left behind (not being updated or changed significantly)
        LOG(MODULE_CORE_HANDLER, "Removed old connections not have been matched.")
        for (auto conn : connectionsOrig)
        {
            LOG(MODULE_CORE_HANDLER, "Removing: " + conn.toString())
            DeleteConnection(conn);
        }

        // Update the time
        groups[id].lastUpdated = system_clock::to_time_t(system_clock::now());

        return hasErrorOccured;
    }

    const ConnectionId QvConfigHandler::CreateConnection(const QString &displayName, const GroupId &groupId, const CONFIGROOT &root)
    {
        LOG(MODULE_CORE_HANDLER, "Creating new connection: " + displayName)
        ConnectionId newId(GenerateUuid());
        groups[groupId].connections << newId;
        connections[newId].groupId = groupId;
        connections[newId].importDate = system_clock::to_time_t(system_clock::now());
        connections[newId].displayName = displayName;
        UpdateConnection(newId, root);
        emit OnConnectionCreated(newId, displayName);
        return newId;
    }

} // namespace Qv2ray::core::handlers
