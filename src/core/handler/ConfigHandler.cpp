#include "ConfigHandler.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/connection/Serialization.hpp"
#include "core/handler/RouteHandler.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "utils/HTTPRequestHelper.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "ConfigHandler"

namespace Qv2ray::core::handler
{
    QvConfigHandler::QvConfigHandler(QObject *parent) : QObject(parent)
    {
        DEBUG("ConnectionHandler Constructor.");
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
            auto groupObject = GroupObject::fromJson(groupJson.value(groupId).toObject());
            if (groupObject.displayName.isEmpty())
            {
                groupObject.displayName = tr("Group: %1").arg(GenerateRandomString(5));
            }
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
                        LOG("Failed to remove connection config file");
                }
                connections.remove(id);
                LOG("Dropped connection id: " + id.toString() + " since it's not in a group");
            }
            else
            {
                const auto connectionFilePath = QV2RAY_CONNECTIONS_DIR + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
                connectionRootCache[id] = CONFIGROOT(JsonFromString(StringFromFile(connectionFilePath)));
                DEBUG("Loaded connection id: " + id.toString() + " into cache.");
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
        connect(kernelHandler, &KernelInstanceHandler::OnCrashed, this, &QvConfigHandler::p_OnKernelCrashed);
        connect(kernelHandler, &KernelInstanceHandler::OnStatsDataAvailable, this, &QvConfigHandler::p_OnStatsDataArrived);
        connect(kernelHandler, &KernelInstanceHandler::OnKernelLogAvailable, this, &QvConfigHandler::OnKernelLogAvailable);
        connect(kernelHandler, &KernelInstanceHandler::OnConnected, this, &QvConfigHandler::OnConnected);
        connect(kernelHandler, &KernelInstanceHandler::OnDisconnected, this, &QvConfigHandler::OnDisconnected);
        //
        pingHelper = new LatencyTestHost(5, this);
        connect(pingHelper, &LatencyTestHost::OnLatencyTestCompleted, this, &QvConfigHandler::p_OnLatencyDataArrived);
        //
        // Save per 1 hour.
        saveTimerId = startTimer(1 * 60 * 60 * 1000);
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
            if (!id.isEmpty() && GlobalConfig.advancedConfig.testLatencyPeriodically)
            {
                StartLatencyTest(id.connectionId, GlobalConfig.networkConfig.latencyTestingMethod);
            }
        }
    }

    void QvConfigHandler::StartLatencyTest()
    {
        for (const auto &connection : connections.keys())
        {
            emit OnLatencyTestStarted(connection);
        }
        pingHelper->TestLatency(connections.keys(), GlobalConfig.networkConfig.latencyTestingMethod);
    }

    void QvConfigHandler::StartLatencyTest(const GroupId &id)
    {
        for (const auto &connection : groups[id].connections)
        {
            emit OnLatencyTestStarted(connection);
        }
        pingHelper->TestLatency(groups[id].connections, GlobalConfig.networkConfig.latencyTestingMethod);
    }

    void QvConfigHandler::StartLatencyTest(const ConnectionId &id, Qv2rayLatencyTestingMethod method)
    {
        emit OnLatencyTestStarted(id);
        pingHelper->TestLatency(id, method);
    }

    const QList<GroupId> QvConfigHandler::Subscriptions() const
    {
        QList<GroupId> subsList;

        for (const auto &group : groups)
        {
            if (group.isSubscription)
            {
                subsList.push_back(groups.key(group));
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
        CheckValidId(id.connectionId, nothing);
        connections[id.connectionId].stats.Clear();
        emit OnStatsAvailable(id, {});
        PluginHost->SendEvent({ GetDisplayName(id.connectionId), 0, 0, 0, 0 });
        return;
    }

    const QList<GroupId> QvConfigHandler::GetConnectionContainedIn(const ConnectionId &connId) const
    {
        CheckValidId(connId, {});
        QList<GroupId> grps;
        for (const auto &group : groups)
        {
            if (group.connections.contains(connId))
                grps.push_back(groups.key(group));
        }
        return grps;
    }

    const std::optional<QString> QvConfigHandler::RenameConnection(const ConnectionId &id, const QString &newName)
    {
        CheckValidId(id, {});
        emit OnConnectionRenamed(id, connections[id].displayName, newName);
        PluginHost->SendEvent({ Events::ConnectionEntry::Renamed, newName, connections[id].displayName });
        connections[id].displayName = newName;
        SaveConnectionConfig();
        return {};
    }

    bool QvConfigHandler::RemoveConnectionFromGroup(const ConnectionId &id, const GroupId &gid)
    {
        CheckValidId(id, false);
        LOG("Removing connection : " + id.toString());
        if (groups[gid].connections.contains(id))
        {
            auto removedEntries = groups[gid].connections.removeAll(id);
            if (removedEntries > 1)
            {
                LOG("Found same connection occured multiple times in a group.");
            }
            // Decrease reference count.
            connections[id].__qvConnectionRefCount -= removedEntries;
        }

        if (GlobalConfig.autoStartId == ConnectionGroupPair{ id, gid })
        {
            GlobalConfig.autoStartId.clear();
        }

        // Emit everything first then clear the connection map.
        PluginHost->SendEvent({ Events::ConnectionEntry::RemovedFromGroup, GetDisplayName(id), "" });
        emit OnConnectionRemovedFromGroup({ id, gid });

        if (connections[id].__qvConnectionRefCount <= 0)
        {
            LOG("Fully removing a connection from cache.");
            connectionRootCache.remove(id);
            //
            QFile connectionFile(QV2RAY_CONNECTIONS_DIR + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
            if (connectionFile.exists())
            {
                if (!connectionFile.remove())
                    LOG("Failed to remove connection config file");
            }
            connections.remove(id);
        }
        return true;
    }

    bool QvConfigHandler::LinkConnectionWithGroup(const ConnectionId &id, const GroupId &newGroupId)
    {
        CheckValidId(id, false);
        if (groups[newGroupId].connections.contains(id))
        {
            LOG("Connection not linked since " + id.toString() + " is already in the group " + newGroupId.toString());
            return false;
        }
        groups[newGroupId].connections.append(id);
        connections[id].__qvConnectionRefCount++;
        PluginHost->SendEvent({ Events::ConnectionEntry::LinkedWithGroup, connections[id].displayName, "" });
        emit OnConnectionLinkedWithGroup({ id, newGroupId });
        return true;
    }

    bool QvConfigHandler::MoveConnectionFromToGroup(const ConnectionId &id, const GroupId &sourceGid, const GroupId &targetGid)
    {
        CheckValidId(id, false);
        CheckValidId(targetGid, false);
        CheckValidId(sourceGid, false);
        //
        if (!groups[sourceGid].connections.contains(id))
        {
            LOG("Trying to move a connection away from a group it does not belong to.");
            return false;
        }
        if (groups[targetGid].connections.contains(id))
        {
            LOG("The connection: " + id.toString() + " has already been in the target group: " + targetGid.toString());
            const auto removedCount = groups[sourceGid].connections.removeAll(id);
            connections[id].__qvConnectionRefCount -= removedCount;
        }
        else
        {
            // If the target group does not contain this connection.
            const auto removedCount = groups[sourceGid].connections.removeAll(id);
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
        CheckValidId(id, tr("Group does not exist"));
        // Copy construct
        auto list = groups[id].connections;
        for (const auto &conn : list)
        {
            MoveConnectionFromToGroup(conn, id, DefaultGroupId);
        }

        PluginHost->SendEvent({ Events::ConnectionEntry::FullyRemoved, groups[id].displayName, "" });

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
        CheckValidId(identifier, false);
        connections[identifier.connectionId].lastConnected = system_clock::to_time_t(system_clock::now());
        //
        CONFIGROOT root = GetConnectionRoot(identifier.connectionId);
        const auto fullConfig = RouteManager->GenerateFinalConfig(root, groups[identifier.groupId].routeConfigId);
        //
        auto errMsg = kernelHandler->StartConnection(identifier, fullConfig);
        if (errMsg)
        {
            QvMessageBoxWarn(nullptr, tr("Failed to start connection"), *errMsg);
            return false;
        }

        GlobalConfig.lastConnectedId = identifier;
        return true;
    }

    void QvConfigHandler::RestartConnection()
    {
        StopConnection();
        StartConnection(GlobalConfig.lastConnectedId);
    }

    void QvConfigHandler::StopConnection()
    {
        kernelHandler->StopConnection();
    }

    void QvConfigHandler::p_OnKernelCrashed(const ConnectionGroupPair &id, const QString &errMessage)
    {
        LOG("Kernel crashed: " + errMessage);
        emit OnDisconnected(id);
        PluginHost->SendEvent({ GetDisplayName(id.connectionId), QMap<QString, int>{}, Events::Connectivity::Disconnected });
        emit OnKernelCrashed(id, errMessage);
    }

    QvConfigHandler::~QvConfigHandler()
    {
        LOG("Triggering save settings from destructor");
        delete kernelHandler;
        SaveConnectionConfig();
    }

    const CONFIGROOT QvConfigHandler::GetConnectionRoot(const ConnectionId &id) const
    {
        CheckValidId(id, CONFIGROOT());
        return connectionRootCache.value(id);
    }

    void QvConfigHandler::p_OnLatencyDataArrived(const ConnectionId &id, const LatencyTestResult &result)
    {
        CheckValidId(id, nothing);
        connections[id].latency = result.avg;
        emit OnLatencyTestFinished(id, result.avg);
    }

    bool QvConfigHandler::UpdateConnection(const ConnectionId &id, const CONFIGROOT &root, bool skipRestart)
    {
        CheckValidId(id, false);
        //
        auto path = QV2RAY_CONNECTIONS_DIR + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        auto content = JsonToString(root);
        bool result = StringToFile(content, path);
        //
        connectionRootCache[id] = root;
        //
        emit OnConnectionModified(id);
        PluginHost->SendEvent({ Events::ConnectionEntry::Edited, connections[id].displayName, "" });
        if (!skipRestart && kernelHandler->CurrentConnection().connectionId == id)
        {
            RestartConnection();
        }
        return result;
    }

    const GroupId QvConfigHandler::CreateGroup(const QString &displayName, bool isSubscription)
    {
        GroupId id(GenerateRandomString());
        groups[id].displayName = displayName;
        groups[id].isSubscription = isSubscription;
        groups[id].creationDate = system_clock::to_time_t(system_clock::now());
        PluginHost->SendEvent({ Events::ConnectionEntry::Created, displayName, "" });
        emit OnGroupCreated(id, displayName);
        SaveConnectionConfig();
        return id;
    }

    const GroupRoutingId QvConfigHandler::GetGroupRoutingId(const GroupId &id)
    {
        if (groups[id].routeConfigId == NullRoutingId)
        {
            groups[id].routeConfigId = GroupRoutingId{ GenerateRandomString() };
        }
        return groups[id].routeConfigId;
    }

    const std::optional<QString> QvConfigHandler::RenameGroup(const GroupId &id, const QString &newName)
    {
        CheckValidId(id, tr("Group does not exist"));
        OnGroupRenamed(id, groups[id].displayName, newName);
        PluginHost->SendEvent({ Events::ConnectionEntry::Renamed, newName, groups[id].displayName });
        groups[id].displayName = newName;
        return {};
    }

    bool QvConfigHandler::SetSubscriptionData(const GroupId &id, std::optional<bool> isSubscription, const std::optional<QString> &address,
                                              std::optional<float> updateInterval)
    {
        CheckValidId(id, false);

        if (isSubscription.has_value())
            groups[id].isSubscription = *isSubscription;

        if (address.has_value())
            groups[id].subscriptionOption.address = *address;

        if (updateInterval.has_value())
            groups[id].subscriptionOption.updateInterval = *updateInterval;

        return true;
    }

    bool QvConfigHandler::SetSubscriptionType(const GroupId &id, const QString &type)
    {
        CheckValidId(id, false);
        groups[id].subscriptionOption.type = type;
        return true;
    }

    bool QvConfigHandler::SetSubscriptionIncludeKeywords(const GroupId &id, const QStringList &keywords)
    {
        CheckValidId(id, false);
        groups[id].subscriptionOption.IncludeKeywords.clear();

        for (const auto &keyword : keywords)
        {
            if (!keyword.trimmed().isEmpty())
            {
                groups[id].subscriptionOption.IncludeKeywords.push_back(keyword);
            }
        }
        return true;
    }

    bool QvConfigHandler::SetSubscriptionIncludeRelation(const GroupId &id, SubscriptionFilterRelation relation)
    {
        CheckValidId(id, false);
        groups[id].subscriptionOption.IncludeRelation = relation;
        return true;
    }

    bool QvConfigHandler::SetSubscriptionExcludeKeywords(const GroupId &id, const QStringList &keywords)
    {
        CheckValidId(id, false);
        groups[id].subscriptionOption.ExcludeKeywords.clear();
        for (const auto &keyword : keywords)
        {
            if (!keyword.trimmed().isEmpty())
            {
                groups[id].subscriptionOption.ExcludeKeywords.push_back(keyword);
            }
        }
        return true;
    }

    bool QvConfigHandler::SetSubscriptionExcludeRelation(const GroupId &id, SubscriptionFilterRelation relation)
    {
        CheckValidId(id, false);
        groups[id].subscriptionOption.ExcludeRelation = relation;
        return true;
    }

    void QvConfigHandler::UpdateSubscriptionAsync(const GroupId &id)
    {
        CheckValidId(id, nothing);
        if (!groups[id].isSubscription)
            return;
        NetworkRequestHelper::AsyncHttpGet(groups[id].subscriptionOption.address, [=](const QByteArray &d) {
            p_CHUpdateSubscription(id, d);
            emit OnSubscriptionAsyncUpdateFinished(id);
        });
    }

    bool QvConfigHandler::UpdateSubscription(const GroupId &id)
    {
        if (!groups[id].isSubscription)
            return false;
        const auto data = NetworkRequestHelper::HttpGet(groups[id].subscriptionOption.address);
        return p_CHUpdateSubscription(id, data);
    }

    bool QvConfigHandler::p_CHUpdateSubscription(const GroupId &id, const QByteArray &data)
    {
        CheckValidId(id, false);
        //
        // ====================================================================================== Begin reading subscription
        std::shared_ptr<SubscriptionDecoder> decoder;

        {
            const auto type = groups[id].subscriptionOption.type;
            for (const auto &plugin : PluginHost->UsablePlugins())
            {
                const auto pluginInfo = PluginHost->GetPlugin(plugin);
                if (pluginInfo->hasComponent(COMPONENT_SUBSCRIPTION_ADAPTER))
                {
                    const auto adapterInterface = pluginInfo->pluginInterface->GetSubscriptionAdapter();
                    for (const auto &[t, _] : adapterInterface->SupportedSubscriptionTypes())
                    {
                        if (t == type)
                            decoder = adapterInterface->GetSubscriptionDecoder(t);
                    }
                }
            }

            if (decoder == nullptr)
            {
                QvMessageBoxWarn(nullptr, tr("Cannot Update Subscription"),
                                 tr("Unknown subscription type: %1").arg(type) + NEWLINE + tr("A subscription plugin is missing?"));
                return false;
            }
        }

        const auto groupName = groups[id].displayName;
        const auto result = decoder->DecodeData(data);
        QList<std::pair<QString, CONFIGROOT>> _newConnections;

        for (const auto &[name, json] : result.connections)
        {
            _newConnections.append({ name, CONFIGROOT(json) });
        }
        for (const auto &link : result.links)
        {
            // Assign a group name, to pass the name check.
            QString _alias;
            QString errMessage;
            QString __groupName = groupName;
            const auto connectionConfigMap = ConvertConfigFromString(link.trimmed(), &_alias, &errMessage, &__groupName);
            if (!errMessage.isEmpty())
                LOG("Error: ", errMessage);
            _newConnections << connectionConfigMap;
        }

        if (_newConnections.count() < 5)
        {
            LOG("Found a subscription with less than 5 connections.");
            if (QvMessageBoxAsk(
                    nullptr, tr("Update Subscription"),
                    tr("%n entrie(s) have been found from the subscription source, do you want to continue?", "", _newConnections.count())) != Yes)
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
                const auto &&[protocol, host, port] = GetConnectionInfo(conn);
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
        decltype(_newConnections) filteredConnections;
        //
        for (const auto &config : _newConnections)
        {
            // filter connections
            const bool isIncludeOperationAND = groups[id].subscriptionOption.IncludeRelation == RELATION_AND;
            const bool isExcludeOperationOR = groups[id].subscriptionOption.ExcludeRelation == RELATION_OR;
            //
            // Initial includeConfig value
            bool includeconfig = isIncludeOperationAND;
            {
                bool hasIncludeItemMatched = false;
                for (const auto &key : groups[id].subscriptionOption.IncludeKeywords)
                {
                    if (!key.trimmed().isEmpty())
                    {
                        hasIncludeItemMatched = true;
                        // WARN: MAGIC, DO NOT TOUCH
                        if (!isIncludeOperationAND == config.first.contains(key.trimmed()))
                        {
                            includeconfig = !isIncludeOperationAND;
                            break;
                        }
                    }
                }
                // If includekeywords is empty then include all configs.
                if (!hasIncludeItemMatched)
                    includeconfig = true;
            }
            if (includeconfig)
            {
                bool hasExcludeItemMatched = false;
                includeconfig = isExcludeOperationOR;
                for (const auto &key : groups[id].subscriptionOption.ExcludeKeywords)
                {
                    if (!key.trimmed().isEmpty())
                    {
                        hasExcludeItemMatched = true;
                        // WARN: MAGIC, DO NOT TOUCH
                        if (isExcludeOperationOR == config.first.contains(key.trimmed()))
                        {
                            includeconfig = !isExcludeOperationOR;
                            break;
                        }
                    }
                }
                // If excludekeywords is empty then don't exclude any configs.
                if (!hasExcludeItemMatched)
                    includeconfig = true;
            }

            if (includeconfig)
            {
                filteredConnections << config;
            }
        }

        LOG("Filtered out less than 5 connections.");
        const auto useFilteredConnections =
            filteredConnections.count() > 5 ||
            QvMessageBoxAsk(nullptr, tr("Update Subscription"),
                            tr("%1 out of %n entrie(s) have been filtered out, do you want to continue?", "", _newConnections.count())
                                    .arg(filteredConnections.count()) +
                                NEWLINE + GetDisplayName(id)) == Yes;

        for (const auto &config : useFilteredConnections ? filteredConnections : _newConnections)
        {
            const auto &_alias = config.first;
            // Should not have complex connection we assume.
            bool canGetOutboundData = false;
            const auto &&[protocol, host, port] = GetConnectionInfo(config.second, &canGetOutboundData);
            const auto outboundData = std::make_tuple(protocol, host, port);
            //
            // ====================================================================================== Begin guessing new ConnectionId
            if (nameMap.contains(_alias))
            {
                // Just go and save the connection...
                LOG("Reused connection id from name: " + _alias);
                const auto _conn = nameMap.take(_alias);
                groups[id].connections << _conn;
                UpdateConnection(_conn, config.second, true);
                // Remove Connection Id from the list.
                originalConnectionIdList.removeAll(_conn);
                typeMap.remove(typeMap.key(_conn));
            }
            else if (canGetOutboundData && typeMap.contains(outboundData))
            {
                LOG("Reused connection id from protocol/host/port pair for connection: " + _alias);
                const auto _conn = typeMap.take(outboundData);
                groups[id].connections << _conn;
                // Update Connection Properties
                UpdateConnection(_conn, config.second, true);
                RenameConnection(_conn, _alias);
                // Remove Connection Id from the list.
                originalConnectionIdList.removeAll(_conn);
                nameMap.remove(nameMap.key(_conn));
            }
            else
            {
                // New connection id is required since nothing matched found...
                LOG("Generated new connection id for connection: " + _alias);
                CreateConnection(config.second, _alias, id, true);
            }
            // ====================================================================================== End guessing new ConnectionId
        }

        // Check if anything left behind (not being updated or changed significantly)
        if (!originalConnectionIdList.isEmpty())
        {
            bool needContinue = QvMessageBoxAsk(nullptr, //
                                                tr("Update Subscription"),
                                                tr("There're %n connection(s) in the group that do not belong the current subscription (any more).",
                                                   "", originalConnectionIdList.count()) +
                                                    NEWLINE + GetDisplayName(id) + NEWLINE + tr("Would you like to remove them?")) == Yes;
            if (needContinue)
            {
                LOG("Removed old connections not have been matched.");
                for (const auto &conn : originalConnectionIdList)
                {
                    LOG("Removing connections not in the new subscription: " + conn.toString());
                    RemoveConnectionFromGroup(conn, id);
                }
            }
        }

        // Update the time
        groups[id].lastUpdatedDate = system_clock::to_time_t(system_clock::now());
        return hasErrorOccured;
    }

    void QvConfigHandler::p_OnStatsDataArrived(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeed> &data)
    {
        if (id.isEmpty())
            return;

        const auto &cid = id.connectionId;
        QMap<StatisticsType, QvStatsSpeedData> result;
        for (const auto t : data.keys())
        {
            const auto &stat = data[t];
            connections[cid].stats[t].upLinkData += stat.first;
            connections[cid].stats[t].downLinkData += stat.second;
            result[t] = { stat, connections[cid].stats[t].toData() };
        }

        emit OnStatsAvailable(id, result);
        PluginHost->SendEvent({ GetDisplayName(cid),                     //
                                result[CurrentStatAPIType].first.first,  //
                                result[CurrentStatAPIType].first.second, //
                                result[CurrentStatAPIType].second.first, //
                                result[CurrentStatAPIType].second.second });
    }
    const ConnectionGroupPair QvConfigHandler::CreateConnection(const CONFIGROOT &root, const QString &displayName, const GroupId &groupId,
                                                                bool skipSaveConfig)
    {
        LOG("Creating new connection: " + displayName);
        ConnectionId newId(GenerateUuid());
        groups[groupId].connections << newId;
        connections[newId].creationDate = system_clock::to_time_t(system_clock::now());
        connections[newId].displayName = displayName;
        connections[newId].__qvConnectionRefCount = 1;
        emit OnConnectionCreated({ newId, groupId }, displayName);
        PluginHost->SendEvent({ Events::ConnectionEntry::Created, displayName, "" });
        UpdateConnection(newId, root);
        if (!skipSaveConfig)
        {
            SaveConnectionConfig();
        }
        return { newId, groupId };
    }

} // namespace Qv2ray::core::handler

#undef CheckIdExistance
#undef CheckGroupExistanceEx
#undef CheckGroupExistance
#undef CheckConnectionExistanceEx
#undef CheckConnectionExistance
