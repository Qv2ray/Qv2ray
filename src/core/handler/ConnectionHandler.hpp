#pragma once

#include "base/Qv2rayBase.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include <QHash>
#include <QHashFunctions>

template <typename T>
class IDType
{
    public:
        IDType(const QString &id): m_id(id) {}
        friend bool operator==(const IDType<T> &lhs, const IDType<T> &rhs)
        {
            return lhs.m_id == rhs.m_id;
        }
        const QString &toString() const
        {
            return m_id;
        }
        uint qHash(uint seed) const
        {
            return ::qHash(m_id, seed);
        }
    private:
        QString m_id;
};

template <typename T> uint qHash(const IDType<T> &key, uint seed = 0)
{
    return key.qHash(seed);
}

namespace Qv2ray::core::handlers
{
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
            const QList<GroupId> Groups() const;
            const QList<ConnectionId> Connections() const;
            const QList<ConnectionId> Connections(const GroupId &groupId) const;
            const QList<ConnectionId> Connections(const SubscriptionId &subscriptionId) const;
            const QList<SubscriptionId> Subscriptions() const;
        public:
            //
            optional<QString> StartConnection(const ConnectionId &id);
            optional<QString> StopConnection(const ConnectionId &id);
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
            QHash<GroupId, QvGroupObject> groups;
            QHash<ConnectionId, QvConnectionObject> connections;
            QHash<SubscriptionId, QvSubscriptionObject> subscriptions;
            //
            QMap<ConnectionId, V2rayKernelInstance> kernelInstances;
    };
    //
    inline unique_ptr<Qv2ray::core::handlers::ConnectionHandler> connectionHandler = nullptr;
    //
    inline void InitialiseConnectionHandler()
    {
        LOG(CORE_HANDLER, "Initializing ConnectionHandler...")
        connectionHandler = make_unique<ConnectionHandler>();
    }
    //
}

using namespace Qv2ray::core::handlers;
