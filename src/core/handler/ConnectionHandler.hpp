#pragma once

#include "base/Qv2rayBase.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include "core/CoreSafeTypes.hpp"
#include "core/connection/ConnectionIO.hpp"

namespace Qv2ray::core::handlers
{

    struct ConnectionIdentifier {
        bool isConnection;
        bool isSubscription;
        ConnectionId connectionId;
        GroupId groupId;
        SubscriptionId subscriptionId;
        //
        ConnectionIdentifier() : isSubscription(false), connectionId("null"), groupId("null"), subscriptionId("null") { }
    };

    class QvConnectionHandler : public QObject
    {
            Q_OBJECT
        public:
            explicit QvConnectionHandler();
            ~QvConnectionHandler();
            //
            const QList<ConnectionIdentifier> Connections() const;
            //
            const QList<GroupId> Groups() const;
            const QList<SubscriptionId> Subscriptions() const;
            const QList<ConnectionIdentifier> Connections(const GroupId &groupId) const;
            const QList<ConnectionIdentifier> Connections(const SubscriptionId &subscriptionId) const;
            //
            bool IsConnectionConnected(const ConnectionId &id);
            //
            const optional<QString> StopConnection(const ConnectionId &id);
            const optional<QString> StartConnection(const ConnectionIdentifier &identifier);
            //
        public:
            //
            // Connection Operations.
            const ConnectionObject &GetConnection(const ConnectionId &id);
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
            const optional<QString> TestLatency(const SubscriptionId &id);
            const optional<QString> TestAllLatency();
            //
            // Group Operations
            const GroupObject &GetGroup(const GroupId &id);
            const GroupId &CreateGroup(const QString displayName);
            const optional<QString> DeleteGroup(const GroupId &id);
            const optional<QString> DuplicateGroup(const GroupId &id);
            const optional<QString> RenameGroup(const GroupId &id, const QString &newName);
            //
            // Subscriptions
            const SubscriptionObject &GetSubscription(const SubscriptionId &id);
            const SubscriptionId &CreateSubscription(const QString &displayName, const QString &address);
            const optional<QString> RenameSubscription(const SubscriptionId &id, const QString &newName);
            const optional<QString> DeleteSubscription(const SubscriptionId &id);
            const optional<QString> UpdateSubscription(const SubscriptionId &id);
            const optional<QString> UpdateSubscriptionASync(const SubscriptionId &id);

        signals:
            //
            void OnConnected(const ConnectionId &id);
            void OnDisConnected(const ConnectionId &id);
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
            void OnSubscriptionCreated(const SubscriptionId &id, const QString &displayName, const QString &address);
            void OnSubscriptionDeleted(const SubscriptionId &id);
            void OnSubscriptionRenamed(const SubscriptionId &id, const QString &oldName, const QString &newName);
            void OnSubscriptionUpdateFinished(const SubscriptionId &id);

        private:
            int saveTimerId;
            QHash<GroupId, GroupObject> groups;
            QHash<ConnectionId, ConnectionObject> connections;
            QHash<SubscriptionId, SubscriptionObject> subscriptions;
            //
            // We only support one cuncurrent connection currently.
            //QHash<ConnectionId, V2rayKernelInstance> kernelInstances;
            V2rayKernelInstance *kernelInstance = nullptr;
            //
            optional<QString> _CHTryStartConnection_p(const ConnectionId &id, const CONFIGROOT &root);
            const QList<ConnectionIdentifier> IdentifierList(const GroupId &groupId, const QList<ConnectionId> &idList) const;
            const QList<ConnectionIdentifier> IdentifierList(const SubscriptionId &groupId, const QList<ConnectionId> &idList) const;
    };
    //
    inline QvConnectionHandler *ConnectionHandler = nullptr;
}

using namespace Qv2ray::core::handlers;
