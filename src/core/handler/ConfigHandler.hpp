#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "core/CoreSafeTypes.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include "core/tcping/QvTCPing.hpp"

#define CheckIdExistance(type, id, val)                                                                                                         \
    if (!type.contains(id))                                                                                                                     \
    {                                                                                                                                           \
        return val;                                                                                                                             \
    }

#define CheckGroupExistanceEx(id, val) CheckIdExistance(groups, id, val)
#define CheckGroupExistance(id) CheckGroupExistanceEx(id, {})

#define CheckConnectionExistanceEx(id, val) CheckIdExistance(connections, id, val)
#define CheckConnectionExistance(id) CheckConnectionExistanceEx(id, {})

namespace Qv2ray::core::handlers
{
    //
    class QvConfigHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit QvConfigHandler();
        ~QvConfigHandler();

      public slots:
        //
        inline const ConnectionId CurrentConnection() const
        {
            return currentConnectionId;
        }
        inline const QList<ConnectionId> Connections() const
        {
            return connections.keys();
        }
        inline const QList<ConnectionId> Connections(const GroupId &groupId) const
        {
            CheckGroupExistance(groupId);
            return groups[groupId].connections;
        }
        inline const QList<GroupId> AllGroups() const
        {
            return groups.keys();
        }
        inline const ConnectionMetaObject GetConnectionMetaObject(const ConnectionId &id) const
        {
            CheckConnectionExistance(id);
            return connections[id];
        }
        inline const GroupMetaObject GetGroupMetaObject(const GroupId &id) const
        {
            CheckGroupExistance(id);
            return groups[id];
        }
        inline bool IsSubscription(const GroupId &id) const
        {
            CheckGroupExistance(id);
            return groups[id].isSubscription;
        }
        //
        //
        const QList<GroupId> Subscriptions() const;
        //
        // Generic Get Options
        const QString GetDisplayName(const GroupId &id, int limit = -1) const;
        const QString GetDisplayName(const ConnectionId &id, int limit = -1) const;
        const GroupId GetGroupIdByDisplayName(const QString &displayName) const;
        // const ConnectionId GetConnectionIdByDisplayName(const QString &displayName) const;
        const ConnectionId GetConnectionIdByDisplayName(const QString &displayName, const GroupId &group) const;
        //
        // Connectivity Operationss
        const optional<QString> StartConnection(const ConnectionId &identifier);
        void StopConnection(); // const ConnectionId &id
        void RestartConnection();
        bool IsConnected(const ConnectionId &id) const;
        //
        // Connection Operations.
        bool UpdateConnection(const ConnectionId &id, const CONFIGROOT &root);
        const optional<QString> DeleteConnection(const ConnectionId &id);
        const optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
        const optional<QString> MoveConnectionGroup(const ConnectionId &id, const GroupId &newGroupId);
        const ConnectionId CreateConnection(const QString &displayName, const GroupId &groupId, const CONFIGROOT &root);
        //
        // Get Conncetion Property
        const GroupId GetConnectionGroupId(const ConnectionId &id) const;
        const CONFIGROOT GetConnectionRoot(const ConnectionId &id) const;
        const CONFIGROOT GetConnectionRoot(const GroupId &group, const ConnectionId &id) const;
        //
        // Misc Connection Operations
        void StartLatencyTest();
        void StartLatencyTest(const GroupId &id);
        void StartLatencyTest(const ConnectionId &id);
        //
        // Group Operations
        const GroupId CreateGroup(const QString displayName, bool isSubscription);
        const optional<QString> DeleteGroup(const GroupId &id);
        const optional<QString> RenameGroup(const GroupId &id, const QString &newName);
        // const optional<QString> DuplicateGroup(const GroupId &id);
        //
        // Subscriptions
        bool SetSubscriptionData(const GroupId &id, const QString &address = "", float updateInterval = -1);
        bool UpdateSubscription(const GroupId &id, bool useSystemProxy);
        // bool UpdateSubscriptionASync(const GroupId &id, bool useSystemProxy);
        const tuple<QString, int64_t, float> GetSubscriptionData(const GroupId &id) const;

      signals:
        void OnCrashed();
        void OnConnected(const ConnectionId &id);
        void OnDisconnected(const ConnectionId &id);
        void OnVCoreLogAvailable(const ConnectionId &id, const QString &log);
        void OnStatsAvailable(const ConnectionId &id, const quint64 upS, const quint64 downS, const quint64 upD, const quint64 downD);
        //
        void OnConnectionCreated(const ConnectionId &id, const QString &displayName);
        void OnConnectionRenamed(const ConnectionId &id, const QString &originalName, const QString &newName);
        void OnConnectionDeleted(const ConnectionId &id, const GroupId &originalGroupId);
        void OnConnectionChanged(const ConnectionId &id);
        void OnConnectionGroupChanged(const ConnectionId &id, const GroupId &originalGroup, const GroupId &newGroup);
        //
        void OnLatencyTestStarted(const ConnectionId &id);
        void OnLatencyTestFinished(const ConnectionId &id, const uint average);
        //
        void OnGroupCreated(const GroupId &id, const QString &displayName);
        void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
        void OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections);
        //
        void OnSubscriptionUpdateFinished(const GroupId &id);

      private slots:
        void OnStatsDataArrived(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed);
        void OnVCoreCrashed(const ConnectionId &id);
        void OnLatencyDataArrived(const QvTCPingResultObject &data);

      protected:
        void timerEvent(QTimerEvent *event) override;

      private:
        void CHSaveConfigData_p();
        //
        optional<QString> CHStartConnection_p(const ConnectionId &id, const CONFIGROOT &root);
        void CHStopConnection_p();
        bool CHUpdateSubscription_p(const GroupId &id, const QByteArray &subscriptionData);

      private:
        int saveTimerId;
        int pingAllTimerId;
        int pingConnectionTimerId;
        QHash<GroupId, GroupMetaObject> groups;
        QHash<ConnectionId, ConnectionMetaObject> connections;
        // QHash<ConnectionId, CONFIGROOT> connectionRootCache;

      private:
        QvHttpRequestHelper *httpHelper;
        bool isHttpRequestInProgress = false;
        QvTCPingHelper *tcpingHelper;
        // We only support one cuncurrent connection currently.
#ifdef QV2RAY_MULTIPlE_ONNECTION
        QHash<ConnectionId, *V2rayKernelInstance> kernelInstances;
#else
        ConnectionId currentConnectionId = NullConnectionId;
        V2rayKernelInstance *vCoreInstance = nullptr;
#endif
    };

    inline ::Qv2ray::core::handlers::QvConfigHandler *ConnectionManager = nullptr;
} // namespace Qv2ray::core::handlers

using namespace Qv2ray::core::handlers;
