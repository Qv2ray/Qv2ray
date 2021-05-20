#pragma once
#include "3rdparty/QJsonStruct/QJsonIO.hpp"
#include "3rdparty/QJsonStruct/QJsonStruct.hpp"

#include <QList>
#include <QMap>
#include <QString>

namespace Qv2ray::base::objects
{
    struct DNSObject
    {
        struct DNSServerObject
        {
            bool QV2RAY_DNS_IS_COMPLEX_DNS;
            QString address;
            int port;
            QList<QString> domains;
            QList<QString> expectIPs;
            DNSServerObject() : QV2RAY_DNS_IS_COMPLEX_DNS(false), port(53){};
            DNSServerObject(const QString &_address) : DNSServerObject()
            {
                address = _address;
            };

            friend bool operator==(const DNSServerObject &left, const DNSServerObject &right)
            {
                return left.QV2RAY_DNS_IS_COMPLEX_DNS == right.QV2RAY_DNS_IS_COMPLEX_DNS && //
                       left.address == right.address &&                                     //
                       left.port == right.port &&                                           //
                       left.domains == right.domains &&                                     //
                       left.expectIPs == right.expectIPs;
            }
            friend bool operator!=(const DNSServerObject &left, const DNSServerObject &right)
            {
                return !(left == right);
            }
            void loadJson(const QJsonValue &___json_object_)
            {
                DNSServerObject ___qjsonstruct_default_check;
                // Hack to convert simple DNS settings to complex format.
                if (___json_object_.isString())
                {
                    address = ___json_object_.toString();
                    QV2RAY_DNS_IS_COMPLEX_DNS = false;
                    return;
                }
                FOREACH_CALL_FUNC(___DESERIALIZE_FROM_JSON_EXTRACT_B_F, F(QV2RAY_DNS_IS_COMPLEX_DNS, address, port, domains, expectIPs));
            }
            [[nodiscard]] static auto fromJson(const QJsonValue &___json_object_)
            {
                DNSServerObject _t;
                _t.loadJson(___json_object_);
                return _t;
            }
            [[nodiscard]] const QJsonObject toJson() const
            {
                QJsonObject ___json_object_;
                DNSServerObject ___qjsonstruct_default_check;
                FOREACH_CALL_FUNC(___SERIALIZE_TO_JSON_EXTRACT_B_F, F(QV2RAY_DNS_IS_COMPLEX_DNS, address, port, domains, expectIPs));
                return ___json_object_;
            }
        };

        QMap<QString, QString> hosts;
        QList<DNSServerObject> servers;
        QString clientIp;
        QString tag;
        bool disableCache = false;
        bool disableFallback = false;
        QString queryStrategy = "UseIP";
        friend bool operator==(const DNSObject &left, const DNSObject &right)
        {
            return left.hosts == right.hosts &&                     //
                   left.servers == right.servers &&                 //
                   left.clientIp == right.clientIp &&               //
                   left.tag == right.tag &&                         //
                   left.disableCache == right.disableCache &&       //
                   left.disableFallback == right.disableFallback && //
                   left.queryStrategy == right.queryStrategy;
        }
        friend bool operator!=(const DNSObject &left, const DNSObject &right)
        {
            return !(left == right);
        }
        JSONSTRUCT_REGISTER(DNSObject, F(hosts, servers, clientIp, tag, disableCache, disableFallback, queryStrategy))
    };
    //
    // Used in config generation
    struct AccountObject
    {
        QString user;
        QString pass;
        JSONSTRUCT_COMPARE(AccountObject, user, pass)
        JSONSTRUCT_REGISTER(AccountObject, F(user, pass))
    };
    //
    //
    struct RuleObject
    {
        bool QV2RAY_RULE_ENABLED = true;
        QString QV2RAY_RULE_TAG = "New Rule";
        //
        QString type = "field";
        QList<QString> inboundTag;
        QString outboundTag;
        QString balancerTag;
        // Addresses
        QList<QString> source;
        QList<QString> domain;
        QList<QString> ip;
        // Ports
        QString sourcePort;
        QString port;
        //
        QString network;
        QList<QString> protocol;
        QString attrs;
        JSONSTRUCT_COMPARE(RuleObject, type, outboundTag, balancerTag, //
                           QV2RAY_RULE_ENABLED, QV2RAY_RULE_TAG,       //
                           domain, ip, port, sourcePort, network, source, inboundTag, protocol, attrs)
        JSONSTRUCT_REGISTER(RuleObject,                              //
                            A(type, outboundTag, balancerTag),       //
                            A(QV2RAY_RULE_ENABLED, QV2RAY_RULE_TAG), //
                            F(domain, ip, port, sourcePort, network, source, inboundTag, protocol, attrs))
    };
    //
    //

    struct StrategyObject
    {
        QString type;
        JSONSTRUCT_COMPARE(StrategyObject, type)
        JSONSTRUCT_REGISTER(StrategyObject, F(type))
    };

