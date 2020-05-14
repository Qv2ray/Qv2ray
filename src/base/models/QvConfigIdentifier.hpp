#pragma once
#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <QHash>
#include <QHashFunctions>
#include <QString>
#include <QtCore>

namespace Qv2ray::base
{
    template<typename T>
    class IDType
    {
      public:
        explicit IDType() : m_id("null"){};
        explicit IDType(const QString &id) : m_id(id){};
        friend bool operator==(const IDType<T> &lhs, const IDType<T> &rhs)
        {
            return lhs.m_id == rhs.m_id;
        }
        friend bool operator!=(const IDType<T> &lhs, const IDType<T> &rhs)
        {
            return lhs.m_id != rhs.m_id;
        }
        const QString &toString() const
        {
            return m_id;
        }
        void loadJson(const QJsonValue &d)
        {
            m_id = d.toString("null");
        }
        QJsonValue toJson() const
        {
            return m_id;
        }

      private:
        QString m_id;
    };

    // Define several safetypes to prevent misuse of QString.
    class __QvGroup;
    class __QvConnection;
    class __QvRoute;
    typedef IDType<__QvGroup> GroupId;
    typedef IDType<__QvConnection> ConnectionId;
    typedef IDType<__QvRoute> RoutingId;
    //
    inline const static auto NullConnectionId = ConnectionId("null");
    inline const static auto NullGroupId = GroupId("null");
    inline const static auto NullRoutingId = RoutingId("null");
    //
    class ConnectionGroupPair
    {
      public:
        ConnectionId connectionId = NullConnectionId;
        GroupId groupId = NullGroupId;
        ConnectionGroupPair() : connectionId(NullConnectionId), groupId(NullGroupId){};
        ConnectionGroupPair(const ConnectionId &conn, const GroupId &group) : connectionId(conn), groupId(group){};
        void clear()
        {
            connectionId = NullConnectionId;
            groupId = NullGroupId;
        }
        bool isEmpty() const
        {
            return connectionId == NullConnectionId;
        }
        friend bool operator==(const ConnectionGroupPair &lhs, const ConnectionGroupPair &rhs)
        {
            return lhs.groupId == rhs.groupId && lhs.connectionId == rhs.connectionId;
        }
        JSONSTRUCT_REGISTER(ConnectionGroupPair, F(connectionId, groupId))
    };
    //
    constexpr unsigned int QVTCPING_VALUE_ERROR = 99999;
    constexpr unsigned int QVTCPING_VALUE_NODATA = QVTCPING_VALUE_ERROR - 1;
    using namespace std::chrono;

    struct __Qv2rayConfigObjectBase
    {
        QString displayName;
        qint64 creationDate;
        qint64 lastUpdatedDate;
        __Qv2rayConfigObjectBase()
            : displayName(), creationDate(system_clock::to_time_t(system_clock::now())), //
              lastUpdatedDate(system_clock::to_time_t(system_clock::now())){};           //
        JSONSTRUCT_REGISTER(__Qv2rayConfigObjectBase, F(displayName, creationDate, lastUpdatedDate))
    };

    struct SubscriptionConfigObject
    {
        QString address;
        float updateInterval;
        SubscriptionConfigObject() : address(""), updateInterval(10){};
        JSONSTRUCT_REGISTER(SubscriptionConfigObject, F(updateInterval, address))
    };

    struct GroupObject : __Qv2rayConfigObjectBase
    {
        QList<ConnectionId> connections;
        bool isSubscription;
        SubscriptionConfigObject subscriptionOption;
        GroupObject() : __Qv2rayConfigObjectBase(), connections(), isSubscription(false), subscriptionOption(){};
        JSONSTRUCT_REGISTER(GroupObject, F(connections, isSubscription, subscriptionOption), B(__Qv2rayConfigObjectBase))
    };

    struct ConnectionObject : __Qv2rayConfigObjectBase
    {
        qint64 lastConnected;
        qint64 latency;
        qint64 upLinkData;
        qint64 downLinkData;
        //
        int __qvConnectionRefCount;
        //
        ConnectionObject() : lastConnected(), latency(QVTCPING_VALUE_NODATA), upLinkData(0), downLinkData(0), __qvConnectionRefCount(0){};
        JSONSTRUCT_REGISTER(ConnectionObject, F(lastConnected, latency, upLinkData, downLinkData), B(__Qv2rayConfigObjectBase))
    };

    template<typename T>
    inline uint qHash(IDType<T> key)
    {
        return ::qHash(key.toString());
    }
    inline uint qHash(const Qv2ray::base::ConnectionGroupPair &pair)
    {
        return ::qHash(pair.connectionId.toString() + pair.groupId.toString());
    }
} // namespace Qv2ray::base

using namespace Qv2ray::base;
Q_DECLARE_METATYPE(ConnectionGroupPair);
