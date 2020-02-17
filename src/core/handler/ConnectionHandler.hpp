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
            const QList<GroupId> Groups() const;
            const QList<ConnectionId> Connections() const;
            const QList<SubscriptionId> Subscriptions() const;
            const QList<ConnectionId> Connections(const GroupId &groupId) const;
            const QList<ConnectionId> Connections(const SubscriptionId &subscriptionId) const;
            //
            optional<QString> StopConnection(const ConnectionId &id);
            //
            template<typename T>
            optional<QString> StartConnection(const T &group, const ConnectionId &id)
            {
                if (!connections.contains(id)) {
                    return tr("No connection selected!") + NEWLINE + tr("Please select a config from the list.");
                }

                auto root = GetConnectionRoot(group, id);
                return _CHTryStartConnection_p(id, root);
            }
            template<typename T> optional<QString> StartConnection(const GroupId &group, const ConnectionId &id);
            template<typename T> optional<QString> StartConnection(const SubscriptionId &group, const ConnectionId &id);
            //
        public:
            //
            // Connection Operations.
            const QvConnectionObject &GetConnection(const ConnectionId &id);
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
            optional<QString> TestLatency(const SubscriptionId &id);
            optional<QString> TestAllLatency();
            //
            // Group Operations
            const QvGroupObject &GetGroup(const GroupId &id);
            const GroupId CreateGroup(const QString displayName);
            optional<QString> DeleteGroup(const GroupId &id);
            optional<QString> DuplicateGroup(const GroupId &id);
            optional<QString> RenameGroup(const GroupId &id, const QString &newName);
            //
            // Subscriptions
            const QvSubscriptionObject &GetSubscription(const SubscriptionId &id);
            const SubscriptionId &CreateSubscription(const QString &displayName, const QString &address);
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
            QHash<GroupId, QvGroupObject> groups;
            QHash<ConnectionId, QvConnectionObject> connections;
            QHash<SubscriptionId, QvSubscriptionObject> subscriptions;
            //
            // We only support one cuncurrent connection currently.
            //QHash<ConnectionId, V2rayKernelInstance> kernelInstances;
            V2rayKernelInstance *kernelInstance = nullptr;
            //
            optional<QString> _CHTryStartConnection_p(const ConnectionId &id, const CONFIGROOT &root);
    };
    //
    inline QvConnectionHandler *ConnectionHandler = nullptr;
}

using namespace Qv2ray::core::handlers;
