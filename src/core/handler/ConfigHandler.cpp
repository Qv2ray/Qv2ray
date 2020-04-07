#include "ConfigHandler.hpp"

#include "common/QvHelpers.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "core/connection/Serialization.hpp"
#include "core/settings/SettingsBackend.hpp"

namespace Qv2ray::core::handlers
{

    QvConfigHandler::QvConfigHandler()
    {
        DEBUG(MODULE_CORE_HANDLER, "ConnectionHandler Constructor.")

        // Do we need to check how many of them are loaded?
        // Do not use: for (const auto &key : connections)
        for (auto i = 0; i < GlobalConfig.connections.count(); i++)
        {
            auto const &id = ConnectionId(GlobalConfig.connections.keys().at(i));
            connections[id] = GlobalConfig.connections.values().at(i);
        }

        for (const auto &key : GlobalConfig.subscriptions.keys())
        {
            GroupId gkey(key);
            if (gkey == NullGroupId)
            {
                LOG(MODULE_CORE_HANDLER, "Removed a null subscription id")
                continue;
            }
            auto const &val = GlobalConfig.subscriptions[key];
            groups[gkey] = val;

            for (auto conn : val.connections)
            {
                connections[ConnectionId(conn)].groupId = GroupId(key);
            }
        }

        for (const auto &key : GlobalConfig.groups.keys())
        {
            GroupId gkey(key);
            if (gkey == NullGroupId)
            {
                LOG(MODULE_CORE_HANDLER, "Removed a null group id")
                continue;
            }
            auto const &val = GlobalConfig.groups.value(key);
            groups[gkey] = val;

            for (auto conn : val.connections)
            {
                connections[ConnectionId(conn)].groupId = GroupId(key);
            }
        }

        for (const auto &id : connections.keys())
        {
            DEBUG(MODULE_CORE_HANDLER, "Loading connection: " + connections.value(id).displayName + " to cache.")
            auto const &group = connections.value(id).groupId;
            if (group != NullGroupId)
            {
                auto path = group.toString() + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
                path.prepend(groups[group].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR);
                //
                connectionRootCache[id] = CONFIGROOT(JsonFromString(StringFromFile(path)));
            }
            else
            {
                connections.remove(id);
                LOG(MODULE_CORE_HANDLER, "Dropped connection id: " + id.toString() + " since it's not in a group")
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
        tcpingHelper = new QvTCPingHelper(5, this);
        httpHelper = new QvHttpRequestHelper(this);
        connect(tcpingHelper, &QvTCPingHelper::OnLatencyTestCompleted, this, &QvConfigHandler::OnLatencyDataArrived);
        //
        // Save per 2 minutes.
        saveTimerId = startTimer(2 * 60 * 1000);
        // Do not ping all...
        // pingAllTimerId = startTimer(5 * 60 * 1000);
        pingConnectionTimerId = startTimer(60 * 1000);
    }

    void QvConfigHandler::CHSaveConfigData_p()
    {
        // Do not copy construct.
        auto &newGlobalConfig = GlobalConfig;
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

        SaveGlobalSettings(newGlobalConfig);
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

    const optional<QString> QvConfigHandler::ClearConnectionUsage(const ConnectionId &id)
    {
        CheckConnectionExistance(id);
        connections[id].upLinkData = 0;
        connections[id].downLinkData = 0;
        emit OnStatsAvailable(id, 0, 0, 0, 0);
        PluginHost->Send_ConnectionStatsEvent({ GetDisplayName(currentConnectionId), 0, 0, 0, 0 });
        return {};
    }

    const optional<QString> QvConfigHandler::RenameConnection(const ConnectionId &id, const QString &newName)
    {
        CheckConnectionExistance(id);
        OnConnectionRenamed(id, connections[id].displayName, newName);
        PluginHost->Send_ConnectionEvent({ newName, connections[id].displayName, ConnectionEvent_Renamed });
        connections[id].displayName = newName;
        CHSaveConfigData_p();
        return {};
    }
    const optional<QString> QvConfigHandler::DeleteConnection(const ConnectionId &id)
    {
        CheckConnectionExistance(id);
        auto groupId = connections[id].groupId;
        QFile connectionFile((groups[groupId].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + groupId.toString() + "/" +
                             id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
        //
        PluginHost->Send_ConnectionEvent({ connections[id].displayName, "", ConnectionEvent_Deleted });
        connections.remove(id);
        groups[groupId].connections.removeAll(id);
        //
        if (GlobalConfig.autoStartId == id.toString())
        {
            GlobalConfig.autoStartId.clear();
        }
        //
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
        PluginHost->Send_ConnectionEvent({ connections[id].displayName, "", ConnectionEvent_Updated });
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
        PluginHost->Send_ConnectionEvent({ groups[id].displayName, "", ConnectionEvent_Deleted });
        //
        groups.remove(id);
        CHSaveConfigData_p();
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

        CONFIGROOT root = GetConnectionRoot(id);
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
        CHSaveConfigData_p();
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
        return connectionRootCache.value(id);
    }

    void QvConfigHandler::OnLatencyDataArrived(const QvTCPingResultObject &result)
    {
        CheckConnectionExistanceEx(result.connectionId, nothing);
        connections[result.connectionId].latency = result.avg;
        emit OnLatencyTestFinished(result.connectionId, result.avg);
    }

    bool QvConfigHandler::UpdateConnection(const ConnectionId &id, const CONFIGROOT &root, bool skipRestart)
    {
        CheckConnectionExistanceEx(id, false);
        auto const &groupId = connections[id].groupId;
        CheckGroupExistanceEx(groupId, false);
        //
        auto path = (groups[groupId].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + groupId.toString() + "/" +
                    id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        auto content = JsonToString(root);
        bool result = StringToFile(content, path);
        //
        connectionRootCache[id] = root;
        //
        emit OnConnectionModified(id);
        PluginHost->Send_ConnectionEvent({ connections[id].displayName, "", ConnectionEvent_Updated });
        if (!skipRestart && id == currentConnectionId)
        {
            emit RestartConnection();
        }
        return result;
    }

    const GroupId QvConfigHandler::CreateGroup(const QString displayName, bool isSubscription)
    {
        GroupId id(GenerateRandomString());
        groups[id].displayName = displayName;
        groups[id].isSubscription = isSubscription;
        groups[id].importDate = system_clock::to_time_t(system_clock::now());
        PluginHost->Send_ConnectionEvent({ displayName, "", ConnectionEvent_Created });
        emit OnGroupCreated(id, displayName);
        CHSaveConfigData_p();
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
        PluginHost->Send_ConnectionEvent({ newName, groups[id].displayName, ConnectionEvent_Renamed });
        groups[id].displayName = newName;
        return {};
    }

    const tuple<QString, int64_t, float> QvConfigHandler::GetSubscriptionData(const GroupId &id) const
    {
        CheckGroupExistanceEx(id, {});
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
        // List that is holding connection IDs to be updated.
        auto subsList = SplitLines(DecodeSubscriptionString(subscriptionData));
        //
        if (subsList.count() < 5)
        {
            auto yes = QvMessageBoxAsk(
                           nullptr, tr("Update Subscription"),
                           tr("%1 entrie(s) have been found from the subscription source, do you want to continue?").arg(subsList.count())) ==
                       QMessageBox::Yes;
            if (!yes)
            {
                return false;
            }
        }
        // Anyway, we try our best to preserve the connection id.
        QMultiMap<QString, ConnectionId> nameMap;
        QMultiMap<tuple<QString, QString, int>, ConnectionId> typeMap;
        for (const auto conn : groups[id].connections)
        {
            nameMap.insertMulti(GetDisplayName(conn), conn);
            auto [protocol, host, port] = GetConnectionInfo(conn);
            if (port != 0)
            {
                typeMap.insertMulti({ protocol, host, port }, conn);
            }
        }
        QDir().mkpath(QV2RAY_SUBSCRIPTION_DIR + id.toString());
        bool hasErrorOccured = false;
        // Copy construct here.
        auto connectionsOrig = groups[id].connections;
        groups[id].connections.clear();
        //
        for (auto vmess : subsList)
        {
            QString errMessage;
            auto ssdGroupName = GetDisplayName(id);
            QString __alias;
            auto conf = ConvertConfigFromString(vmess.trimmed(), &__alias, &errMessage, &ssdGroupName);
            Q_UNUSED(ssdGroupName)
            // Things may go wrong when updating a subscription with ssd:// link
            for (auto _alias : conf.keys())
            {
                for (const auto config : conf.values(_alias))
                {
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
                    // Begin guessing new ConnectionId
                    if (nameMap.contains(_alias))
                    {
                        // Just go and save the connection...
                        LOG(MODULE_CORE_HANDLER, "Reused connection id from name: " + _alias)
                        auto _conn = nameMap.take(_alias);
                        groups[id].connections << _conn;
                        UpdateConnection(_conn, config);
                        // Remove Connection Id from the list.
                        connectionsOrig.removeAll(_conn);
                        typeMap.remove(typeMap.key(_conn));
                    }
                    else if (canGetOutboundData && typeMap.contains(outboundData))
                    {
                        LOG(MODULE_CORE_HANDLER, "Reused connection id from protocol/host/port pair for connection: " + _alias)
                        auto _conn = typeMap.take(outboundData);
                        groups[id].connections << _conn;
                        // Update Connection Properties
                        UpdateConnection(_conn, config);
                        RenameConnection(_conn, _alias);
                        // Remove Connection Id from the list.
                        connectionsOrig.removeAll(_conn);
                        nameMap.remove(nameMap.key(_conn));
                    }
                    else
                    {
                        // New connection id is required since nothing matched found...
                        LOG(MODULE_CORE_HANDLER, "Generated new connection id for connection: " + _alias)
                        CreateConnection(_alias, id, config);
                    }
                    // End guessing connectionId
                }
            }
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
        emit OnConnectionCreated(newId, displayName);
        PluginHost->Send_ConnectionEvent({ displayName, "", ConnectionEvent_Created });
        UpdateConnection(newId, root);
        CHSaveConfigData_p();
        return newId;
    }

} // namespace Qv2ray::core::handlers
