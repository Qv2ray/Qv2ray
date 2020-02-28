#include "ConnectionHandler.hpp"

#include "common/QvHelpers.hpp"
#include "core/config/ConfigBackend.hpp"
#include "core/connection/Serialization.hpp"

namespace Qv2ray::core::handlers
{

    QvConnectionHandler::QvConnectionHandler()
    {
        DEBUG(MODULE_CORE_HANDLER, "ConnectionHandler Constructor.")

        // Do we need to check how many of them are loaded?
        for (auto i = 0; i < GlobalConfig.connections.count(); i++)
        { connections[ConnectionId(GlobalConfig.connections.keys()[i])] = GlobalConfig.connections.values()[i]; }

        for (auto key : GlobalConfig.subscriptions.keys())
        {
            auto val = GlobalConfig.subscriptions[key];
            groups[GroupId(key)] = val;

            for (auto conn : val.connections) { connections[ConnectionId(conn)].groupId = GroupId(key); }
        }

        for (auto key : GlobalConfig.groups.keys())
        {
            auto val = GlobalConfig.groups[key];
            groups[GroupId(key)] = val;

            for (auto conn : val.connections) { connections[ConnectionId(conn)].groupId = GroupId(key); }
        }

        vCoreInstance = new V2rayKernelInstance();
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessErrored, this, &QvConnectionHandler::OnVCoreCrashed);
        connect(vCoreInstance, &V2rayKernelInstance::OnNewStatsDataArrived, this, &QvConnectionHandler::OnStatsDataArrived);
        // Directly connected to a signal.
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessOutputReadyRead, this, &QvConnectionHandler::OnVCoreLogAvailable);
        //
        //
        tcpingHelper = new QvTCPingHelper(5, this);
        connect(tcpingHelper, &QvTCPingHelper::OnLatencyTestCompleted, this, &QvConnectionHandler::OnLatencyDataArrived);
        //
        saveTimerId = startTimer(10 * 1000);
        pingAllTimerId = startTimer(5 * 60 * 1000);
        pingConnectionTimerId = startTimer(60 * 1000);
    }

    void QvConnectionHandler::CHSaveConfigData_p()
    {
        // Copy
        auto newGlobalConfig = GlobalConfig;
        newGlobalConfig.connections.clear();
        newGlobalConfig.groups.clear();
        newGlobalConfig.subscriptions.clear();

        for (auto i = 0; i < connections.count(); i++)
        { newGlobalConfig.connections[connections.keys()[i].toString()] = connections.values()[i]; }

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

    void QvConnectionHandler::timerEvent(QTimerEvent *event)
    {
        if (event->timerId() == saveTimerId) { CHSaveConfigData_p(); }
        else if (event->timerId() == pingAllTimerId)
        {
            StartLatencyTest();
        }
        else if (event->timerId() == pingConnectionTimerId)
        {
            if (currentConnectionId != NullConnectionId) { StartLatencyTest(currentConnectionId); }
        }
    }

    void QvConnectionHandler::StartLatencyTest()
    {
        for (auto connection : connections.keys()) { StartLatencyTest(connection); }
    }

    void QvConnectionHandler::StartLatencyTest(const GroupId &id)
    {
        for (auto connection : groups[id].connections) { StartLatencyTest(connection); }
    }

    void QvConnectionHandler::StartLatencyTest(const ConnectionId &id)
    {
        emit OnLatencyTestStarted(id);
        tcpingHelper->TestLatency(id);
    }

    const QList<ConnectionId> QvConnectionHandler::Connections() const
    {
        return connections.keys();
    }

    const QList<GroupId> QvConnectionHandler::AllGroups() const
    {
        return groups.keys();
    }

    const QList<GroupId> QvConnectionHandler::Subscriptions() const
    {
        QList<GroupId> subsList;

        for (auto group : groups.keys())
        {
            if (groups[group].isSubscription) { subsList.push_back(group); }
        }

        return subsList;
    }

    const QList<ConnectionId> QvConnectionHandler::Connections(const GroupId &groupId) const
    {
        return groups[groupId].connections;
    }

    const QString QvConnectionHandler::GetDisplayName(const ConnectionId &id, int limit) const
    {
        return TruncateString(connections[id].displayName, limit);
    }

    const QString QvConnectionHandler::GetDisplayName(const GroupId &id, int limit) const
    {
        return TruncateString(groups[id].displayName, limit);
    }

    const ConnectionId QvConnectionHandler::GetConnectionIdByDisplayName(const QString &displayName) const
    {
        for (auto conn : connections.keys())
        {
            if (connections[conn].displayName == displayName) { return conn; }
        }

        return NullConnectionId;
    }

    const GroupId QvConnectionHandler::GetGroupIdByDisplayName(const QString &displayName) const
    {
        for (auto group : groups.keys())
        {
            if (groups[group].displayName == displayName) { return group; }
        }

        return NullGroupId;
    }

    const GroupId QvConnectionHandler::GetConnectionGroupId(const ConnectionId &id) const
    {
        if (!connections.contains(id)) { LOG(MODULE_CORE_HANDLER, "Cannot find id: " + id.toString()); }

        return connections[id].groupId;
    }

    double QvConnectionHandler::GetConnectionLatency(const ConnectionId &id) const
    {
        if (!connections.contains(id)) { LOG(MODULE_CORE_HANDLER, "Cannot find id: " + id.toString()); }

        return connections[id].latency;
    }

    const optional<QString> QvConnectionHandler::DeleteConnection(const ConnectionId &id)
    {
        //
        auto groupId = connections[id].groupId;
        QFile connectionFile((groups[groupId].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + groupId.toString() + "/" +
                             id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
        //
        bool exists = connectionFile.exists();
        if (exists)
        {
            bool removed = connectionFile.remove();
            if (removed)
            {
                connections.remove(id);
                groups[groupId].connections.removeAll(id);
                return {};
            }
            else
                return "Failed to remove file";
        }
        else
            return tr("File does not exist.");
    }

    const optional<QString> QvConnectionHandler::StartConnection(const ConnectionId &identifier)
    {
        if (!connections.contains(identifier)) { return tr("No connection selected!") + NEWLINE + tr("Please select a config from the list."); }

        if (currentConnectionId != NullConnectionId) { StopConnection(); }

        CONFIGROOT root = GetConnectionRoot(connections[identifier].groupId, identifier);
        return CHStartConnection_p(identifier, root);
    }

    void QvConnectionHandler::StopConnection() // const ConnectionId &id
    {
        // Currently just simply stop it.
        //_UNUSED(id)
        // if (currentConnectionId == id) {
        //}
        CHStopConnection_p();
    }

    bool QvConnectionHandler::IsConnected(const ConnectionId &id) const
    {
        return currentConnectionId == id;
    }

    const QString QvConnectionHandler::GetConnectionProtocolString(const ConnectionId &id) const
    {
        QString result;

        if (!connections.contains(id)) { result = tr("N/A"); }

        CONFIGROOT root = GetConnectionRoot(connections[id].groupId, id);
        QStringList protocols;
        QStringList streamProtocols;
        auto outbound = root["outbounds"].toArray().first().toObject();
        result.append(outbound["protocol"].toString());

        if (outbound.contains("streamSettings"))
        {
            result.append(" / " + outbound["streamSettings"].toObject()["network"].toString());

            if (outbound["streamSettings"].toObject().contains("tls"))
            { result.append(outbound["streamSettings"].toObject()["tls"].toBool() ? " / tls" : ""); }
        }

        return result;
    }

    const tuple<quint64, quint64> QvConnectionHandler::GetConnectionUsageAmount(const ConnectionId &id) const
    {
        if (!connections.contains(id)) { return make_tuple(0, 0); }

        return make_tuple(connections[id].upLinkData, connections[id].downLinkData);
    }

    // const GroupMetaObject QvConnectionHandler::GetGroup(const GroupId &id)
    // const
    //{
    //    return groups[id];
    //}

    QvConnectionHandler::~QvConnectionHandler()
    {
        CHSaveConfigData_p();

        if (vCoreInstance->KernelStarted)
        {
            vCoreInstance->StopConnection();
            LOG(MODULE_CORE_HANDLER, "Stopped connection from destructor.")
        }

        delete vCoreInstance;
    }

    const CONFIGROOT QvConnectionHandler::GetConnectionRoot(const ConnectionId &id) const
    {
        return connections.contains(id) ? GetConnectionRoot(connections[id].groupId, id) : CONFIGROOT();
    }

    const CONFIGROOT QvConnectionHandler::GetConnectionRoot(const GroupId &group, const ConnectionId &id) const
    {
        auto path = group.toString() + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        path.prepend(groups[group].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR);
        return CONFIGROOT(JsonFromString(StringFromFile(path)));
    }

    const tuple<QString, QString, int> QvConnectionHandler::GetConnectionData(const ConnectionId &id) const
    {
        // TODO, what if is complex?
        auto root = GetConnectionRoot(id);

        bool isSucceed = false;
        auto result = CHGetOutboundData_p(root, &isSucceed);
        return result;
    }

    const tuple<QString, QString, int> QvConnectionHandler::CHGetOutboundData_p(const CONFIGROOT &root, bool *ok) const
    {
        *ok = false;

        for (auto item : root["outbounds"].toArray())
        {
            OUTBOUND outBoundRoot = OUTBOUND(item.toObject());
            QString host;
            int port;
            QString outboundType = "";

            if (GetOutboundData(outBoundRoot, &host, &port, &outboundType))
            {
                *ok = true;
                return make_tuple(outboundType, host, port);
            }
            else
            {
                LOG(MODULE_CORE_HANDLER, "Unknown outbound type: " + outboundType + ", cannot deduce host and port.")
            }
        }
        return make_tuple(tr("N/A"), tr("N/A"), 0);
    }

    void QvConnectionHandler::OnLatencyDataArrived(const QvTCPingResultObject &result)
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

    bool QvConnectionHandler::UpdateConnection(const ConnectionId &id, const CONFIGROOT &root)
    {
        auto groupId = connections[id].groupId;
        auto path = (groups[groupId].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR) + groupId.toString() + "/" +
                    id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        auto content = JsonToString(root);
        emit OnConnectionChanged(id);
        return StringToFile(content, path);
    }

    const tuple<QString, int64_t, float> QvConnectionHandler::GetSubscriptionData(const GroupId &id)
    {
        tuple<QString, int64_t, float> result;

        if (!groups[id].isSubscription) { return result; }

        return make_tuple(groups[id].address, groups[id].lastUpdated, groups[id].updateInterval);
    }

    bool QvConnectionHandler::UpdateSubscription(const GroupId &id, bool useSystemProxy)
    {
        if (isHttpRequestInProgress) { return false; }
        isHttpRequestInProgress = true;
        auto data = httpHelper->syncget(groups[id].address, useSystemProxy);
        isHttpRequestInProgress = false;
        return CHUpdateSubscription_p(id, data);
    }

    bool QvConnectionHandler::CHUpdateSubscription_p(const GroupId &id, const QByteArray &subscriptionData)
    {
        if (!groups.contains(id)) { return false; }
        bool isAutoConnectionContainedWithin = groups[id].connections.contains(ConnectionId(GlobalConfig.autoStartId));
        Q_UNUSED(isAutoConnectionContainedWithin)
        //
        // Anyway, we try our best to preserve the connection id.
        QMap<QString, ConnectionId> nameMap;
        QMap<tuple<QString, QString, int>, ConnectionId> typeMap;
        for (auto conn : groups[id].connections)
        {
            nameMap[GetDisplayName(conn)] = conn;
            auto [protocol, host, port] = GetConnectionData(conn);
            if (port != 0) { typeMap[make_tuple(protocol, host, port)] = conn; }
        }
        //
        /// List that is holding connection IDs to be updated.
        auto connectionsOrig = groups[id].connections;
        auto str = DecodeSubscriptionString(subscriptionData);
        if (str.isEmpty()) return false;
        //
        auto subsList = SplitLines(str);
        QDir(QV2RAY_SUBSCRIPTION_DIR + id.toString()).removeRecursively();
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
            auto outboundData = CHGetOutboundData_p(config, &canGetOutboundData);
            //
            // Begin guessing new ConnectionId
            if (nameMap.contains(_alias))
            {
                // Just go and save the connection...
                LOG(MODULE_CORE_HANDLER, "Guessed id from name: " + _alias + ", connectionId: " + nameMap[_alias].toString())
                UpdateConnection(nameMap[_alias], config);
                // Remove Connection Id from the list.
                connectionsOrig.removeAll(nameMap[_alias]);
            }
            else if (canGetOutboundData && typeMap.contains(outboundData))
            {
                LOG(MODULE_CORE_HANDLER, "Guessed id from protocol/host/port pair for connectionId: " + typeMap[outboundData].toString())
                UpdateConnection(typeMap[outboundData], config);
                // Update displayName
                connections[typeMap[outboundData]].displayName = _alias;
                // Remove Connection Id from the list.
                connectionsOrig.removeAll(typeMap[outboundData]);
            }
            else
            {
                // New connection id is required since nothing matched found...
                ConnectionId newId(GenerateUuid());
                connections[newId].groupId = id;
                connections[newId].importDate = system_clock::to_time_t(system_clock::now());
                connections[newId].displayName = _alias;
                LOG(MODULE_CORE_HANDLER, "Generated new connectionId.")
                UpdateConnection(newId, config);
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

        return hasErrorOccured;
    }

} // namespace Qv2ray::core::handlers
