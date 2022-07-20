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

#pragma once

#include "Qv2rayBase/Qv2rayBaseFeatures.hpp"
#include "QvPlugin/PluginInterface.hpp"

namespace Qv2rayBase::Profile
{
    class ProfileManagerPrivate;
    class QV2RAYBASE_EXPORT ProfileManager
        : public QObject
        , public Qv2rayPlugin::Connections::IProfileManager
    {
        Q_OBJECT
      public:
        explicit ProfileManager(QObject *parent = nullptr);
        virtual ~ProfileManager();

        bool IsValidId(const ConnectionId &id) const;
        bool IsValidId(const GroupId &id) const;
        bool IsValidId(const ProfileId &id) const;
        bool IsValidId(const RoutingId &id) const;

        void SaveConnectionConfig();

        // Connection Related
        const ProfileContent GetConnection(const ConnectionId &id) const override;
        const QList<ConnectionId> GetConnections() const override;
        const QList<ConnectionId> GetConnections(const GroupId &groupId) const override;
        const ConnectionObject GetConnectionObject(const ConnectionId &id) const override;
        const ProfileId CreateConnection(const ProfileContent &root, const QString &name, const GroupId &groupId = DefaultGroupId) override;
        void SetConnectionTags(const ConnectionId &id, const QStringList &tags) override;
        void UpdateConnection(const ConnectionId &id, const ProfileContent &root) override;
        void RenameConnection(const ConnectionId &id, const QString &newName) override;

        // Group Related
        const QList<GroupId> GetGroups() const override;
        const QList<GroupId> GetGroups(const ConnectionId &connId) const override;
        const GroupObject GetGroupObject(const GroupId &id) const override;
        const GroupId CreateGroup(const QString &displayName) override;
        bool DeleteGroup(const GroupId &id, bool removeConnections) override;
        bool RenameGroup(const GroupId &id, const QString &newName) override;

        const RoutingId GetGroupRoutingId(const GroupId &id) override;
        void SetGroupRoutingId(const GroupId &gid, const RoutingId &rid);

        // Routing Related
        RoutingObject GetRouting(const RoutingId &id = DefaultRoutingId) const override;
        void UpdateRouting(const RoutingId &id, const RoutingObject &) override;

        // Connection-Group Relation
        bool RemoveFromGroup(const ConnectionId &id, const GroupId &gid) override;
        bool MoveToGroup(const ConnectionId &id, const GroupId &sourceGid, const GroupId &targetGid) override;
        bool LinkWithGroup(const ConnectionId &id, const GroupId &newGroupId) override;

        // Profile Related
        bool RestartConnection() override;
        bool StartConnection(const ProfileId &identifier) override;
        void StopConnection() override;
        bool IsConnected(const ProfileId &id) const override;

        // Subscription Related
        void IgnoreSubscriptionUpdate(const GroupId &group);
        bool UpdateSubscription(const GroupId &id, bool async);
        void SetSubscriptionData(const GroupId &id, const SubscriptionConfigObject &config);

        // Statistics Related
        void ClearGroupUsage(const GroupId &id);
        void ClearConnectionUsage(const ProfileId &id);

        // Latency Testing Related
        void StartLatencyTest(const ConnectionId &id, const LatencyTestEngineId &engine);
        void StartLatencyTest(const GroupId &id, const LatencyTestEngineId &engine);

      signals:
        void OnLatencyTestStarted(const ConnectionId &id);
        void OnSubscriptionUpdateFinished(const GroupId &id, const QList<ProfileId> &newConnections);
        void OnConnectionCreated(const ProfileId &Id, const QString &displayName);
        void OnConnectionModified(const ConnectionId &id);
        void OnConnectionRenamed(const ConnectionId &Id, const QString &originalName, const QString &newName);

        void OnConnectionLinkedWithGroup(const ProfileId &newPair);
        void OnConnectionRemovedFromGroup(const ProfileId &pairId);

        void OnGroupCreated(const GroupId &id, const QString &displayName);
        void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
        void OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections);

      private slots:
        bool p_UpdateSubscriptionImpl(const GroupId &id, bool isAsync);
        void p_OnLatencyDataArrived(const ConnectionId &id, const Qv2rayPlugin::LatencyTestResponse &data);
        void p_OnStatsDataArrived(const ProfileId &id, const StatisticsObject &speed);

      private:
        QScopedPointer<ProfileManagerPrivate> d_ptr;
        Q_DECLARE_PRIVATE(ProfileManager)
    };
} // namespace Qv2rayBase::Profile
