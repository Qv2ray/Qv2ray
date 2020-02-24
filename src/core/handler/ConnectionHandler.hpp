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
            // Generic Get Options
            const QString GetDisplayName(const ConnectionId &id, int limit = -1) const;
            const QString GetDisplayName(const GroupId &id, int limit = -1) const;
            //
            // Connectivity Operationss
            bool IsConnected(const ConnectionId &id) const;
            const optional<QString> StartConnection(const ConnectionId &identifier);
            void StopConnection(); //const ConnectionId &id
            //
            // Connection Operations.
            const GroupId GetConnectionGroupId(const ConnectionId &id) const;
            int64_t GetConnectionLatency(const ConnectionId &id) const;
            const ConnectionId &CreateConnection(const QString &displayName, const GroupId &groupId, const CONFIGROOT &root);
            const optional<QString> DeleteConnection(const ConnectionId &id);
            const optional<QString> UpdateConnection(const ConnectionId &id, const CONFIGROOT &root);
            const optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
            const optional<QString> DuplicateConnection(const ConnectionId &id);
            const optional<QString> MoveConnectionGroup(const ConnectionId &id, const GroupId &newGroupId);
            //
            // Get Conncetion Property
            const QString GetConnectionProtocolString(const ConnectionId &id) const;
            const tuple<QString, int> GetConnectionInfo(const ConnectionId &connectionId) const;
            const tuple<quint64, quint64> GetConnectionUsageAmount(const ConnectionId &id) const;
            //
            // Misc Connection Operations
            const optional<QString> TestLatency();
            const optional<QString> TestLatency(const GroupId &id);
            const optional<QString> TestLatency(const ConnectionId &id);
            //
            // Group Operations
            //const GroupMetaObject GetGroup(const GroupId &id) const;
            const optional<QString> DeleteGroup(const GroupId &id);
            const optional<QString> DuplicateGroup(const GroupId &id);
            const GroupId &CreateGroup(const QString displayName, bool isSubscription);
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
            void OnVCoreLogAvailable(const ConnectionId &id, const QString &log);
            void OnStatsAvailable(const ConnectionId &id, uint64_t totalUpload, uint64_t totalDownload);
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

        private slots:
            void OnStatsDataArrived(const ConnectionId &id, const QString tag, const quint64 uploadSpeed, const quint64 downloadSpeed);
            void OnVCoreCrashed(const ConnectionId &id);

        protected:
            void timerEvent(QTimerEvent *event) override;

        private:
            void CHSaveConnectionData_p();
            //
            bool CHGetOutboundData_p(const OUTBOUND &out, QString *host, int *port) const;
            optional<QString> CHStartConnection_p(const ConnectionId &id, const CONFIGROOT &root);
            void CHStopConnection_p();
            const CONFIGROOT CHGetConnectionRoot_p(const ConnectionId &id) const;
            const CONFIGROOT CHGetConnectionRoot_p(const GroupId &group, const ConnectionId &id) const;
            bool CHSaveConnectionConfig_p(CONFIGROOT obj, const ConnectionId &id, bool override);
            //
            //
            // We only support one cuncurrent connection currently.
#ifdef QV2RAY_MULTIPlE_ONNECTION
            QHash<ConnectionId, V2rayKernelInstance> kernelInstances;
#else
            ConnectionId currentConnectionId;
            V2rayKernelInstance *vCoreInstance = nullptr;
#endif
            int saveTimerId;
            int apiTimerId;
            QHash<GroupId, GroupMetaObject> groups;
            QHash<ConnectionId, ConnectionMetaObject> connections;
            //
            QHash<ConnectionId, CONFIGROOT> connectionRootCache;
    };

    inline ::Qv2ray::core::handlers::QvConnectionHandler *ConnectionManager = nullptr;
}

using namespace Qv2ray::core::handlers;
