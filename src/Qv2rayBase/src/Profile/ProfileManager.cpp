//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Qv2rayBase/Profile/ProfileManager.hpp"

#include "Qv2rayBase/Common/HTTPRequestHelper.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Interfaces/IStorageProvider.hpp"
#include "Qv2rayBase/Plugin/LatencyTestHost.hpp"
#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/private/Profile/ProfileManager_p.hpp"

#include <QNetworkReply>
#include <QTimerEvent>

#if QT_CONFIG(concurrent)
#include <QtConcurrent/QtConcurrent>
#endif

#define CheckValidId(id, returnValue)                                                                                                                                    \
    do                                                                                                                                                                   \
    {                                                                                                                                                                    \
        if (!IsValidId(id))                                                                                                                                              \
            return returnValue;                                                                                                                                          \
    } while (0)

#define nothing

namespace Qv2rayBase::Profile
{
    using namespace Qv2rayPlugin::Event;
    using namespace Qv2rayBase::Utils;

    ProfileManager::ProfileManager(QObject *parent) : QObject(parent)
    {
        d_ptr.reset(new ProfileManagerPrivate);
        Q_D(ProfileManager);
        qDebug() << "ProfileManager Constructor.";

        connect(Qv2rayBaseLibrary::LatencyTestHost(), &Qv2rayBase::Plugin::LatencyTestHost::OnLatencyTestCompleted, this, &ProfileManager::p_OnLatencyDataArrived);
        connect(Qv2rayBaseLibrary::KernelManager(), &Qv2rayBase::Profile::KernelManager::OnStatsDataAvailable, this, &ProfileManager::p_OnStatsDataArrived);

        d->connections = Qv2rayBaseLibrary::StorageProvider()->GetConnections();
        const auto _groups = Qv2rayBaseLibrary::StorageProvider()->GetGroups();
        const auto _routings = Qv2rayBaseLibrary::StorageProvider()->GetRoutings();

        for (auto it = _groups.constKeyValueBegin(); it != _groups.constKeyValueEnd(); it++)
        {
            auto id = it->first;
            auto grp = it->second;
            if (grp.name.isEmpty())
                grp.name = tr("Group: %1").arg(GenerateRandomString(5));

            d->groups.insert(id, grp);

            for (const auto &connId : grp.connections)
            {
                d->connections[connId]._group_ref++;
            }
        }

        for (auto it = _routings.constKeyValueBegin(); it != _routings.constKeyValueEnd(); it++)
        {
            d->routings.insert(it->first, it->second);
        }

        for (auto it = d->connections.constKeyValueBegin(); it != d->connections.constKeyValueEnd(); it++)
        {
            auto id = it->first;
            auto conn = it->second;
            if (d->connections[id]._group_ref == 0)
            {
                d->connections.remove(id);
                Qv2rayBaseLibrary::StorageProvider()->DeleteConnection(id);
                qInfo() << "Dropped connection id:" << id << "since it's not in a group";
            }
            else
            {
                d->connectionRootCache[id] = Qv2rayBaseLibrary::StorageProvider()->GetConnectionContent(id);
                qDebug() << "Loaded connection id:" << id << "into cache.";
            }
        }

        // Force default group name.
        if (!d->groups.contains(DefaultGroupId))
        {
            d->groups.insert(DefaultGroupId, GroupObject{});
            d->groups[DefaultGroupId].name = tr("Default Group");
        }
    }

    ProfileManager::~ProfileManager()
    {
        SaveConnectionConfig();
    }

    void ProfileManager::SaveConnectionConfig()
    {
        Q_D(ProfileManager);
        Qv2rayBaseLibrary::StorageProvider()->StoreConnections(d->connections);
        Qv2rayBaseLibrary::StorageProvider()->StoreGroups(d->groups);
        Qv2rayBaseLibrary::StorageProvider()->StoreRoutings(d->routings);
        Qv2rayBaseLibrary::StorageProvider()->EnsureSaved();
    }

