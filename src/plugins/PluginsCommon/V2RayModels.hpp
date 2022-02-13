#pragma once

#include "QvPlugin/Common/CommonTypes.hpp"
#include "QvPlugin/Utils/BindableProps.hpp"

namespace Qv2ray::Models
{
    struct RouteMatrixConfig
    {
        const static inline auto EXTRA_OPTIONS_ID = u"RouteMatrixConfig"_qs;
        struct Detail
        {
            Detail(const QList<QString> &_direct = {}, const QList<QString> &_block = {}, const QList<QString> &_proxy = {})
            {
                direct = _direct;
                block = _block;
                proxy = _proxy;
            }
            Bindable<QList<QString>> direct;
            Bindable<QList<QString>> block;
            Bindable<QList<QString>> proxy;
            QJS_COMPARE(Detail, direct, block, proxy);
            QJS_JSON(P(proxy, block, direct))
        };

        RouteMatrixConfig(const Detail &_domains = {}, const Detail &_ips = {}, const QString &ds = {})
        {
            domainStrategy = ds;
            domains = _domains;
            ips = _ips;
        }
        Bindable<QString> domainStrategy;
        Bindable<QString> domainMatcher{ u"mph"_qs };
        Bindable<Detail> domains;
        Bindable<Detail> ips;
        QJS_COMPARE(RouteMatrixConfig, domainStrategy, domainMatcher, domains, ips);
        QJS_JSON(P(domainStrategy, domainMatcher, domains, ips))
        static auto fromJson(const QJsonObject &o)
        {
            RouteMatrixConfig route;
            route.loadJson(o);
            return route;
        }
    };

    // We don't use any of the inheritance features, just here to ensure the Json level compatibility.
    struct V2RayDNSObject : public BasicDNSObject
    {
        enum QueryStrategy
        {
            UseIP,
            UseIPv4,
            UseIPv6,
        };

        // Same as above.
        struct V2RayDNSServerObject : public BasicDNSServerObject
        {
            Bindable<bool> QV2RAY_DNS_IS_COMPLEX_DNS{ false };
            Bindable<bool> SkipFallback{ false };
            Bindable<QList<QString>> domains;
            Bindable<QList<QString>> expectIPs;
            QJS_COMPARE(V2RayDNSServerObject, QV2RAY_DNS_IS_COMPLEX_DNS, SkipFallback, port, address, domains, expectIPs);
            QJS_JSON(P(QV2RAY_DNS_IS_COMPLEX_DNS, SkipFallback), F(address, port, domains, expectIPs))
        };

        Bindable<QList<V2RayDNSServerObject>> servers;
        Bindable<QString> clientIp;
        Bindable<QString> tag;
        Bindable<bool> disableCache{ false };
        Bindable<bool> disableFallback{ false };
        Bindable<QString> queryStrategy{ u"UseIP"_qs };
        QJS_COMPARE(V2RayDNSObject, servers, clientIp, tag, disableCache, disableFallback, queryStrategy, servers, hosts, extraOptions);
        QJS_JSON(P(clientIp, tag, disableCache, disableFallback, queryStrategy), F(servers, hosts, extraOptions));
        static auto fromJson(const QJsonObject &o)
        {
            V2RayDNSObject dns;
            dns.loadJson(o);
            return dns;
        }
    };

    struct V2RayFakeDNSObject
    {
        Bindable<QString> ipPool;
        Bindable<int> poolSize;
        QJS_COMPARE(V2RayFakeDNSObject, ipPool, poolSize)
        QJS_JSON(F(ipPool, poolSize))
        static auto fromJson(const QJsonObject &o)
        {
            V2RayFakeDNSObject dns;
            dns.loadJson(o);
            return dns;
        }
    };

