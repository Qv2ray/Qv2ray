#pragma once
#include "QvCoreSettings.hpp"
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
        const QString toString() const
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
    typedef IDType<__QvRoute> GroupRoutingId;
    //
    inline const static auto NullConnectionId = ConnectionId("null");
    inline const static auto NullGroupId = GroupId("null");
    inline const static auto NullRoutingId = GroupRoutingId("null");
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
            return groupId == NullGroupId || connectionId == NullConnectionId;
        }
        friend bool operator==(const ConnectionGroupPair &lhs, const ConnectionGroupPair &rhs)
        {
            return lhs.groupId == rhs.groupId && lhs.connectionId == rhs.connectionId;
        }
        JSONSTRUCT_REGISTER(ConnectionGroupPair, F(connectionId, groupId))
    };
    //
    constexpr unsigned int LATENCY_TEST_VALUE_ERROR = 99999;
    constexpr unsigned int LATENCY_TEST_VALUE_NODATA = LATENCY_TEST_VALUE_ERROR - 1;
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

    struct GroupRoutingConfig : __Qv2rayConfigObjectBase
    {
        bool overrideDNS;
        config::QvConfig_DNS dnsConfig;
        //
        bool overrideRoute;
        config::QvConfig_Route routeConfig;
        //
        bool overrideConnectionConfig;
        config::QvConfig_Connection connectionConfig;
        //
        bool overrideForwardProxyConfig;
        config::QvConfig_ForwardProxy forwardProxyConfig;
        //
        GroupRoutingConfig()
            : overrideDNS(false),               //
              overrideRoute(false),             //
              overrideConnectionConfig(false),  //
              overrideForwardProxyConfig(false) //
              {};
        JSONSTRUCT_REGISTER(GroupRoutingConfig,                            //
                            F(overrideRoute, routeConfig),                 //
                            F(overrideDNS, dnsConfig),                     //
                            F(overrideConnectionConfig, connectionConfig), //
                            F(overrideForwardProxyConfig, forwardProxyConfig))
    };

    enum SubscriptionFilterRelation
    {
        RELATION_AND = 0,
        RELATION_OR = 1
    };

    struct SubscriptionConfigObject
    {
        QString address;
        float updateInterval;
        SubscriptionFilterRelation IncludeRelation;
        SubscriptionFilterRelation ExcludeRelation;
        QList<QString> IncludeKeywords;
        QList<QString> ExcludeKeywords;
        SubscriptionConfigObject()
            : address(""), updateInterval(10),                             //
              IncludeRelation(RELATION_OR), ExcludeRelation(RELATION_AND), //
              IncludeKeywords(), ExcludeKeywords(){};
        JSONSTRUCT_REGISTER(SubscriptionConfigObject,
                            F(updateInterval, address, IncludeRelation, ExcludeRelation, IncludeKeywords, ExcludeKeywords))
    };

    struct GroupObject : __Qv2rayConfigObjectBase
    {
        QList<ConnectionId> connections;
        bool isSubscription;
        GroupRoutingId routeConfigId;
        SubscriptionConfigObject subscriptionOption;
        GroupObject() : __Qv2rayConfigObjectBase(), connections(), isSubscription(false), subscriptionOption(){};
        JSONSTRUCT_REGISTER(GroupObject, F(connections, isSubscription, routeConfigId, subscriptionOption), B(__Qv2rayConfigObjectBase))
    };

    enum StatisticsType
    {
        API_INBOUND = 0,
        API_OUTBOUND_PROXY = 1,
        API_OUTBOUND_DIRECT = 2,
        API_OUTBOUND_BLACKHOLE = 3,
    };

#define CurrentStatAPIType (GlobalConfig.uiConfig.graphConfig.useOutboundStats ? API_OUTBOUND_PROXY : API_INBOUND)

    typedef long qvspeed;
    typedef quint64 qvdata;
    typedef QPair<qvspeed, qvspeed> QvStatsSpeed;
    typedef QPair<qvdata, qvdata> QvStatsData;
    typedef QPair<QvStatsSpeed, QvStatsData> QvStatsSpeedData;

    struct ConnectionStatsEntryObject
    {
        qvdata upLinkData;
        qvdata downLinkData;
        QvStatsData toData()
        {
            return { upLinkData, downLinkData };
        }
        void fromData(const QvStatsData &d)
        {
            upLinkData = d.first;
            downLinkData = d.second;
        }
        JSONSTRUCT_REGISTER(ConnectionStatsEntryObject, F(upLinkData, downLinkData))
    };

    enum ConnectionImportSource
    {
        IMPORT_SOURCE_SUBSCRIPTION,
        IMPORT_SOURCE_MANUAL
    };

    struct ConnectionStatsObject
    {
        ConnectionStatsEntryObject &operator[](StatisticsType i)
        {
            while (entries.count() <= i)
            {
                entries.append(ConnectionStatsEntryObject{});
            }
            return entries[i];
        }
        QJsonValue toJson() const
        {
            return JsonStructHelper::___json_struct_store_data(entries);
        }
        void loadJson(const QJsonValue &d)
        {
            entries.clear();
            JsonStructHelper::___json_struct_load_data(entries, d);
        }
        void Clear()
        {
            entries.clear();
        }

      private:
        QList<ConnectionStatsEntryObject> entries;
    };

    struct ConnectionObject : __Qv2rayConfigObjectBase
    {
        qint64 lastConnected;
        qint64 latency;
        ConnectionImportSource importSource;
        ConnectionStatsObject stats;
        //
        int __qvConnectionRefCount;
        //
        ConnectionObject()
            : lastConnected(),                    //
              latency(LATENCY_TEST_VALUE_NODATA), //
              importSource(IMPORT_SOURCE_MANUAL), //
              stats(),                            //
              __qvConnectionRefCount(0)           //
              {};
        JSONSTRUCT_REGISTER(ConnectionObject, F(lastConnected, latency, importSource, stats), B(__Qv2rayConfigObjectBase))
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
Q_DECLARE_METATYPE(ConnectionGroupPair)
Q_DECLARE_METATYPE(ConnectionId)
Q_DECLARE_METATYPE(GroupId)
Q_DECLARE_METATYPE(GroupRoutingId)

Q_DECLARE_METATYPE(QvStatsSpeed)
Q_DECLARE_METATYPE(QvStatsData)
Q_DECLARE_METATYPE(QvStatsSpeedData)
Q_DECLARE_METATYPE(StatisticsType)
