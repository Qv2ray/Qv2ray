#pragma once
#include "QJsonStruct.hpp"
#include "base/models/CoreObjectModels.hpp"
#include "plugin-interface/connections/ConnectionsBaseTypes.hpp"

namespace Qv2ray::base::config
{

    constexpr unsigned int LATENCY_TEST_VALUE_ERROR = 99999;
    constexpr unsigned int LATENCY_TEST_VALUE_NODATA = LATENCY_TEST_VALUE_ERROR - 1;
    using namespace std::chrono;

    struct __Qv2rayConfigObjectBase : public QObject
    {
        Q_OBJECT
        QJS_FUNCTION(__Qv2rayConfigObjectBase, F(displayName, creationDate, lastUpdatedDate))
        QJS_PROP(QString, displayName, REQUIRED)
        QJS_PROP_D(qint64, creationDate, system_clock::to_time_t(system_clock::now()))
        QJS_PROP_D(qint64, lastUpdatedDate, system_clock::to_time_t(system_clock::now()))
    };

    enum SubscriptionFilterRelation
    {
        RELATION_AND = 0,
        RELATION_OR = 1
    };

    struct SubscriptionConfigObject : public QObject
    {
        Q_OBJECT
        QJS_FUNCTION(SubscriptionConfigObject, F(updateInterval, address, type), F(includeRelation, excludeRelation, includeKeywords, excludeKeywords))
        QJS_PROP(QString, address)
        QJS_PROP_D(QString, type, "sip008")
        QJS_PROP_D(float, updateInterval, 10)
        QJS_PROP(QList<QString>, includeKeywords)
        QJS_PROP(QList<QString>, excludeKeywords)
        QJS_PROP_D(SubscriptionFilterRelation, includeRelation, RELATION_OR)
        QJS_PROP_D(SubscriptionFilterRelation, excludeRelation, RELATION_AND)
    };

    struct GroupObject : public __Qv2rayConfigObjectBase
    {
        Q_OBJECT
        QJS_FUNCTION(GroupObject, F(connections, isSubscription, routeConfigId, subscriptionOption), B(__Qv2rayConfigObjectBase))
        QJS_PROP_D(bool, isSubscription, false)
        QJS_PROP(QList<ConnectionId>, connections)
        QJS_PROP(GroupRoutingId, routeConfigId)
        QJS_PROP(SubscriptionConfigObject, subscriptionOption)
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
        QJS_FUNCTION(ConnectionStatsEntryObject, F(upLinkData, downLinkData))
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
        using ThisType = ConnectionObject;
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

    typedef objects::DNSObject DNSConfig;
    typedef objects::FakeDNSObject FakeDNSConfig;

    struct __RouteConfig_Impl : public QObject
    {
        Q_OBJECT
      public:
        __RouteConfig_Impl(const QList<QString> &_direct, const QList<QString> &_block, const QList<QString> &_proxy)
        {
            direct = _direct;
            block = _block;
            proxy = _proxy;
        }
        QJS_FUNCTION(__RouteConfig_Impl, F(proxy, block, direct))
        QJS_PROP(QList<QString>, direct)
        QJS_PROP(QList<QString>, block)
        QJS_PROP(QList<QString>, proxy)
    };

    struct RouteConfig : public QObject
    {
        Q_OBJECT
      public:
        RouteConfig(const __RouteConfig_Impl &_domains, const __RouteConfig_Impl &_ips, const QString &ds)
        {
            domainStrategy = ds;
            domains = _domains;
            ips = _ips;
        }
        QJS_FUNCTION(RouteConfig, F(domainStrategy, domainMatcher, domains, ips))
        QJS_PROP(QString, domainStrategy, REQUIRED)
        QJS_PROP_D(QString, domainMatcher, "mph", REQUIRED)
        QJS_PROP(__RouteConfig_Impl, domains, REQUIRED)
        QJS_PROP(__RouteConfig_Impl, ips, REQUIRED)
    };

    struct ForwardProxyConfig : public QObject
    {
        Q_OBJECT
      public:
        QJS_FUNCTION(ForwardProxyConfig, F(enableForwardProxy, type, serverAddress, port, useAuth, username, password))
        QJS_PROP_D(bool, enableForwardProxy, false)
        QJS_PROP_D(QString, type, "http")
        QJS_PROP(QString, serverAddress)
        QJS_PROP_D(int, port, 0)
        QJS_PROP_D(bool, useAuth, false)
        QJS_PROP(QString, username)
        QJS_PROP(QString, password)
    };

    struct ConnectionConfig : public QObject
    {
        Q_OBJECT
      public:
        QJS_FUNCTION(ConnectionConfig, F(bypassCN, bypassBT, bypassLAN, enableProxy, v2rayFreedomDNS, dnsIntercept))
        QJS_PROP_D(bool, enableProxy, true)
        QJS_PROP_D(bool, bypassCN, true)
        QJS_PROP_D(bool, bypassBT, false)
        QJS_PROP_D(bool, bypassLAN, true)
        QJS_PROP_D(bool, v2rayFreedomDNS, false)
        QJS_PROP_D(bool, dnsIntercept, false)
    };

    struct QvConfig_SystemProxy : public QObject
    {
        Q_OBJECT
      public:
        QJS_FUNCTION(QvConfig_SystemProxy, F(setSystemProxy))
        QJS_PROP_D(bool, setSystemProxy, true)
    };