    namespace transfer
    {
        struct HTTPRequestObject
        {
            Bindable<QString> version{ u"1.1"_qs };
            Bindable<QString> method{ u"GET"_qs };
            Bindable<QList<QString>> path{ { u"/"_qs } };
            Bindable<QMap<QString, QStringList>> headers{ {
                { u"Host"_qs, { u"www.baidu.com"_qs, u"www.bing.com"_qs } },
                { u"User-Agent"_qs,
                  { u"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"_qs,
                    u"Mozilla/5.0 (iPhone; CPU iPhone OS 10_0_2 like Mac OS X) AppleWebKit/601.1 (KHTML, like Gecko) CriOS/53.0.2785.109 Mobile/14A456 Safari/601.1.46"_qs } },
                { u"Accept-Encoding"_qs, { u"gzip, deflate"_qs } },
                { u"Connection"_qs, { u"keep-alive"_qs } },
                { u"Pragma"_qs, { u"no-cache"_qs } },
            } };
            QJS_COMPARE(HTTPRequestObject, version, method, path, headers)
            QJS_JSON(P(version, method, path, headers))
        };

        struct HTTPResponseObject
        {
            Bindable<QString> version{ u"1.1"_qs };
            Bindable<QString> status{ u"200"_qs };
            Bindable<QString> reason{ u"OK"_qs };
            Bindable<QMap<QString, QStringList>> headers{ {
                { u"Content-Type"_qs, { u"application/octet-stream"_qs, u"video/mpeg"_qs } },
                { u"Transfer-Encoding"_qs, { u"chunked"_qs } },
                { u"Connection"_qs, { u"keep-alive"_qs } },
                { u"Pragma"_qs, { u"no-cache"_qs } },
            } };
            QJS_COMPARE(HTTPResponseObject, version, reason, status, status)
            QJS_JSON(P(version, reason, status, status))
        };

        struct TCPHeader_Internal
        {
            Bindable<QString> type{ u"none"_qs };
            Bindable<HTTPRequestObject> request;
            Bindable<HTTPResponseObject> response;
            QJS_COMPARE(TCPHeader_Internal, type, request, response)
            QJS_JSON(P(type, request, response))
        };

        struct ObfsHeaderObject
        {
            Bindable<QString> type{ u"none"_qs };
            QJS_COMPARE(ObfsHeaderObject, type);
            QJS_JSON(P(type))
        };

        struct TCPObject
        {
            Bindable<TCPHeader_Internal> header;
            QJS_COMPARE(TCPObject, header);
            QJS_JSON(P(header))
        };

        struct KCPObject
        {
            Bindable<int> mtu{ 1350 };
            Bindable<int> tti{ 50 };
            Bindable<int> uplinkCapacity{ 5 };
            Bindable<int> downlinkCapacity{ 20 };
            Bindable<bool> congestion{ false };
            Bindable<int> readBufferSize{ 2 };
            Bindable<int> writeBufferSize{ 2 };
            Bindable<QString> seed;
            Bindable<ObfsHeaderObject> header;
            QJS_COMPARE(KCPObject, mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, seed, header);
            QJS_JSON(P(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, seed, header))
        };

        struct WebSocketObject
        {
            Bindable<QString> path{ u"/"_qs };
            Bindable<QMap<QString, QString>> headers;
            Bindable<int> maxEarlyData{ 0 };
            Bindable<bool> useBrowserForwarding{ false };
            Bindable<QString> earlyDataHeaderName;
            QJS_COMPARE(WebSocketObject, path, headers, maxEarlyData, useBrowserForwarding, useBrowserForwarding);
            QJS_JSON(P(path, headers, maxEarlyData, useBrowserForwarding, useBrowserForwarding))
        };

        struct HttpObject
        {
            Bindable<QList<QString>> host;
            Bindable<QString> path{ u"/"_qs };
            Bindable<QString> method;
            QJS_COMPARE(HttpObject, host, path, method);
            QJS_JSON(P(host, path, method))
        };

        struct DomainSocketObject
        {
            Bindable<QString> path{ u"/"_qs };
            Bindable<bool> abstract{ false };
            Bindable<bool> padding{ false };
            QJS_COMPARE(DomainSocketObject, path, abstract, padding);
            QJS_JSON(P(path, abstract, padding))
        };

