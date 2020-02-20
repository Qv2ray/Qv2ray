#pragma once

#include "base/Qv2rayBase.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include "core/CoreSafeTypes.hpp"
#include "core/connection/ConnectionIO.hpp"

namespace Qv2ray::core::handlers
{
    class QvConnectionHandler : public QObject
    {
            Q_OBJECT
        public:
            explicit QvConnectionHandler();
            ~QvConnectionHandler();
            //
            const QList<ConnectionId> Connections() const;
            //
            const QList<GroupId> AllGroups() const;
            const QList<GroupId> Subscriptions() const;
            const QList<ConnectionId> Connections(const GroupId &groupId) const;
            //
            bool IsConnectionConnected(const ConnectionId &id) const;
            //
            const optional<QString> StopConnection(const ConnectionId &id);
            const optional<QString> StartConnection(const ConnectionId &identifier);
            //
        public:
            //
            // Connection Operations.
            const QString GetConnectionBasicInfo(const ConnectionId &id) const;
            const ConnectionMetaObject GetConnection(const ConnectionId &id) const;
            const ConnectionId &CreateConnection(const QString &displayName, const GroupId &groupId, const CONFIGROOT &root);
            const optional<QString> DeleteConnection(const ConnectionId &id);
            const optional<QString> UpdateConnection(const ConnectionId &id, const CONFIGROOT &root);
            const optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
            const optional<QString> DuplicateConnection(const ConnectionId &id);
            const optional<QString> MoveConnectionGroup(const ConnectionId &id, const GroupId &newGroupId);
            //
            // Misc Connection Operations
            const optional<QString> TestLatency(const ConnectionId &id);
            const optional<QString> TestLatency(const GroupId &id);
            const optional<QString> TestLatency();
            //
            // Group Operations
            const GroupMetaObject GetGroup(const GroupId &id) const;
            const GroupId &CreateGroup(const QString displayName, bool isSubscription);
            const optional<QString> DeleteGroup(const GroupId &id);
            const optional<QString> DuplicateGroup(const GroupId &id);
            const optional<QString> RenameGroup(const GroupId &id, const QString &newName);
            //
            // Subscriptions
            const GroupId &CreateSubscription(const QString &displayName, const QString &address);
            const optional<QString> RenameSubscription(const GroupId &id, const QString &newName);
            const optional<QString> DeleteSubscription(const GroupId &id);
            const optional<QString> UpdateSubscription(const GroupId &id);
            const optional<QString> UpdateSubscriptionASync(const GroupId &id);

        signals:
            //
            void OnConnected(const ConnectionId &id);
            void OnDisConnected(const ConnectionId &id);
            void OnNewConnectionStatsAvaliable(const ConnectionId &id, uint64_t totalUpload, uint64_t totalDownload);
            //
            void OnConnectionCreated(const ConnectionId &id, const QString &displayName);
            void OnConnectionRenamed(const ConnectionId &id, const QString &originalName, const QString &newName);
            void OnConnectionChanged(const ConnectionId &id);
            void OnConnectionGroupChanged(const ConnectionId &id, const QString &originalGroup, const QString &newGroup);
            //
            void OnConnectionLatencyTestStart(const ConnectionId &id);
            void OnConnectionLatencyTestFinished(const ConnectionId &id);
            //
            void OnGroupCreated(const GroupId &id, const QString &displayName);
            void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
            void OnGroupDeleted(const GroupId &id, const QString &displayName);
            //
            void OnSubscriptionCreated(const GroupId &id, const QString &displayName, const QString &address);
            void OnSubscriptionDeleted(const GroupId &id, const QString &oldName, const QString &newName);
            void OnSubscriptionUpdateFinished(const GroupId &id);

        private:
            optional<QString> CHTryStartConnection_p(const ConnectionId &id, const CONFIGROOT &root);
            const CONFIGROOT CHGetConnectionRoot_p(const GroupId &group, const ConnectionId &id) const;
            bool CHSaveConnectionConfig(CONFIGROOT obj, const ConnectionId &id, bool override);
            //
            // We only support one cuncurrent connection currently.
            //QHash<ConnectionId, V2rayKernelInstance> kernelInstances;
            V2rayKernelInstance *kernelInstance = nullptr;
            int saveTimerId;
            QHash<GroupId, GroupMetaObject> groups;
            QHash<ConnectionId, ConnectionMetaObject> connections;
    };

    inline QvConnectionHandler *ConnectionHandler = nullptr;
}

using namespace Qv2ray::core::handlers;
