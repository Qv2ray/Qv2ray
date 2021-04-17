#pragma once
#include "3rdparty/QJsonStruct/QJsonStruct.hpp"
#include "QvCoreSettings.hpp"

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
#define DECL_IDTYPE(type)                                                                                                                            \
    class __##type;                                                                                                                                  \
    typedef IDType<__##type> type

    DECL_IDTYPE(GroupId);
    DECL_IDTYPE(ConnectionId);
    DECL_IDTYPE(GroupRoutingId);

    inline const static ConnectionId NullConnectionId;
    inline const static GroupId NullGroupId;
    inline const static GroupRoutingId NullRoutingId;

    struct ConnectionGroupPair
    {
      public:
        ConnectionGroupPair(){};
        ConnectionGroupPair(const ConnectionGroupPair &another)
        {
            *this = another;
        }
        ConnectionGroupPair &operator=(const ConnectionGroupPair &another)
        {
            connectionId = another.connectionId;
            groupId = another.groupId;
            return *this;
        }
        friend bool operator==(const ConnectionGroupPair &one, const ConnectionGroupPair &another)
        {
            return one.connectionId == another.connectionId && one.groupId == another.groupId;
        }
        friend bool operator!=(const ConnectionGroupPair &one, const ConnectionGroupPair &another)
        {
            return !(one == another);
        }
        ConnectionGroupPair(const ConnectionId &conn, const GroupId &group)
        {
            connectionId = conn;
            groupId = group;
        }

        ConnectionId connectionId = NullConnectionId;
        GroupId groupId = NullGroupId;

        void clear()
        {
            connectionId = NullConnectionId;
            groupId = NullGroupId;
        }

      public:
        bool isEmpty() const
        {
            return groupId == NullGroupId || connectionId == NullConnectionId;
        }
        QJS_PLAIN_JSON(connectionId, groupId)
    };

    constexpr unsigned int LATENCY_TEST_VALUE_ERROR = 99999;
    constexpr unsigned int LATENCY_TEST_VALUE_NODATA = LATENCY_TEST_VALUE_ERROR - 1;
    using namespace std::chrono;

    struct __Qv2rayConfigObjectBase : public QObject
    {
        Q_OBJECT
        QJS_PROP(QString, displayName, REQUIRED)
        QJS_PROP_D(qint64, creationDate, system_clock::to_time_t(system_clock::now()))
        QJS_PROP_D(qint64, lastUpdatedDate, system_clock::to_time_t(system_clock::now()))
        QJS_FUNCTION(__Qv2rayConfigObjectBase, F(displayName, creationDate, lastUpdatedDate))
    };

    struct GroupRoutingConfig : __Qv2rayConfigObjectBase
    {
        Q_OBJECT
        QJS_PROP_D(bool, overrideDNS, false)
        QJS_PROP(config::QvConfig_DNS, dnsConfig)
        QJS_PROP(config::QvConfig_FakeDNS, fakeDNSConfig)
        //
        QJS_PROP_D(bool, overrideRoute, false)
        QJS_PROP(config::QvConfig_Route, routeConfig)
        //
        QJS_PROP_D(bool, overrideConnectionConfig, false)
        QJS_PROP(config::QvConfig_Connection, connectionConfig)
        //
        QJS_PROP_D(bool, overrideForwardProxyConfig, false)
        QJS_PROP(config::QvConfig_ForwardProxy, forwardProxyConfig)
        //
        QJS_FUNCTION(GroupRoutingConfig, B(__Qv2rayConfigObjectBase), F(overrideRoute, routeConfig), //
                     F(overrideDNS, dnsConfig, fakeDNSConfig),                                       //
                     F(overrideConnectionConfig, connectionConfig),                                  //
                     F(overrideForwardProxyConfig, forwardProxyConfig))
    };

    enum SubscriptionFilterRelation
    {
        RELATION_AND = 0,
        RELATION_OR = 1
    };

    struct SubscriptionConfigObject : public QObject
    {
        Q_OBJECT
        QJS_PROP(QString, address)
        QJS_PROP_D(QString, type, "sip008")
        QJS_PROP_D(float, updateInterval, 10)
        QJS_PROP(QList<QString>, IncludeKeywords)
        QJS_PROP(QList<QString>, ExcludeKeywords)
        QJS_PROP_D(SubscriptionFilterRelation, IncludeRelation, RELATION_OR)
        QJS_PROP_D(SubscriptionFilterRelation, ExcludeRelation, RELATION_AND)
        QJS_FUNCTION(SubscriptionConfigObject, F(updateInterval, address, type),
                     F(IncludeRelation, ExcludeRelation, IncludeKeywords, ExcludeKeywords))
    };

    struct GroupObject : public __Qv2rayConfigObjectBase
    {
        Q_OBJECT
        QJS_PROP_D(bool, isSubscription, false)
        QJS_PROP(QList<ConnectionId>, connections)
        QJS_PROP(GroupRoutingId, routeConfigId)
        QJS_PROP(SubscriptionConfigObject, subscriptionOption)
        QJS_FUNCTION(GroupObject, F(connections, isSubscription, routeConfigId, subscriptionOption), B(__Qv2rayConfigObjectBase))
    };

    enum StatisticsType
    {
        API_INBOUND = 0,
        API_OUTBOUND_PROXY = 1,
        API_OUTBOUND_DIRECT = 2,
        API_OUTBOUND_BLACKHOLE = 3,
    };

    typedef long qvspeed;
    typedef quint64 qvdata;
    typedef std::pair<qvspeed, qvspeed> QvStatsSpeed;
    typedef std::pair<qvdata, qvdata> QvStatsData;
    typedef std::pair<QvStatsSpeed, QvStatsData> QvStatsSpeedData;

    struct ConnectionStatsEntryObject : public QObject
    {
        Q_OBJECT
        QJS_PROP(qvdata, upLinkData)
        QJS_PROP(qvdata, downLinkData)
        QvStatsData toData()
        {
            return { upLinkData, downLinkData };
        }
        void fromData(const QvStatsData &d)
        {
            upLinkData = d.first;
            downLinkData = d.second;
        }
        QJS_FUNCTION(ConnectionStatsEntryObject, F(upLinkData, downLinkData))
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
                entries.append(ConnectionStatsEntryObject{});
            return entries[i];
        }
        ConnectionStatsEntryObject &get(StatisticsType i)
        {
            while (entries.count() <= i)
                entries.append(ConnectionStatsEntryObject{});
            return entries[i];
        }
        QJsonValue toJson() const
        {
            return JsonStructHelper::Serialize(entries);
        }
        friend bool operator==(const ConnectionStatsObject &left, const ConnectionStatsObject &right)
        {
            return left.toJson() == right.toJson();
        }
        friend bool operator!=(const ConnectionStatsObject &left, const ConnectionStatsObject &right)
        {
            return !(left == right);
        }
        void loadJson(const QJsonValue &d)
        {
            JsonStructHelper::Deserialize(entries, d);
        }
        void Clear()
        {
            entries.clear();
        }

      private:
        QList<ConnectionStatsEntryObject> entries;
    };

    struct ConnectionObject : public __Qv2rayConfigObjectBase
    {
        Q_OBJECT
        QJS_PROP(qint64, lastConnected)
        QJS_PROP_D(qint64, latency, LATENCY_TEST_VALUE_NODATA)
        QJS_PROP_D(ConnectionImportSource, importSource, IMPORT_SOURCE_MANUAL)
        QJS_PROP(ConnectionStatsObject, stats)
        int __qvConnectionRefCount = 0;

      public:
        ConnectionObject(const ConnectionObject &another)
        {
            *this = another;
        }
        ConnectionObject &operator=(const ConnectionObject &___another___instance__)
        {
            FOR_EACH(_QJS_COPY_BF, F(lastConnected, latency, importSource, stats), B(__Qv2rayConfigObjectBase));
            __qvConnectionRefCount = ___another___instance__.__qvConnectionRefCount;
            return *this;
        }
        QJS_FUNCTION_DEFAULT_CONSTRUCTOR(ConnectionObject, F(lastConnected, latency, importSource, stats), B(__Qv2rayConfigObjectBase))
        QJS_FUNC_JSON(ConnectionObject, F(lastConnected, latency, importSource, stats), B(__Qv2rayConfigObjectBase));
        QJS_FUNC_COMP(ConnectionObject, F(lastConnected, latency, importSource, stats), B(__Qv2rayConfigObjectBase));
    };

    struct ProtocolSettingsInfoObject
    {
        QString protocol;
        QString address;
        int port;
        ProtocolSettingsInfoObject(){};
        ProtocolSettingsInfoObject(const QString &_protocol, const QString _address, int _port)
            : protocol(_protocol), //
              address(_address),   //
              port(_port)          //
              {};
        QJS_PLAIN_JSON(protocol, address, port)
    };

    template<typename T>
    inline size_t qHash(IDType<T> key)
    {
        return ::qHash(key.toString());
    }
    inline size_t qHash(const Qv2ray::base::ConnectionGroupPair &pair)
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
