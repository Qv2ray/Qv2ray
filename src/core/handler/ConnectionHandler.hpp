#pragma once

#include "base/Qv2rayBase.hpp"
#include "core/kernel/KernelInteractions.hpp"

namespace Qv2ray::core::handlers
{
    template <typename T>
    class IDType
    {
        public:
            IDType(const QString &id): m_id(id) {}
            friend bool operator==(const IDType<T> &lhs, const IDType<T> &rhs)
            {
                return lhs.m_id == rhs.m_id;
            }
        private:
            QString m_id;
    };
    //
    class __QvGroup;
    class __QvConnection;
    class __QvSubscription;
    typedef IDType<__QvGroup> GroupId;
    typedef IDType<__QvConnection> ConnectionId;
    typedef IDType<__QvSubscription> SubscriptionId;

    class ConnectionHandler : public QObject
    {
            Q_OBJECT
        public:
            explicit ConnectionHandler();
            ~ConnectionHandler();
            //
            const QList<GroupId> Groups() const
            {
                return groups.keys();
            }
            const QList<ConnectionId> Connections() const
            {
                return connections.keys();
            }
            const QList<SubscriptionId> Subscriptions() const
            {
                return subscriptions.keys();
            }
        public:
            //
            optional<QString> StartConnection(const ConnectionId &id);
            optional<QString> StopConnection(const ConnectionId &id);
            //
            // Connection Operations.
            const ConnectionId &CreateConnection(const QString &displayName, const GroupId &groupId, const CONFIGROOT &root);
            optional<QString> DeleteConnection(const ConnectionId &id);
            optional<QString> UpdateConnection(const ConnectionId &id, const CONFIGROOT &root);
            optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
            optional<QString> DuplicateConnection(const ConnectionId &id);
            optional<QString> MoveConnectionGroup(const ConnectionId &id, const GroupId &newGroupId);
            //
            // Misc Connection Operations
            optional<QString> TestLatency(const ConnectionId &id);
            optional<QString> TestLatency(const GroupId &id);
            optional<QString> TestAllLatency();
            //
            // Group Operations
            const GroupId CreateGroup(const QString displayName);
            optional<QString> DeleteGroup(const GroupId &id);
            optional<QString> DuplicateGroup(const GroupId &id);
            optional<QString> RenameGroup(const GroupId &id, const QString &newName);
            //
            // Subscriptions
            const SubscriptionId CreateSubscription(const QString &displayName, const QString &address);
            optional<QString> RenameSubscription(const SubscriptionId &id, const QString &newName);
            optional<QString> DeleteSubscription(const SubscriptionId &id);
            optional<QString> UpdateSubscription(const SubscriptionId &id);
            optional<QString> UpdateSubscriptionASync(const SubscriptionId &id);

        signals:
            void OnConnectionCreated(const ConnectionId &id, const QString &displayName);
            void OnConnectionRenamed(const ConnectionId &id, const QString &originalName, const QString &newName);
            void OnConnectionChanged(const ConnectionId &id);
            void OnConnectionGroupChanged(const ConnectionId &id, const QString &originalGroup, const QString &newGroup);
            //
            void OnConnectionLatencyTestFinished(const ConnectionId &id);
            //
            void OnGroupCreated(const GroupId &id, const QString &displayName);
            void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
            void OnGroupDeleted(const GroupId &id, const QString &displayName);
            //
            void OnSubscriptionCreated(const SubscriptionId &id, const QString &displayName, const QString &address);
            void OnSubscriptionDeleted(const SubscriptionId &id);
            void OnSubscriptionRenamed(const SubscriptionId &id, const QString &oldName, const QString &newName);
            void OnSubscriptionUpdateFinished(const SubscriptionId &id);

        private:
            int saveTimerId;
            QMap<GroupId, QvConnectionObject> groups;
            QMap<ConnectionId, QvConnectionObject> connections;
            QMap<SubscriptionId, QvSubscriptionObject> subscriptions;
            //
            QMap<ConnectionId, V2rayKernelInstance> kernelInstances;
    };
}

using namespace Qv2ray::core::handlers;