    struct BalancerObject
    {
        QString tag;
        QList<QString> selector;
        StrategyObject strategy;
        JSONSTRUCT_COMPARE(BalancerObject, tag, selector, strategy)
        JSONSTRUCT_REGISTER(BalancerObject, F(tag, selector, strategy))
    };
    //
    //
    namespace transfer
    {
        struct HTTPRequestObject
        {
            QString version = "1.1";
            QString method = "GET";
            QList<QString> path = { "/" };
            QMap<QString, QList<QString>> headers;
            HTTPRequestObject()
            {
                headers = {
                    { "Host", { "www.baidu.com", "www.bing.com" } },
                    { "User-Agent",
                      { "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
                        "Mozilla/5.0 (iPhone; CPU iPhone OS 10_0_2 like Mac OS X) AppleWebKit/601.1 (KHTML, like Gecko) CriOS/53.0.2785.109 Mobile/14A456 Safari/601.1.46" } },
                    { "Accept-Encoding", { "gzip, deflate" } },
                    { "Connection", { "keep-alive" } },
                    { "Pragma", { "no-cache" } }
                };
            }
            JSONSTRUCT_COMPARE(HTTPRequestObject, version, method, path, headers)
            JSONSTRUCT_REGISTER(HTTPRequestObject, F(version, method, path, headers))
        };
        //
        //
        struct HTTPResponseObject
        {
            QString version = "1.1";
            QString status = "200";
            QString reason = "OK";
            QMap<QString, QList<QString>> headers;
            HTTPResponseObject()
            {
                headers = { { "Content-Type", { "application/octet-stream", "video/mpeg" } }, //
                            { "Transfer-Encoding", { "chunked" } },                           //
                            { "Connection", { "keep-alive" } },                               //
                            { "Pragma", { "no-cache" } } };
            }
            JSONSTRUCT_COMPARE(HTTPResponseObject, version, status, reason, headers)
            JSONSTRUCT_REGISTER(HTTPResponseObject, F(version, status, reason, headers))
        };
        //
        //
        struct TCPHeader_Internal
        {
            QString type = "none";
            HTTPRequestObject request;
            HTTPResponseObject response;
            JSONSTRUCT_COMPARE(TCPHeader_Internal, type, request, response)
            JSONSTRUCT_REGISTER(TCPHeader_Internal, A(type), F(request, response))
        };
        //
        //
        struct ObfsHeaderObject
        {
            QString type = "none";
            JSONSTRUCT_COMPARE(ObfsHeaderObject, type)
            JSONSTRUCT_REGISTER(ObfsHeaderObject, F(type))
        };
        //
        //
        struct TCPObject
        {
            TCPHeader_Internal header;
            JSONSTRUCT_COMPARE(TCPObject, header)
            JSONSTRUCT_REGISTER(TCPObject, F(header))
        };
        //
        //
        struct KCPObject
        {
            int mtu = 1350;
            int tti = 50;
            int uplinkCapacity = 5;
            int downlinkCapacity = 20;
            bool congestion = false;
            int readBufferSize = 2;
            int writeBufferSize = 2;
            QString seed;
            ObfsHeaderObject header;
            KCPObject(){};
            JSONSTRUCT_COMPARE(KCPObject, mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, seed, header)
            JSONSTRUCT_REGISTER(KCPObject, F(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header, seed))
        };
        //
        //
        struct WebSocketObject
        {
            QString path = "/";
            QMap<QString, QString> headers;
            int maxEarlyData = 0;
            bool useBrowserForwarding = false;
            QString earlyDataHeaderName;
            JSONSTRUCT_COMPARE(WebSocketObject, path, headers, maxEarlyData, useBrowserForwarding, earlyDataHeaderName)
            JSONSTRUCT_REGISTER(WebSocketObject, F(path, headers, maxEarlyData, useBrowserForwarding, earlyDataHeaderName))
        };
        //
        //
        struct HttpObject
        {
            QList<QString> host;
            QString path = "/";
            QString method = "PUT";
            QMap<QString, QList<QString>> headers;
            JSONSTRUCT_COMPARE(HttpObject, host, path, method, headers)
            JSONSTRUCT_REGISTER(HttpObject, F(host, path, method, headers))
        };
        //
        //
        struct DomainSocketObject
        {
            QString path = "/";
            JSONSTRUCT_COMPARE(DomainSocketObject, path)
            JSONSTRUCT_REGISTER(DomainSocketObject, F(path))
        };
        //
        //
        struct QuicObject
        {
            QString security = "none";
            QString key;
            ObfsHeaderObject header;
            JSONSTRUCT_COMPARE(QuicObject, security, key, header)
            JSONSTRUCT_REGISTER(QuicObject, F(security, key, header))
        };
        //
        //
        struct gRPCObject
        {
            QString serviceName;
            bool multiMode = false;
            JSONSTRUCT_COMPARE(gRPCObject, serviceName, multiMode)
            JSONSTRUCT_REGISTER(gRPCObject, F(serviceName, multiMode))
        };

