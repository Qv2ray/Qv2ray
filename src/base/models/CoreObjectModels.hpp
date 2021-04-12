#pragma once
#include "3rdparty/QJsonStruct/QJsonIO.hpp"
#include "3rdparty/QJsonStruct/QJsonStruct.hpp"

#include <QList>
#include <QMap>
#include <QString>

namespace Qv2ray::base::objects
{
    struct DNSServerObject
    {
        Q_GADGET
        QJS_CONSTRUCTOR(DNSServerObject)

        QJS_PROP_D(bool, QV2RAY_DNS_IS_COMPLEX_DNS, false)
        QJS_PROP_D(int, port, 53)
        QJS_PROP(QString, address)
        QJS_PROP(QList<QString>, domains)
        QJS_PROP(QList<QString>, expectIPs)

        DNSServerObject(const QString &_address) : DNSServerObject()
        {
            address = _address;
        }

        QJS_FUNC_COMPARE(F(QV2RAY_DNS_IS_COMPLEX_DNS, address, port, domains, expectIPs))

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
            FOR_EACH(_QJS_FROM_JSON_F, QV2RAY_DNS_IS_COMPLEX_DNS, address, port, domains, expectIPs);
        }
        [[nodiscard]] static auto fromJson(const QJsonValue &json)
        {
            DNSServerObject _t;
            _t.loadJson(json);
            return _t;
        }
        [[nodiscard]] const QJsonObject toJson() const
        {
            QJsonObject ___json_object_;
            FOR_EACH(_QJS_TO_JSON_F, QV2RAY_DNS_IS_COMPLEX_DNS, address, port, domains, expectIPs);
            return ___json_object_;
        }
    };

    struct DNSObject
    {
        typedef QMap<QString, QString> QStringStringMap;

        Q_GADGET
        QJS_CONSTRUCTOR(DNSObject)
        QJS_PROP(QStringStringMap, hosts)
        QJS_PROP(QList<DNSServerObject>, servers)
        QJS_PROP(QString, clientIp)
        QJS_PROP(QString, tag)
        QJS_PROP_D(bool, disableCache, false)
        QJS_PROP_D(bool, disableFallback, false)
        QJS_PROP_D(QString, queryStrategy, "UseIP")
        QJS_FUNCTION(F(hosts, servers, clientIp, tag, disableCache, disableFallback, queryStrategy))
    };

    // Used in config generation
    struct AccountObject
    {
        Q_GADGET
        QJS_CONSTRUCTOR(AccountObject)
        QJS_PROP(QString, user);
        QJS_PROP(QString, pass);
        QJS_FUNCTION(F(user, pass))
    };

    struct RuleObject
    {
        Q_GADGET
        QJS_CONSTRUCTOR(RuleObject)
        QJS_PROP_D(bool, QV2RAY_RULE_ENABLED, true, REQUIRED)
        QJS_PROP_D(QString, QV2RAY_RULE_TAG, "New Rule", REQUIRED)
        //
        QJS_PROP_D(QString, type, "field", REQUIRED);
        QJS_PROP(QList<QString>, inboundTag);
        QJS_PROP(QString, outboundTag, REQUIRED);
        QJS_PROP(QString, balancerTag, REQUIRED);
        // Addresses
        QJS_PROP(QList<QString>, source)
        QJS_PROP(QList<QString>, domain)
        QJS_PROP(QList<QString>, ip)
        // Ports
        QJS_PROP(QString, sourcePort)
        QJS_PROP(QString, port)
        //
        QJS_PROP(QString, network)
        QJS_PROP(QList<QString>, protocol)
        QJS_PROP(QString, attrs)
        QJS_FUNCTION(F(type, outboundTag, balancerTag, QV2RAY_RULE_ENABLED, QV2RAY_RULE_TAG), //
                     F(domain, ip, port, sourcePort, network, source, inboundTag, protocol, attrs))
    };

    struct BalancerObject
    {
        Q_GADGET
        QJS_CONSTRUCTOR(BalancerObject)
        QJS_PROP(QString, tag);
        QJS_PROP(QList<QString>, selector)
        QJS_FUNCTION(F(tag, selector))
    };

    namespace transfer
    {
        struct HTTPRequestObject
        {
            Q_GADGET
            typedef QMap<QString, QList<QString>> QString_ListString_Map;

            QString_ListString_Map headers_init = {
                { "Host", { "www.baidu.com", "www.bing.com" } },
                { "User-Agent",
                  { "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
                    "Mozilla/5.0 (iPhone; CPU iPhone OS 10_0_2 like Mac OS X) AppleWebKit/601.1 (KHTML, like Gecko) CriOS/53.0.2785.109 Mobile/14A456 Safari/601.1.46" } },
                { "Accept-Encoding", { "gzip, deflate" } },
                { "Connection", { "keep-alive" } },
                { "Pragma", { "no-cache" } }
            };

            QJS_CONSTRUCTOR(HTTPRequestObject)
            QJS_PROP_D(QString, version, "1.1")
            QJS_PROP_D(QString, method, "GET")
            QJS_PROP_D(QList<QString>, path, "/")
            QJS_PROP_D(QString_ListString_Map, headers, headers_init)
            QJS_FUNCTION(F(version, method, path, headers))
        };

        struct HTTPResponseObject
        {
            Q_GADGET
            typedef QMap<QString, QList<QString>> QString_ListString_Map;

            QJS_CONSTRUCTOR(HTTPResponseObject)

            QString_ListString_Map headers_init{ { "Content-Type", { "application/octet-stream", "video/mpeg" } }, //
                                                 { "Transfer-Encoding", { "chunked" } },                           //
                                                 { "Connection", { "keep-alive" } },                               //
                                                 { "Pragma", { "no-cache" } } };
            QJS_PROP_D(QString, version, "1.1")
            QJS_PROP_D(QString, status, "200")
            QJS_PROP_D(QString, reason, "OK")
            QJS_PROP_D(QString_ListString_Map, headers, headers_init);

            QJS_FUNCTION(F(version, status, reason, headers))
        };

        struct TCPHeader_Internal
        {
            Q_GADGET
            QJS_CONSTRUCTOR(TCPHeader_Internal)
            QJS_PROP_D(QString, type, "none", REQUIRED);
            QJS_PROP(HTTPRequestObject, request);
            QJS_PROP(HTTPResponseObject, response);
            QJS_FUNCTION(F(type, request, response))
        };

        struct ObfsHeaderObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(ObfsHeaderObject)
            QJS_PROP_D(QString, type, "none");
            QJS_FUNCTION(F(type))
        };

        struct TCPObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(TCPObject)
            QJS_PROP(TCPHeader_Internal, header)
            QJS_FUNCTION(F(header))
        };

        struct KCPObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(KCPObject)
            QJS_PROP_D(int, mtu, 1350);
            QJS_PROP_D(int, tti, 50);
            QJS_PROP_D(int, uplinkCapacity, 5)
            QJS_PROP_D(int, downlinkCapacity, 20)
            QJS_PROP_D(bool, congestion, false)
            QJS_PROP_D(int, readBufferSize, 2)
            QJS_PROP_D(int, writeBufferSize, 2)
            QJS_PROP(QString, seed)
            QJS_PROP(ObfsHeaderObject, header)
            QJS_FUNCTION(F(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header, seed))
        };

        struct WebSocketObject
        {
            Q_GADGET
            typedef QMap<QString, QString> QStringStringMap;
            QJS_CONSTRUCTOR(WebSocketObject)
            QJS_PROP_D(QString, path, "/");
            QJS_PROP(QStringStringMap, headers);
            QJS_FUNCTION(F(path, headers))
        };

        struct HttpObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(HttpObject)
            QJS_PROP(QList<QString>, host);
            QJS_PROP_D(QString, path, "/");
            QJS_FUNCTION(F(host, path))
        };

        struct DomainSocketObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(DomainSocketObject)
            QJS_PROP_D(QString, path, "/");
            QJS_FUNCTION(F(path))
        };

        struct QuicObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(QuicObject)
            QJS_PROP_D(QString, security, "none")
            QJS_PROP(QString, key);
            QJS_PROP(ObfsHeaderObject, header);
            QJS_FUNCTION(F(security, key, header))
        };

        struct gRPCObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(gRPCObject)
            QJS_PROP_D(QString, serviceName, "GunService");
            QJS_FUNCTION(F(serviceName))
        };

        struct SockoptObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(SockoptObject)
            QJS_PROP_D(int, mark, 255)
            QJS_PROP_D(bool, tcpFastOpen, false)
            QJS_PROP_D(QString, tproxy, "off");
            QJS_FUNCTION(F(mark, tcpFastOpen, tproxy))
        };

        struct CertificateObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(CertificateObject)
            QJS_PROP_D(QString, usage, "encipherment");
            QJS_PROP(QString, certificateFile)
            QJS_PROP(QString, keyFile)
            QJS_PROP(QList<QString>, certificate)
            QJS_PROP(QList<QString>, key)
            QJS_FUNCTION(F(usage, certificateFile, keyFile, certificate, key))
        };

        struct TLSObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(TLSObject)
            QJS_PROP(QString, serverName);
            QJS_PROP_D(bool, disableSessionResumption, true);
            QJS_PROP_D(bool, disableSystemRoot, false);
            QJS_PROP(QList<QString>, alpn);
            QJS_PROP(QList<CertificateObject>, certificates);
            QJS_FUNCTION(F(serverName, disableSessionResumption, disableSystemRoot, alpn, certificates))
        };

        struct XTLSObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(XTLSObject)
            QJS_PROP(QString, serverName);
            QJS_PROP_D(bool, disableSessionResumption, true);
            QJS_PROP_D(bool, disableSystemRoot, false);
            QJS_PROP(QList<QString>, alpn);
            QJS_PROP(QList<CertificateObject>, certificates);
            QJS_FUNCTION(F(serverName, disableSessionResumption, disableSystemRoot, alpn, certificates))
        };
    } // namespace transfer
    //
    //
    struct StreamSettingsObject
    {
        Q_GADGET
        QJS_CONSTRUCTOR(StreamSettingsObject)
        QJS_PROP_D(QString, network, "tcp");
        QJS_PROP_D(QString, security, "none");
        QJS_PROP(transfer::SockoptObject, sockopt);
        QJS_PROP(transfer::TLSObject, tlsSettings);
        QJS_PROP(transfer::XTLSObject, xtlsSettings);
        QJS_PROP(transfer::TCPObject, tcpSettings);
        QJS_PROP(transfer::KCPObject, kcpSettings);
        QJS_PROP(transfer::WebSocketObject, wsSettings);
        QJS_PROP(transfer::HttpObject, httpSettings);
        QJS_PROP(transfer::DomainSocketObject, dsSettings);
        QJS_PROP(transfer::QuicObject, quicSettings);
        QJS_PROP(transfer::gRPCObject, grpcSettings);
        QJS_FUNCTION(F(network, security, sockopt),
                     F(tcpSettings, tlsSettings, xtlsSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings, grpcSettings))
    };

    struct FakeDNSObject
    {
        Q_GADGET
        QJS_CONSTRUCTOR(FakeDNSObject)
        QJS_PROP_D(QString, ipPool, "240.0.0.0/8");
        QJS_PROP_D(int, poolSize, 65535);
        QJS_FUNCTION(F(ipPool, poolSize))
    };

    // Some protocols from: https://v2ray.com/chapter_02/02_protocols.html
    namespace protocol
    {
        constexpr auto VMESS_USER_ALTERID_DEFAULT = 0;
        struct VMessUserObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(VMessUserObject)
            QJS_PROP_D(int, alterId, VMESS_USER_ALTERID_DEFAULT)
            QJS_PROP_D(QString, security, "auto")
            QJS_PROP_D(int, level, 0)
            QJS_PROP(QString, id, REQUIRED)
            QJS_FUNCTION(F(id, alterId, security, level))
        };

        struct VMessServerObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(VMessServerObject)
            QJS_PROP_D(QString, address, "0.0.0.0", REQUIRED)
            QJS_PROP_D(int, port, 0, REQUIRED)
            QJS_PROP(QList<VMessUserObject>, users)
            QJS_FUNCTION(F(address, port, users))
        };

        struct ShadowSocksServerObject
        {
            Q_GADGET
            QJS_CONSTRUCTOR(ShadowSocksServerObject)
            QJS_PROP_D(QString, address, "0.0.0.0")
            QJS_PROP_D(int, port, 0)
            QJS_PROP_D(QString, method, "aes-256-gcm")
            QJS_PROP(QString, password)
            QJS_FUNCTION(F(address, method, password, port))
        };
    } // namespace protocol
} // namespace Qv2ray::base::objects