    void ProfileManager::StartLatencyTest(const GroupId &id, const LatencyTestEngineId &engine)
    {
        Q_D(ProfileManager);
        if (engine.isNull())
        {
            Qv2rayBaseLibrary::Warn(tr("Invalid Latency Test Engine"), tr("Latency test engine ID is null"));
            return;
        }
        for (const auto &connection : d->groups.value(id).connections)
            StartLatencyTest(connection, engine);
    }

    void ProfileManager::StartLatencyTest(const ConnectionId &id, const LatencyTestEngineId &engine)
    {
        if (engine.isNull())
        {
            Qv2rayBaseLibrary::Warn(tr("Invalid Latency Test Engine"), tr("Latency test engine ID is null"));
            return;
        }
        emit OnLatencyTestStarted(id);
        Qv2rayBaseLibrary::LatencyTestHost()->TestLatency(id, engine);
    }

    void ProfileManager::ClearGroupUsage(const GroupId &id)
    {
        Q_D(ProfileManager);
        for (const auto &conn : d->groups.value(id).connections)
        {
            ClearConnectionUsage({ conn, id });
        }
    }
    void ProfileManager::ClearConnectionUsage(const ProfileId &id)
    {
        Q_D(ProfileManager);
        CheckValidId(id.connectionId, nothing);
        d->connections[id.connectionId].statistics.clear();
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionStats>({ id.connectionId, {} });
        return;
    }
    const QList<GroupId> ProfileManager::GetGroups(const ConnectionId &connId) const
    {
        Q_D(const ProfileManager);
        CheckValidId(connId, {});
        QList<GroupId> grps;
        for (auto it = d->groups.constKeyValueBegin(); it != d->groups.constKeyValueEnd(); it++)
        {
            if (it->second.connections.contains(connId))
                grps.push_back(it->first);
        }
        return grps;
    }

    bool ProfileManager::RestartConnection()
    {
        return StartConnection(Qv2rayBaseLibrary::KernelManager()->CurrentConnection());
    }

