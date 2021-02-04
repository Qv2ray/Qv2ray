#pragma once

#include "components/latency/LatencyTest.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/handler/KernelInstanceHandler.hpp"

namespace Qv2ray::common::network
{
    class NetworkRequestHelper;
}

#define CheckValidId(id, returnValue)                                                                                                                \
    if (!IsValidId(id))                                                                                                                              \
        return returnValue;

namespace Qv2ray::core::handler
{
    class QvConfigHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit QvConfigHandler(QObject *parent = nullptr);
        ~QvConfigHandler();

      public slots:
        inline const QList<ConnectionId> GetConnections() const
        {
            return connections.keys();
        }
        inline const QList<ConnectionId> GetConnections(const GroupId &groupId) const
        {
            CheckValidId(groupId, {});
            return groups[groupId].connections;
        }
        inline QList<GroupId> AllGroups() const
        {
            auto k = groups.keys();
            std::sort(k.begin(), k.end(), [&](const GroupId &idA, const GroupId &idB) {
                const auto result = groups[idA].displayName < groups[idB].displayName;
                return result;
            });
            return k;
        }
        inline bool IsValidId(const ConnectionId &id) const
        {
            return connections.contains(id);
        }
        inline bool IsValidId(const GroupId &id) const
        {
            return groups.contains(id);
        }
        inline bool IsValidId(const ConnectionGroupPair &id) const
        {
            return IsValidId(id.connectionId) && IsValidId(id.groupId);
        }
        inline const ConnectionObject GetConnectionMetaObject(const ConnectionId &id) const
        {
            CheckValidId(id, {});
            return connections[id];
        }
        inline GroupObject GetGroupMetaObject(const GroupId &id) const
        {
            CheckValidId(id, {});
            return groups[id];
        }

        bool IsConnected(const ConnectionGroupPair &id) const
        {
            return kernelHandler->CurrentConnection() == id;
        }

        inline void IgnoreSubscriptionUpdate(const GroupId &group)
        {
            CheckValidId(group, nothing);
            if (groups[group].isSubscription)
                groups[group].lastUpdatedDate = system_clock::to_time_t(system_clock::now());
        }

        void SaveConnectionConfig();
        const QList<GroupId> Subscriptions() const;
        const QList<GroupId> GetConnectionContainedIn(const ConnectionId &connId) const;
        //
        // Connectivity Operationss
        bool StartConnection(const ConnectionGroupPair &identifier);
        void StopConnection();
        void RestartConnection();
        //
        // Connection Operations.
        void ClearGroupUsage(const GroupId &id);
        void ClearConnectionUsage(const ConnectionGroupPair &id);
        //
        const ConnectionGroupPair CreateConnection(const CONFIGROOT &root, const QString &displayName, const GroupId &groupId = DefaultGroupId,
                                                   bool skipSaveConfig = false);
        bool UpdateConnection(const ConnectionId &id, const CONFIGROOT &root, bool skipRestart = false);
        const std::optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
        //
        // Connection - Group binding
        bool RemoveConnectionFromGroup(const ConnectionId &id, const GroupId &gid);
        bool MoveConnectionFromToGroup(const ConnectionId &id, const GroupId &sourceGid, const GroupId &targetGid);
        bool LinkConnectionWithGroup(const ConnectionId &id, const GroupId &newGroupId);
        //
        // Get Conncetion Property
        const CONFIGROOT GetConnectionRoot(const ConnectionId &id) const;
        //
        // Misc Connection Operations
        void StartLatencyTest();
        void StartLatencyTest(const GroupId &id);
        void StartLatencyTest(const ConnectionId &id, Qv2rayLatencyTestingMethod method = GlobalConfig.networkConfig.latencyTestingMethod);
        //
        // Group Operations
        const GroupId CreateGroup(const QString &displayName, bool isSubscription);
        const std::optional<QString> DeleteGroup(const GroupId &id);
        const std::optional<QString> RenameGroup(const GroupId &id, const QString &newName);
        const GroupRoutingId GetGroupRoutingId(const GroupId &id);
        // const optional<QString> DuplicateGroup(const GroupId &id);
        //
        // Subscriptions
        void UpdateSubscriptionAsync(const GroupId &id);
        bool UpdateSubscription(const GroupId &id);
        bool SetSubscriptionData(const GroupId &id, std::optional<bool> isSubscription = std::nullopt,
                                 const std::optional<QString> &address = std::nullopt, std::optional<float> updateInterval = std::nullopt);
        bool SetSubscriptionType(const GroupId &id, const QString &type);
        bool SetSubscriptionIncludeKeywords(const GroupId &id, const QStringList &Keywords);
        bool SetSubscriptionExcludeKeywords(const GroupId &id, const QStringList &Keywords);
        bool SetSubscriptionIncludeRelation(const GroupId &id, SubscriptionFilterRelation relation);
        bool SetSubscriptionExcludeRelation(const GroupId &id, SubscriptionFilterRelation relation);

        // bool UpdateSubscriptionASync(const GroupId &id, bool useSystemProxy);
        // const std::tuple<QString, int64_t, float> GetSubscriptionData(const GroupId &id) const;

      signals:
        void OnKernelLogAvailable(const ConnectionGroupPair &id, const QString &log);
        void OnStatsAvailable(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeedData> &data);
        //
        void OnConnectionCreated(const ConnectionGroupPair &Id, const QString &displayName);
        void OnConnectionModified(const ConnectionId &id);
        void OnConnectionRenamed(const ConnectionId &Id, const QString &originalName, const QString &newName);
        //
        void OnConnectionLinkedWithGroup(const ConnectionGroupPair &newPair);
        void OnConnectionRemovedFromGroup(const ConnectionGroupPair &pairId);
        //
        void OnLatencyTestStarted(const ConnectionId &id);
        void OnLatencyTestFinished(const ConnectionId &id, const int average);
        //
        void OnGroupCreated(const GroupId &id, const QString &displayName);
        void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
        void OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections);
        //
        void OnSubscriptionAsyncUpdateFinished(const GroupId &id);
        void OnConnected(const ConnectionGroupPair &id);
        void OnDisconnected(const ConnectionGroupPair &id);
        void OnKernelCrashed(const ConnectionGroupPair &id, const QString &errMessage);
        //
      private slots:
        void p_OnKernelCrashed(const ConnectionGroupPair &id, const QString &errMessage);
        void p_OnLatencyDataArrived(const ConnectionId &id, const LatencyTestResult &data);
        void p_OnStatsDataArrived(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeed> &data);

      protected:
        void timerEvent(QTimerEvent *event) override;

      private:
        bool p_CHUpdateSubscription(const GroupId &id, const QByteArray &data);

      private:
        int saveTimerId;
        int pingAllTimerId;
        int pingConnectionTimerId;
        QHash<GroupId, GroupObject> groups;
        QHash<ConnectionId, ConnectionObject> connections;
        QHash<ConnectionId, CONFIGROOT> connectionRootCache;

      private:
        LatencyTestHost *pingHelper;
        KernelInstanceHandler *kernelHandler;
    };

    inline ::Qv2ray::core::handler::QvConfigHandler *ConnectionManager = nullptr;
} // namespace Qv2ray::core::handler

using namespace Qv2ray::core::handler;