        //
        //
        struct SockoptObject
        {
            int mark = 0;
            bool tcpFastOpen = false;
            QString tproxy = "off";
            int tcpKeepAliveInterval = 0;
            JSONSTRUCT_COMPARE(SockoptObject, mark, tcpFastOpen, tproxy, tcpKeepAliveInterval)
            JSONSTRUCT_REGISTER(SockoptObject, F(mark, tcpFastOpen, tproxy, tcpKeepAliveInterval))
        };
        //
        //
        struct CertificateObject
        {
            QString usage = "encipherment";
            QString certificateFile;
            QString keyFile;
            QList<QString> certificate;
            QList<QString> key;
            JSONSTRUCT_COMPARE(CertificateObject, usage, certificateFile, keyFile, certificate, key)
            JSONSTRUCT_REGISTER(CertificateObject, F(usage, certificateFile, keyFile, certificate, key))
        };
        //
        //
        struct TLSObject
        {
            QString serverName;
            bool allowInsecure = false;
            bool enableSessionResumption = false;
            bool disableSystemRoot = false;
            QList<QString> alpn;
            QList<QString> pinnedPeerCertificateChainSha256;
            QList<CertificateObject> certificates;
            JSONSTRUCT_COMPARE(TLSObject, serverName, allowInsecure, enableSessionResumption, disableSystemRoot, alpn,
                               pinnedPeerCertificateChainSha256, certificates)
            JSONSTRUCT_REGISTER(TLSObject, F(serverName, allowInsecure, enableSessionResumption, disableSystemRoot, alpn,
                                             pinnedPeerCertificateChainSha256, certificates))
        };
        //
        //
        struct XTLSObject
        {
            QString serverName;
            bool allowInsecure = false;
            bool enableSessionResumption = false;
            bool disableSystemRoot = false;
            QList<QString> alpn;
            QList<CertificateObject> certificates;
            JSONSTRUCT_COMPARE(XTLSObject, serverName, allowInsecure, enableSessionResumption, disableSystemRoot, alpn, certificates)
            JSONSTRUCT_REGISTER(XTLSObject, F(serverName, allowInsecure, enableSessionResumption, disableSystemRoot, alpn, certificates))
        };
    } // namespace transfer
    //
    //
    struct StreamSettingsObject
    {
        QString network = "tcp";
        QString security = "none";
        transfer::SockoptObject sockopt;
        transfer::TLSObject tlsSettings;
        transfer::XTLSObject xtlsSettings;
        transfer::TCPObject tcpSettings;
        transfer::KCPObject kcpSettings;
        transfer::WebSocketObject wsSettings;
        transfer::HttpObject httpSettings;
        transfer::DomainSocketObject dsSettings;
        transfer::QuicObject quicSettings;
        transfer::gRPCObject grpcSettings;
        JSONSTRUCT_COMPARE(StreamSettingsObject, network, security, sockopt, //
                           tcpSettings, tlsSettings, xtlsSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings, grpcSettings)
        JSONSTRUCT_REGISTER(StreamSettingsObject, F(network, security, sockopt),
                            F(tcpSettings, tlsSettings, xtlsSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings, grpcSettings))
    };

    struct FakeDNSObject
    {
        QString ipPool = "198.18.0.0/15";
        int poolSize = 65535;
        JSONSTRUCT_REGISTER(FakeDNSObject, A(ipPool, poolSize))
        JSONSTRUCT_COMPARE(FakeDNSObject, ipPool, poolSize)
    };

    //
    // Some protocols from: https://v2ray.com/chapter_02/02_protocols.html
    namespace protocol
    {
        //
        // VMess Server
        constexpr auto VMESS_USER_ALTERID_DEFAULT = 0;
        struct VMessServerObject
        {
            struct UserObject
            {
                QString id;
                int alterId = VMESS_USER_ALTERID_DEFAULT;
                QString security = "auto";
                int level = 0;
                JSONSTRUCT_COMPARE(UserObject, id, alterId, security, level)
                JSONSTRUCT_REGISTER(UserObject, F(id, alterId, security, level))
            };

            QString address;
            int port;
            QList<UserObject> users;
            JSONSTRUCT_COMPARE(VMessServerObject, address, port, users)
            JSONSTRUCT_REGISTER(VMessServerObject, F(address, port, users))
        };
        //
        // ShadowSocks Server
        struct ShadowSocksServerObject
        {
            QString address;
            QString method;
            QString password;
            int port;
            JSONSTRUCT_COMPARE(ShadowSocksServerObject, address, method, password)
            JSONSTRUCT_REGISTER(ShadowSocksServerObject, F(address, port, method, password))
        };
    } // namespace protocol
} // namespace Qv2ray::base::objects