    void ProfileManager::RenameConnection(const ConnectionId &id, const QString &newName)
    {
        Q_D(ProfileManager);
        CheckValidId(id, nothing);
        emit OnConnectionRenamed(id, d->connections[id].name, newName);
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::Renamed, NullGroupId, id, d->connections[id].name });
        d->connections[id].name = newName;
        SaveConnectionConfig();
    }

    bool ProfileManager::RemoveFromGroup(const ConnectionId &id, const GroupId &gid)
    {
        Q_D(ProfileManager);
        CheckValidId(id, false);
        qInfo() << "Removing connection:" << id;
        if (d->groups[gid].connections.contains(id))
        {
            auto removedEntries = d->groups[gid].connections.removeAll(id);
            if (removedEntries > 1)
            {
                qInfo() << "Found same connection occured multiple times in a group.";
            }
            // Decrease reference count.
            d->connections[id]._group_ref -= removedEntries;
        }

        // Emit everything first then clear the connection map.
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::RemovedFromGroup, gid, id, "" });
        emit OnConnectionRemovedFromGroup({ id, gid });

        if (d->connections[id]._group_ref <= 0)
        {
            qInfo() << "Fully removing a connection from cache.";
            d->connectionRootCache.remove(id);
            Qv2rayBaseLibrary::StorageProvider()->DeleteConnection(id);
            d->connections.remove(id);
        }
        return true;
    }

    bool ProfileManager::LinkWithGroup(const ConnectionId &id, const GroupId &newGroupId)
    {
        Q_D(ProfileManager);
        CheckValidId(id, false);
        if (d->groups[newGroupId].connections.contains(id))
        {
            qInfo() << "Connection not linked since" << id << "is already in the group" << newGroupId;
            return false;
        }
        d->groups[newGroupId].connections.append(id);
        d->connections[id]._group_ref++;
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::LinkedWithGroup, newGroupId, id, d->connections[id].name });
        emit OnConnectionLinkedWithGroup({ id, newGroupId });
        return true;
    }

    bool ProfileManager::MoveToGroup(const ConnectionId &id, const GroupId &sourceGid, const GroupId &targetGid)
    {
        Q_D(ProfileManager);
        CheckValidId(id, false);
        CheckValidId(targetGid, false);
        CheckValidId(sourceGid, false);

        if (!d->groups[sourceGid].connections.contains(id))
        {
            qInfo() << "Trying to move a connection away from a group it does not belong to.";
            return false;
        }
        if (d->groups[targetGid].connections.contains(id))
        {
            qInfo() << "The connection:" << id << "is already in the target group:" << targetGid;
            const auto removedCount = d->groups[sourceGid].connections.removeAll(id);
            d->connections[id]._group_ref -= removedCount;
        }
        else
        {
            // If the target group does not contain this connection.
            const auto removedCount = d->groups[sourceGid].connections.removeAll(id);
            d->connections[id]._group_ref -= removedCount;
            //
            d->groups[targetGid].connections.append(id);
            d->connections[id]._group_ref++;
        }

        emit OnConnectionRemovedFromGroup({ id, sourceGid });
        emit OnConnectionLinkedWithGroup({ id, targetGid });

        return true;
    }

    bool ProfileManager::DeleteGroup(const GroupId &id, bool removeConnections)
    {
        Q_D(ProfileManager);
        CheckValidId(id, false);

        if (id == DefaultGroupId)
        {
            if (removeConnections)
                for (const auto &conn : d->groups[id].connections)
                    RemoveFromGroup(conn, id);
            return false;
        }

        for (const auto &conn : d->groups[id].connections)
            if (removeConnections)
                RemoveFromGroup(conn, id);
            else
                MoveToGroup(conn, id, DefaultGroupId);

        const auto list = d->groups[id].connections;

        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::FullyRemoved, id, NullConnectionId, d->groups[id].name });
        d->groups.remove(id);
        SaveConnectionConfig();
        emit OnGroupDeleted(id, list);
        if (id == DefaultGroupId)
        {
            d->groups[id].name = tr("Default Group");
        }
        return true;
    }

    bool ProfileManager::StartConnection(const ProfileId &identifier)
    {
        Q_D(ProfileManager);
        CheckValidId(identifier, false);
        ProfileContent root = GetConnection(identifier.connectionId);

        const auto groupRouting = GetRouting(d->groups[identifier.groupId].route_id);
        const auto globalRouting = GetRouting(DefaultRoutingId);

        if (!root.routing.overrideDNS)
        {
            if (groupRouting.overrideDNS)
            {
                root.routing.dns = groupRouting.dns;
                root.routing.fakedns = groupRouting.fakedns;
                JsonStructHelper::MergeJson(root.routing.extraOptions, groupRouting.extraOptions);
            }
            else
            {
                root.routing.dns = globalRouting.dns;
                root.routing.fakedns = globalRouting.fakedns;
                JsonStructHelper::MergeJson(root.routing.extraOptions, globalRouting.extraOptions);
            }
        }

        if (!root.routing.overrideRules)
        {
            if (groupRouting.overrideRules)
            {
                root.routing.rules = groupRouting.rules;
                JsonStructHelper::MergeJson(root.routing.extraOptions, groupRouting.extraOptions);
            }
            else
            {
                root.routing.rules = globalRouting.rules;
                JsonStructHelper::MergeJson(root.routing.extraOptions, globalRouting.extraOptions);
            }
        }

        const auto newProfile = Qv2rayBaseLibrary::PluginAPIHost()->PreprocessProfile(root);

        auto errMsg = Qv2rayBaseLibrary::KernelManager()->StartConnection(identifier, newProfile);
        if (errMsg)
        {
            Qv2rayBaseLibrary::Warn(tr("Failed to start connection"), *errMsg);
            return false;
        }
        d->connections[identifier.connectionId].last_connected = system_clock::now();
        return true;
    }

    void ProfileManager::StopConnection()
    {
        Qv2rayBaseLibrary::KernelManager()->StopConnection();
    }

    const ProfileContent ProfileManager::GetConnection(const ConnectionId &id) const
    {
        Q_D(const ProfileManager);
        CheckValidId(id, ProfileContent());
        return d->connectionRootCache.value(id);
    }

    void ProfileManager::p_OnLatencyDataArrived(const ConnectionId &id, const Qv2rayPlugin::LatencyTestResponse &data)
    {
        Q_D(ProfileManager);
        CheckValidId(id, nothing);
        d->connections[id].latency = data.avg;
    }

    void ProfileManager::UpdateConnection(const ConnectionId &id, const ProfileContent &root)
    {
        Q_D(ProfileManager);
        CheckValidId(id, nothing);
        d->connectionRootCache[id] = root;
        Qv2rayBaseLibrary::StorageProvider()->StoreConnection(id, root);
        emit OnConnectionModified(id);
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::Edited, NullGroupId, id, d->connections[id].name });
    }

    const GroupId ProfileManager::CreateGroup(const QString &displayName)
    {
        Q_D(ProfileManager);
        GroupId id(GenerateRandomString());
        d->groups[id].name = displayName;
        d->groups[id].created = system_clock::now();
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::Created, id, NullConnectionId, displayName });
        emit OnGroupCreated(id, displayName);
        SaveConnectionConfig();
        return id;
    }

    const RoutingId ProfileManager::GetGroupRoutingId(const GroupId &id)
    {
        Q_D(ProfileManager);
        if (d->groups[id].route_id.isNull())
            d->groups[id].route_id = RoutingId{ GenerateRandomString() };
        return d->groups[id].route_id;
    }

    void ProfileManager::SetGroupRoutingId(const GroupId &gid, const RoutingId &rid)
    {
        Q_D(ProfileManager);
        CheckValidId(gid, nothing);
        d->groups[gid].route_id = rid;
    }

    RoutingObject ProfileManager::GetRouting(const RoutingId &id) const
    {
        Q_D(const ProfileManager);
        return d->routings.contains(id) ? d->routings.value(id) : d->routings.value(DefaultRoutingId);
    }

    void ProfileManager::UpdateRouting(const RoutingId &id, const RoutingObject &o)
    {
        Q_D(ProfileManager);
        d->routings.insert(id, o);
    }

    bool ProfileManager::RenameGroup(const GroupId &id, const QString &newName)
    {
        Q_D(ProfileManager);
        CheckValidId(id, false);
        emit OnGroupRenamed(id, d->groups[id].name, newName);
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::Renamed, id, NullConnectionId, d->groups[id].name });
        d->groups[id].name = newName;
        return true;
    }

    void ProfileManager::SetSubscriptionData(const GroupId &id, const SubscriptionConfigObject &config)
    {
        Q_D(ProfileManager);
        CheckValidId(id, nothing);
        d->groups[id].subscription_config = config;
    }

    bool ProfileManager::UpdateSubscription(const GroupId &id, bool async)
    {
        Q_D(ProfileManager);
        CheckValidId(id, false);
        if (!d->groups[id].subscription_config.isSubscription)
            return false;

        return p_UpdateSubscriptionImpl(id, async);
    }

    bool ProfileManager::p_UpdateSubscriptionImpl(const GroupId &id, bool isAsync)
    {
        Q_D(ProfileManager);
        CheckValidId(id, false);

        ///
        /// \brief Step 1: Select subscription provider.
        const auto func_select_provider = [d, id]() -> Qv2rayPlugin::SubscriptionProviderInfo
        {
            const auto type = d->groups[id].subscription_config.providerId;
            const auto [plugin, providerInfo] = Qv2rayBaseLibrary::PluginAPIHost()->Subscription_GetProviderInfo(type);
            if (!plugin)
                throw std::runtime_error("Cannot find appropriate subscription provider.");
            return providerInfo;
        };

        ///
        /// \brief Step 2: Fetch and decode subscription according to the provider optiosn.
        const auto fetch_decode_func = [id, d](const Qv2rayPlugin::SubscriptionProviderInfo &info) -> Qv2rayPlugin::SubscriptionResult
        {
            const auto subscriptionConfig = d->groups[id].subscription_config;
            switch (info.mode)
            {
                case Qv2rayPlugin::Subscribe_Decoder:
                {
                    const auto &[err, errString, data] = NetworkRequestHelper::StaticGet(subscriptionConfig.address);
                    if (err == QNetworkReply::NoError)
                        return info.Creator()->DecodeSubscription(data);
                    else
                        throw std::runtime_error("Failed to download subscription:" NEWLINE + errString.toStdString());
                }
                case Qv2rayPlugin::Subscribe_FetcherAndDecoder:
                {
                    return info.Creator()->FetchDecodeSubscription(subscriptionConfig.providerSettings);
                }
                default: Q_UNREACHABLE(); break;
            }
            Q_UNREACHABLE();
        };

        ///
        /// \brief Step 3: begin importing connections from the result.
        const auto process_subscription_func = [this, id, d](const Qv2rayPlugin::SubscriptionResult &result) -> std::tuple<bool, QList<ProfileId>>
        {
            QMultiMap<QString, ProfileContent> fetchedConnections;

            fetchedConnections += result.GetValue<Qv2rayPlugin::SR_ProfileContents>();

            for (const auto &[name, outbound] : result.GetValue<Qv2rayPlugin::SR_OutboundObjects>().toStdMultiMap())
                fetchedConnections.insert(name, ProfileContent(outbound));

            for (const auto &link : result.GetValue<Qv2rayPlugin::SR_Links>())
            {
                // Assign a group name, to pass the name check.
                const auto linkResult = ConvertConfigFromString(link.trimmed());
                if (!linkResult)
                {
                    qInfo() << "Error: Cannot decode share link: " << link;
                    continue;
                }
                fetchedConnections.insert(linkResult->first, linkResult->second);
            }

            const auto tags = result.GetValue<Qv2rayPlugin::SR_Tags>();

            //
            // ====================================================================================== Begin Connection Data Storage
            // Anyway, we try our best to preserve the connection id.
            QMultiMap<QString, ConnectionId> nameMap;
            QMultiHash<IOBoundData, ConnectionId> typeMap;
            {
                // Store connection type metadata into map.
                for (const auto &conn : d->groups[id].connections)
                {
                    nameMap.insert(GetDisplayName(conn), conn);
                    const auto outbounds = GetConnection(conn).outbounds;
                    if (!outbounds.isEmpty())
                    {
                        const auto info = GetOutboundInfo(outbounds.first());
                        typeMap.insert(info, conn);
                    }
                    else
                    {
                        qWarning() << "Met a connection with no outbounds, not saving to type maps.";
                    }
                }
            }

            // ====================================================================================== End Connection Data Storage
            //
            bool hasErrorOccured = false;
            // Copy construct here.

            QList<ConnectionId> originalConnectionIdList;
            originalConnectionIdList.reserve(d->groups[id].connections.size());
            for (const auto &_id : d->groups[id].connections)
                originalConnectionIdList << _id;
            d->groups[id].connections.clear();

            QMultiMap<QString, ProfileContent> filteredConnections;
            for (auto it = fetchedConnections.constKeyValueBegin(); it != fetchedConnections.constKeyValueEnd(); it++)
            {
                const auto name = it->first;
                const auto config = it->second;
                const auto includeRelation = d->groups[id].subscription_config.includeRelation;
                const auto excludeRelation = d->groups[id].subscription_config.excludeRelation;

                const auto includeKeywords = d->groups[id].subscription_config.includeKeywords;
                const auto excludeKeywords = d->groups[id].subscription_config.excludeKeywords;

                //
                // Matched cases (when ShouldHaveThisConnection is NOT ALTERED by the loop below):
                // Relation = OR  -> ShouldHaveThisConnection = FALSE --> None of the keywords can be found.       (Case 1: Since even one     match   will alter the
                // value.) Relation = AND -> ShouldHaveThisConnection = TRUE  --> All keywords are in the connection name. (Case 2: Since even one not matched will alter
                // the value.)
                bool ShouldHaveThisConnection = includeRelation == SubscriptionConfigObject::RELATION_AND;
                {
                    bool isIncludeKeywordsListEffective = false;
                    for (const auto &includeKey : includeKeywords)
                    {
                        // Empty, or spaced string is not "effective"
                        if (includeKey.trimmed().isEmpty())
                            continue;

                        isIncludeKeywordsListEffective = true;

                        // Matched cases:
                        // Case 1: Relation = OR  && Contains
                        // Case 2: Relation = AND && Not Contains
                        if ((includeRelation == SubscriptionConfigObject::RELATION_OR) == name.contains(includeKey.trimmed()))
                        {
                            // Relation = OR  -> Should     include the connection
                            // Relation = AND -> Should not include the connection (since keyword is "Not Contained" in the connection name).
                            ShouldHaveThisConnection = includeRelation == SubscriptionConfigObject::RELATION_OR;

                            // We have already made the decision, skip checking for other "include" keywords.
                            break;
                        }
                        else
                        {
                            // -- For other two cases:
                            // Case 3: Relation = OR  && Not Contains -> Check Next.
                            // Case 4: Relation = AND && Contains     -> Check Next: Cannot determine if all keywords are contained in the connection name.
                            continue;
                        }
                    }

                    // In case of the list of include keywords is empty: (We consider a QList<QString> with only empty strings, or just spaces, is still empty)
                    if (!isIncludeKeywordsListEffective)
                        ShouldHaveThisConnection = true;
                }

                // Continue check for exclude relation if we still want this connection for now.
                if (ShouldHaveThisConnection)
                {
                    bool isExcludeKeywordsListEffective = false;
                    ShouldHaveThisConnection = excludeRelation == SubscriptionConfigObject::RELATION_OR;
                    for (const auto &excludeKey : excludeKeywords)
                    {
                        if (excludeKey.trimmed().isEmpty())
                            continue;

                        isExcludeKeywordsListEffective = true;

                        // Matched cases:
                        // Relation = OR  && Contains
                        // Relation = AND && Not Contains
                        if (excludeRelation == SubscriptionConfigObject::RELATION_OR == name.contains(excludeKey.trimmed()))
                        {
                            // Relation = OR  -> Should not include the connection (since the EXCLUDED keyword is in the connection name).
                            // Relation = AND -> Should     include the connection (since we can say "not ALL exclude keywords can be found", so that the AND relation
                            // breaks)
                            ShouldHaveThisConnection = excludeRelation == SubscriptionConfigObject::RELATION_AND;
                            break;
                        }
                        else
                        {
                            // -- For other two cases:
                            // Relation = OR  && Not Contains -> Check Next.
                            // Relation = AND && Contains     -> Check Next: Cannot determine if all keywords are contained in the connection name.
                            continue;
                        }
                    }

                    // See above comment for "isIncludeKeywordsListEffective"
                    if (!isExcludeKeywordsListEffective)
                        ShouldHaveThisConnection = true;
                }

                // So we finally made the decision.
                if (ShouldHaveThisConnection)
                    filteredConnections.insert(name, config);
            }

            QList<ProfileId> newConnections;

            for (auto it = filteredConnections.constKeyValueBegin(); it != filteredConnections.constKeyValueEnd(); it++)
            {
                const auto name = it->first;
                const auto config = it->second;
                // Should not have complex connection as we assume.
                const auto outboundData = GetOutboundInfo(config.outbounds.first());

                // Firstly we try to preserve connection ids by comparing with names.
                if (nameMap.contains(name))
                {
                    // Just go and save the connection...
                    qInfo() << "Reused connection id from name:" << name;
                    const auto cid = nameMap.take(name);
                    d->groups[id].connections << cid;

                    UpdateConnection(cid, config);
                    SetConnectionTags(cid, tags.value(name));

                    // Remove Connection Id from the list.
                    originalConnectionIdList.removeAll(cid);
                    typeMap.remove(typeMap.key(cid));
                    continue;
                }

                if (typeMap.contains(outboundData))
                {
                    qInfo() << "Reused connection id from protocol/host/port pair for connection:" << name;
                    const auto cid = typeMap.take(outboundData);
                    d->groups[id].connections << cid;

                    UpdateConnection(cid, config);
                    RenameConnection(cid, name);
                    SetConnectionTags(cid, tags.value(name));

                    // Remove Connection Id from the list.
                    originalConnectionIdList.removeAll(cid);
                    nameMap.remove(nameMap.key(cid));
                    continue;
                }

                // New connection id is required since nothing matched found...
                qInfo() << "Generated new connection id for connection:" << name;
                const auto cid = CreateConnection(config, name, id);
                newConnections << cid;
                SetConnectionTags(cid.connectionId, tags.value(name));
            }

            // In case there are deltas
            if (!originalConnectionIdList.isEmpty())
            {
                qInfo() << "Removed old d->connections not have been matched.";
                for (const auto &conn : originalConnectionIdList)
                {
                    qInfo() << "Removing d->connections not in the new subscription:" << conn;
                    RemoveFromGroup(conn, id);
                }
            }

            // Update the time
            d->groups[id].updated = system_clock::now();
            return { hasErrorOccured, newConnections };
        };

#if QT_CONFIG(concurrent)
        QFuture<std::tuple<bool, QList<ProfileId>>> future = QtConcurrent::run(func_select_provider).then(fetch_decode_func).then(process_subscription_func);
        // Thread hack: to keep Messagebox always on the main thread.
        if (isAsync)
        {
            auto newfuture = future
                                 .then(this,
                                       [this, id](const std::tuple<bool, QList<ProfileId>> &tup)
                                       {
                                           if (const auto [r, newconnections] = tup; r)
                                               emit OnSubscriptionUpdateFinished(id, newconnections);
                                       })
                                 .onFailed(
                                     [](const std::exception &e)
                                     {
                                         Qv2rayBaseLibrary::Warn(tr("Cannot update subscription"), QString::fromStdString(e.what()));
                                         return false;
                                     });
        }
        else
        {
            try
            {
                const auto [r, conns] = future.result();
                emit OnSubscriptionUpdateFinished(id, conns);
                return r;
            }
            catch (const std::exception &e)
            {
                Qv2rayBaseLibrary::Warn(tr("Cannot update subscription"), QString::fromStdString(e.what()));
                return false;
            }
        }

        return true;
#else
        try
        {
            return std::get<0>(process_subscription_func(fetch_decode_func(func_select_provider())));
        }
        catch (const std::runtime_error &e)
        {
            Qv2rayBaseLibrary::Warn(tr("Cannot update subscription"), QString::fromStdString(e.what()));
            return false;
        }
#endif
    }

    void ProfileManager::IgnoreSubscriptionUpdate(const GroupId &group)
    {
        Q_D(ProfileManager);
        CheckValidId(group, nothing);
        if (d->groups[group].subscription_config.isSubscription)
            d->groups[group].updated = system_clock::now();
    }

    void ProfileManager::p_OnStatsDataArrived(const ProfileId &id, const StatisticsObject &speed)
    {
        Q_D(ProfileManager);
        if (id.isNull())
            return;

        const auto &cid = id.connectionId;
        d->connections[cid].statistics.directUp += speed.directUp;
        d->connections[cid].statistics.directDown += speed.directDown;
        d->connections[cid].statistics.proxyUp += speed.proxyUp;
        d->connections[cid].statistics.proxyDown += speed.proxyDown;

        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionStats>({ cid, d->connections[cid].statistics });
    }

    const ProfileId ProfileManager::CreateConnection(const ProfileContent &root, const QString &name, const GroupId &groupId)
    {
        ProfileContent newroot = root;

        Q_D(ProfileManager);
        qInfo() << "Creating new connection:" << name;

        for (auto i = 0; i < newroot.inbounds.size(); i++)
            if (newroot.inbounds.at(i).name.isEmpty())
                newroot.inbounds[i].name = name + u"-inbound-"_qs + QString::number(i + 1);

        for (auto i = 0; i < newroot.outbounds.size(); i++)
            if (newroot.outbounds.at(i).name.isEmpty())
                newroot.outbounds[i].name = name + u"-outbound-"_qs + QString::number(i + 1);

        ConnectionId newId(GenerateRandomString());
        d->groups[groupId].connections << newId;
        d->connections[newId].created = system_clock::now();
        d->connections[newId].name = name;
        d->connections[newId]._group_ref = 1;
        d->connectionRootCache[newId] = newroot;
        Qv2rayBaseLibrary::StorageProvider()->StoreConnection(newId, newroot);
        emit OnConnectionCreated({ newId, groupId }, name);
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<ConnectionEntry>({ ConnectionEntry::Created, groupId, newId, name });
        return { newId, groupId };
    }

    void ProfileManager::SetConnectionTags(const ConnectionId &id, const QStringList &tags)
    {
        Q_D(ProfileManager);
        CheckValidId(id, nothing);
        d->connections[id].tags = { tags.begin(), tags.end() };
    }

    const QList<ConnectionId> ProfileManager::GetConnections() const
    {
        Q_D(const ProfileManager);
        return d->connections.keys();
    }

    const QList<ConnectionId> ProfileManager::GetConnections(const GroupId &groupId) const
    {
        Q_D(const ProfileManager);
        CheckValidId(groupId, {});
        return d->groups[groupId].connections;
    }

    const QList<GroupId> ProfileManager::GetGroups() const
    {
        Q_D(const ProfileManager);
        auto k = d->groups.keys();
        std::sort(k.begin(), k.end(), [&](const GroupId &idA, const GroupId &idB) { return d->groups[idA].name < d->groups[idB].name; });
        return k;
    }

    const ConnectionObject ProfileManager::GetConnectionObject(const ConnectionId &id) const
    {
        Q_D(const ProfileManager);
        CheckValidId(id, {});
        return d->connections[id];
    }

    const GroupObject ProfileManager::GetGroupObject(const GroupId &id) const
    {
        Q_D(const ProfileManager);
        CheckValidId(id, {});
        return d->groups[id];
    }

    bool ProfileManager::IsConnected(const ProfileId &id) const
    {
        return Qv2rayBaseLibrary::KernelManager()->CurrentConnection() == id;
    }

    bool ProfileManager::IsValidId(const ConnectionId &id) const
    {
        Q_D(const ProfileManager);
        return d->connections.contains(id);
    }

    bool ProfileManager::IsValidId(const GroupId &id) const
    {
        Q_D(const ProfileManager);
        return d->groups.contains(id);
    }

    bool ProfileManager::IsValidId(const ProfileId &id) const
    {
        return IsValidId(id.connectionId) && IsValidId(id.groupId);
    }

    bool ProfileManager::IsValidId(const RoutingId &id) const
    {
        Q_D(const ProfileManager);
        return d->routings.contains(id);
    }
} // namespace Qv2rayBase::Profile

#undef CheckIdExistance
#undef CheckGroupExistanceEx
#undef CheckGroupExistance
#undef CheckConnectionExistanceEx
#undef CheckConnectionExistance
