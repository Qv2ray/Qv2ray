#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "components/latency/QvTCPing.hpp"
#include "core/CoreSafeTypes.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/handler/KernelInstanceHandler.hpp"

#define CheckIdExistance(type, id, val)                                                                                                         \
    if (!type.contains(id))                                                                                                                     \
    {                                                                                                                                           \
        return val;                                                                                                                             \
    }

#define CheckGroupExistanceEx(id, val) CheckIdExistance(groups, id, val)
#define CheckGroupExistance(id) CheckGroupExistanceEx(id, tr("Group does not exist"))

#define CheckConnectionExistanceEx(id, val) CheckIdExistance(connections, id, val)
#define CheckConnectionExistance(id) CheckConnectionExistanceEx(id, tr("Connection does not exist"))
namespace Qv2ray::core::handlers
{
    class QvConfigHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit QvConfigHandler();
        ~QvConfigHandler();

      public slots:
        inline const QList<ConnectionId> Connections() const
        {
            return connections.keys();
        }
        inline const QList<ConnectionId> Connections(const GroupId &groupId) const
        {
            CheckGroupExistanceEx(groupId, {});
            return groups[groupId].connections;
        }
        inline const QList<GroupId> AllGroups() const
        {
            return groups.keys();
        }
        inline const ConnectionMetaObject GetConnectionMetaObject(const ConnectionId &id) const
        {
            CheckConnectionExistanceEx(id, {});
            return connections[id];
        }
        inline const GroupMetaObject GetGroupMetaObject(const GroupId &id) const
        {
            CheckGroupExistanceEx(id, {});
            return groups[id];
        }
        inline bool IsSubscription(const GroupId &id) const
        {
            CheckGroupExistanceEx(id, {});
            return groups[id].isSubscription;
        }
        //
        //
        void CHSaveConfigData();
        const QList<GroupId> Subscriptions() const;
        //
        // Get Options
        const GroupId GetGroupIdByDisplayName(const QString &displayName) const;
        /// TRY NOT TO USE THIS FUNCTION
        const ConnectionId GetConnectionIdByDisplayName(const QString &displayName, const GroupId &group) const;
        //
        // Connectivity Operationss
        const optional<QString> StartConnection(const ConnectionId &identifier);
        void StopConnection(); // const ConnectionId &id
        void RestartConnection();
        bool IsConnected(const ConnectionId &id) const;
        //
        // Connection Operations.
        bool UpdateConnection(const ConnectionId &id, const CONFIGROOT &root, bool skipRestart = false);
        void ClearGroupUsage(const GroupId &id);
        void ClearConnectionUsage(const ConnectionId &id);
        const optional<QString> DeleteConnection(const ConnectionId &id);
        const optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
        const optional<QString> MoveConnectionGroup(const ConnectionId &id, const GroupId &newGroupId);
        const ConnectionId CreateConnection(const QString &displayName, const GroupId &groupId, const CONFIGROOT &root,
                                            bool skipSaveConfig = false);
        //
        // Get Conncetion Property
        const CONFIGROOT GetConnectionRoot(const ConnectionId &id) const;
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
        bool UpdateSubscription(const GroupId &id);
        // bool UpdateSubscriptionASync(const GroupId &id, bool useSystemProxy);
        const tuple<QString, int64_t, float> GetSubscriptionData(const GroupId &id) const;

      signals:
        void OnKernelLogAvailable(const ConnectionId &id, const QString &log);
        void OnStatsAvailable(const ConnectionId &id, const quint64 upS, const quint64 downS, const quint64 upD, const quint64 downD);
        //
        void OnConnectionCreated(const ConnectionId &id, const QString &displayName);
        void OnConnectionRenamed(const ConnectionId &id, const QString &originalName, const QString &newName);
        void OnConnectionDeleted(const ConnectionId &id, const GroupId &originalGroupId);
        void OnConnectionModified(const ConnectionId &id);
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
        void OnConnected(const ConnectionId &id);
        void OnDisconnected(const ConnectionId &id);
        void OnKernelCrashed(const ConnectionId &id, const QString &errMessage);
        //
      private slots:
        void OnKernelCrashed_p(const ConnectionId &id, const QString &errMessage);
        void OnLatencyDataArrived_p(const QvTCPingResultObject &data);
        void OnStatsDataArrived_p(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed);

      protected:
        void timerEvent(QTimerEvent *event) override;

      private:
        bool CHUpdateSubscription_p(const GroupId &id, const QByteArray &subscriptionData);

      private:
        int saveTimerId;
        int pingAllTimerId;
        int pingConnectionTimerId;
        QHash<GroupId, GroupMetaObject> groups;
        QHash<ConnectionId, ConnectionMetaObject> connections;
        QHash<ConnectionId, CONFIGROOT> connectionRootCache;

      private:
        QvHttpRequestHelper *httpHelper;
        bool isHttpRequestInProgress = false;
        QvTCPingHelper *tcpingHelper;
        KernelInstanceHandler *kernelHandler;
    };

    inline ::Qv2ray::core::handlers::QvConfigHandler *ConnectionManager = nullptr;
} // namespace Qv2ray::core::handlers

using namespace Qv2ray::core::handlers;