        struct QuicObject
        {
            Bindable<QString> security{ u"none"_qs };
            Bindable<QString> key;
            Bindable<ObfsHeaderObject> header;
            QJS_COMPARE(QuicObject, security, key, header);
            QJS_JSON(P(security, key, header))
        };

        struct gRPCObject
        {
            Bindable<QString> serviceName{ u"GunService"_qs };
            QJS_COMPARE(gRPCObject, serviceName);
            QJS_JSON(P(serviceName))
        };

        struct SockoptObject
        {
            Bindable<int> mark{ 255 };
            Bindable<int> tcpKeepAliveInterval{ 0 };
            Bindable<bool> tcpFastOpen{ false };
            Bindable<QString> tproxy{ u"off"_qs };
            QJS_COMPARE(SockoptObject, mark, tcpKeepAliveInterval, tcpFastOpen, tproxy);
            QJS_JSON(P(mark, tcpKeepAliveInterval, tcpFastOpen, tproxy))
        };

        struct CertificateObject
        {
            Bindable<QString> usage{ u"encipherment"_qs };
            Bindable<QString> certificateFile;
            Bindable<QString> keyFile;
            Bindable<QList<QString>> certificate;
            Bindable<QList<QString>> key;
            QJS_COMPARE(CertificateObject, usage, certificateFile, keyFile, certificate, key);
            QJS_JSON(P(usage, certificateFile, keyFile, certificate, key))
        };

        struct TLSObject
        {
            Bindable<QString> serverName;
            Bindable<bool> disableSessionResumption{ true };
            Bindable<bool> disableSystemRoot{ false };
            Bindable<QList<QString>> alpn;
            Bindable<QList<CertificateObject>> certificates;
            QJS_COMPARE(TLSObject, serverName, disableSessionResumption, disableSystemRoot, alpn, certificates);
            QJS_JSON(P(serverName, disableSessionResumption, disableSystemRoot, alpn, certificates))
        };
    } // namespace transfer

    struct StreamSettingsObject
    {
        Bindable<QString> network{ u"tcp"_qs };
        Bindable<QString> security{ u"none"_qs };
        Bindable<transfer::SockoptObject> sockopt;
        Bindable<transfer::TLSObject> tlsSettings;
        Bindable<transfer::TCPObject> tcpSettings;
        Bindable<transfer::HttpObject> httpSettings;
        Bindable<transfer::WebSocketObject> wsSettings;
        Bindable<transfer::KCPObject> kcpSettings;
        Bindable<transfer::DomainSocketObject> dsSettings;
        Bindable<transfer::QuicObject> quicSettings;
        Bindable<transfer::gRPCObject> grpcSettings;
        QJS_COMPARE(StreamSettingsObject, network, security, sockopt, tlsSettings, tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings,
                    grpcSettings);
        QJS_JSON(P(network, security, sockopt, tlsSettings, tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings, grpcSettings))
        static inline auto fromJson(const QJsonObject &o)
        {
            StreamSettingsObject stream;
            stream.loadJson(o);
            return stream;
        };

        inline operator IOStreamSettings()
        {
            return IOStreamSettings{ this->toJson() };
        }
    };

    struct HTTPSOCKSObject
    {
        Bindable<QString> user;
        Bindable<QString> pass;
        Bindable<int> level{ 0 };
        QJS_JSON(P(user, pass, level))
    };

    // ShadowSocks Server
    struct ShadowSocksClientObject
    {
        Bindable<QString> method{ u"aes-256-gcm"_qs };
        Bindable<QString> password;
        QJS_JSON(P(method, password))
    };

    // VMess Server
    struct VMessClientObject
    {
        Bindable<QString> id;
        Bindable<QString> experiments;
        Bindable<QString> security{ u"auto"_qs };
        QJS_JSON(F(id, security, experiments))
    };

    struct TrojanClientObject
    {
        Bindable<QString> password;
        QJS_JSON(F(password))
    };
} // namespace Qv2ray::Models