    struct Qv2rayConfig_ProtocolInboundBase : public QObject
    {
        Q_OBJECT
      public:
        const QList<QString> default_destOverride{ "http", "tls" };
        QJS_FUNCTION(Qv2rayConfig_ProtocolInboundBase, F(port, useAuth, sniffing, destOverride, account, metadataOnly))
        QJS_PROP_D(int, port, 0)
        QJS_PROP_D(bool, useAuth, false)
        QJS_PROP_D(bool, sniffing, false)
        QJS_PROP_D(QList<QString>, destOverride, default_destOverride)
        QJS_PROP(objects::AccountObject, account)
        QJS_PROP(bool, metadataOnly)
    };

    struct QvConfig_SocksInbound : public Qv2rayConfig_ProtocolInboundBase
    {
        Q_OBJECT
      public:
        QvConfig_SocksInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 1089;
        }
        using ThisType = QvConfig_SocksInbound;
        QJS_PROP_D(bool, enableUDP, true)
        QJS_PROP_D(QString, localIP, "127.0.0.1")
        QJS_FUNC_COMP(QvConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
        QJS_FUNC_COPY(QvConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
        QJS_FUNC_JSON(QvConfig_SocksInbound, B(Qv2rayConfig_ProtocolInboundBase), F(enableUDP, localIP))
    };

    struct QvConfig_HttpInbound : public Qv2rayConfig_ProtocolInboundBase
    {
        Q_OBJECT
        using ThisType = QvConfig_HttpInbound;

      public:
        QvConfig_HttpInbound() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 8889;
        }
        QJS_FUNC_COMP(QvConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
        QJS_FUNC_COPY(QvConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
        QJS_FUNC_JSON(QvConfig_HttpInbound, B(Qv2rayConfig_ProtocolInboundBase))
    };

    struct QvConfig_TProxy : public Qv2rayConfig_ProtocolInboundBase
    {
        Q_OBJECT
        using ThisType = QvConfig_TProxy;

      public:
        QvConfig_TProxy() : Qv2rayConfig_ProtocolInboundBase()
        {
            port = 12345;
            sniffing = true;
        }
        QJS_PROP_D(QString, tProxyIP, "127.0.0.1")
        QJS_PROP_D(QString, tProxyV6IP, "::1")
        QJS_PROP_D(bool, hasTCP, true)
        QJS_PROP_D(bool, hasUDP, true)
        QJS_PROP_D(QString, mode, "tproxy")
        QJS_FUNC_COMP(QvConfig_TProxy, B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
        QJS_FUNC_COPY(QvConfig_TProxy, B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
        QJS_FUNC_JSON(QvConfig_TProxy, B(Qv2rayConfig_ProtocolInboundBase), F(tProxyIP, tProxyV6IP, hasTCP, hasUDP, mode))
    };

    struct QvConfig_Inbounds : public QObject
    {
        Q_OBJECT
        QJS_FUNCTION(QvConfig_Inbounds, F(listenip, useSocks, useHTTP, useTPROXY), F(tProxySettings, httpSettings, socksSettings, systemProxySettings))
        QJS_PROP_D(QString, listenip, "127.0.0.1")
        QJS_PROP_D(bool, useSocks, true)
        QJS_PROP_D(bool, useHTTP, true)
        QJS_PROP_D(bool, useTPROXY, false)
        QJS_PROP(QvConfig_TProxy, tProxySettings)
        QJS_PROP(QvConfig_HttpInbound, httpSettings)
        QJS_PROP(QvConfig_SocksInbound, socksSettings)
        QJS_PROP(QvConfig_SystemProxy, systemProxySettings)
    };

    struct QvConfig_Outbounds : public QObject
    {
        Q_OBJECT
      public:
        QJS_FUNCTION(QvConfig_Outbounds, F(mark))
        QJS_PROP_D(int, mark, 255)
    };

    struct GroupRoutingConfig : __Qv2rayConfigObjectBase
    {
        Q_OBJECT
        QJS_FUNCTION(GroupRoutingConfig, B(__Qv2rayConfigObjectBase), //
                     F(overrideRoute, routeConfig),                   //
                     F(overrideDNS, dnsConfig, fakeDNSConfig),        //
                     F(overrideConnectionConfig, connectionConfig),   //
                     F(overrideForwardProxyConfig, forwardProxyConfig))
        QJS_PROP_D(bool, overrideDNS, false)
        QJS_PROP(DNSConfig, dnsConfig)
        QJS_PROP(FakeDNSConfig, fakeDNSConfig)
        //
        QJS_PROP_D(bool, overrideRoute, false)
        QJS_PROP(RouteConfig, routeConfig)
        //
        QJS_PROP_D(bool, overrideConnectionConfig, false)
        QJS_PROP(ConnectionConfig, connectionConfig)
        //
        QJS_PROP_D(bool, overrideForwardProxyConfig, false)
        QJS_PROP(ForwardProxyConfig, forwardProxyConfig)
    };

} // namespace Qv2ray::base::config

Q_DECLARE_METATYPE(Qv2ray::base::config::QvStatsSpeed)
Q_DECLARE_METATYPE(Qv2ray::base::config::QvStatsData)
Q_DECLARE_METATYPE(Qv2ray::base::config::QvStatsSpeedData)
Q_DECLARE_METATYPE(Qv2ray::base::config::StatisticsType)
