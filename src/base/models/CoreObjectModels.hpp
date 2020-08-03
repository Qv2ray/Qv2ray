#pragma once
#include "libs/QJsonStruct/QJsonIO.hpp"
#include "libs/QJsonStruct/QJsonStruct.hpp"

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
            JSONSTRUCT_REGISTER(DNSServerObject, F(QV2RAY_DNS_IS_COMPLEX_DNS, address, port, domains, expectIPs))
        };
        QMap<QString, QString> hosts;
        QList<DNSServerObject> servers;
        QString clientIp;
        QString tag;
        friend bool operator==(const DNSObject &left, const DNSObject &right)
        {
            return left.hosts == right.hosts && left.servers == right.servers && left.clientIp == right.clientIp && left.tag == right.tag;
        }
        JSONSTRUCT_REGISTER(DNSObject, F(hosts, servers, clientIp, tag))
    };
    //
    // Used in config generation
    struct AccountObject
    {
        QString user;
        QString pass;
        AccountObject() : user(), pass(){};
        JSONSTRUCT_REGISTER(AccountObject, F(user, pass))
    };
    //
    //
    struct ApiObject
    {
        QString tag;
        QList<QString> services;
        ApiObject() : tag("api"), services(){};
        JSONSTRUCT_REGISTER(ApiObject, F(tag, services))
    };
    //
    //
    struct SystemPolicyObject
    {
        bool statsInboundUplink;
        bool statsInboundDownlink;
        SystemPolicyObject() : statsInboundUplink(), statsInboundDownlink(){};
        JSONSTRUCT_REGISTER(SystemPolicyObject, F(statsInboundUplink, statsInboundDownlink))
    };
    //
    //
    struct LevelPolicyObject
    {
        int handshake;
        int connIdle;
        int uplinkOnly;
        int downlinkOnly;
        bool statsUserUplink;
        bool statsUserDownlink;
        int bufferSize;
        LevelPolicyObject() : handshake(), connIdle(), uplinkOnly(), downlinkOnly(), statsUserUplink(), statsUserDownlink(), bufferSize(){};
        JSONSTRUCT_REGISTER(LevelPolicyObject, F(handshake, connIdle, uplinkOnly, downlinkOnly, statsUserUplink, statsUserDownlink, bufferSize))
    };
    //
    //
    struct PolicyObject
    {
        QMap<QString, LevelPolicyObject> level;
        QList<SystemPolicyObject> system;
        PolicyObject() : level(), system(){};
        JSONSTRUCT_REGISTER(PolicyObject, F(level, system))
    };
    //
    //
    struct RuleObject
    {
        // Added due to the request of @aliyuchang33
        bool QV2RAY_RULE_ENABLED;
        bool QV2RAY_RULE_USE_BALANCER;
        QString QV2RAY_RULE_TAG;
        //
        QString type;
        QList<QString> domain;
        QList<QString> ip;
        QString port;
        QString network;
        QList<QString> source;
        QList<QString> user;
        QList<QString> inboundTag;
        QList<QString> protocol;
        QString attrs;
        QString outboundTag;
        QString balancerTag;
        RuleObject()
            : QV2RAY_RULE_ENABLED(true), QV2RAY_RULE_USE_BALANCER(false), QV2RAY_RULE_TAG("new rule"), type("field"), domain(), ip(),
              port("1-65535"), network(""), source(), user(), inboundTag(), protocol(), attrs(), outboundTag(""), balancerTag(""){};
        JSONSTRUCT_REGISTER(RuleObject, F(QV2RAY_RULE_ENABLED, QV2RAY_RULE_USE_BALANCER, QV2RAY_RULE_TAG, type, domain, ip, port, network,
                                          source, user, inboundTag, protocol, attrs, outboundTag, balancerTag))
    };
    //
    //
    struct BalancerObject
    {
        QString tag;
        QList<QString> selector;
        BalancerObject() : tag(), selector(){};
        JSONSTRUCT_REGISTER(BalancerObject, F(tag, selector))
    };
    //
    //
    namespace transfer
    {
        struct HTTPRequestObject
        {
            QString version;
            QString method;
            QList<QString> path;
            QMap<QString, QList<QString>> headers;
            HTTPRequestObject() : version("1.1"), method("GET"), path(), headers(){};
            JSONSTRUCT_REGISTER(HTTPRequestObject, F(version, method, path, headers))
        };
        //
        //
        struct HTTPResponseObject
        {
            QString version;
            QString status;
            QString reason;
            QMap<QString, QList<QString>> headers;
            HTTPResponseObject() : version("1.1"), status("200"), reason("OK"), headers(){};
            JSONSTRUCT_REGISTER(HTTPResponseObject, F(version, status, reason, headers))
        };
        //
        //
        struct TCPHeader_M_Object
        {
            QString type;
            HTTPRequestObject request;
            HTTPResponseObject response;
            TCPHeader_M_Object() : type("none"), request(), response(){};
            JSONSTRUCT_REGISTER(TCPHeader_M_Object, F(type, request, response))
        };
        //
        //
        struct HeaderObject
        {
            QString type;
            HeaderObject() : type("none"){};
            JSONSTRUCT_REGISTER(HeaderObject, F(type))
        };
        //
        //
        struct TCPObject
        {
            TCPHeader_M_Object header;
            TCPObject() : header(){};
            JSONSTRUCT_REGISTER(TCPObject, F(header))
        };
        //
        //
        struct KCPObject
        {
            int mtu = 1350;
            int tti = 20;
            int uplinkCapacity = 5;
            int downlinkCapacity = 20;
            bool congestion = false;
            int readBufferSize = 1;
            int writeBufferSize = 1;
            QString seed;
            HeaderObject header;
            KCPObject() : header(){};
            JSONSTRUCT_REGISTER(KCPObject,
                                F(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header, seed))
        };
        //
        //
        struct WebSocketObject
        {
            QString path;
            QMap<QString, QString> headers;
            WebSocketObject() : path("/"), headers(){};
            JSONSTRUCT_REGISTER(WebSocketObject, F(path, headers))
        };
        //
        //
        struct HttpObject
        {
            QList<QString> host;
            QString path;
            HttpObject() : host(), path("/"){};
            JSONSTRUCT_REGISTER(HttpObject, F(host, path))
        };
        //
        //
        struct DomainSocketObject
        {
            QString path;
            DomainSocketObject() : path("/"){};
            JSONSTRUCT_REGISTER(DomainSocketObject, F(path))
        };
        //
        //
        struct QuicObject
        {
            QString security;
            QString key;
            HeaderObject header;
            QuicObject() : security(""), key(""), header(){};
            JSONSTRUCT_REGISTER(QuicObject, F(security, key, header))
        };
        //
        //
        struct SockoptObject
        {
            int mark;
            bool tcpFastOpen;
            QString tproxy;
            SockoptObject() : mark(0), tcpFastOpen(false), tproxy("off"){};
            JSONSTRUCT_REGISTER(SockoptObject, F(mark, tcpFastOpen, tproxy))
        };
        //
        //
        struct CertificateObject
        {
            QString usage;
            QString certificateFile;
            QString keyFile;
            QList<QString> certificate;
            QList<QString> key;
            CertificateObject() : usage(), certificateFile(), keyFile(), certificate(), key(){};
            JSONSTRUCT_REGISTER(CertificateObject, F(usage, certificateFile, keyFile, certificate, key))
        };
        //
        //
        struct TLSObject
        {
            QString serverName;
            bool allowInsecure;
            bool allowInsecureCiphers;
            bool disableSessionResumption;
            QList<QString> alpn;
            QList<CertificateObject> certificates;
            bool disableSystemRoot;
            TLSObject()
                : serverName(), allowInsecure(), allowInsecureCiphers(), disableSessionResumption(true), certificates(), disableSystemRoot(){};
            JSONSTRUCT_REGISTER(TLSObject, F(serverName, allowInsecure, allowInsecureCiphers, disableSessionResumption, alpn, certificates,
                                             disableSystemRoot))
        };
    } // namespace transfer
    //
    //
    struct SniffingObject
    {
        bool enabled = false;
        QList<QString> destOverride;
        SniffingObject() : enabled(), destOverride(){};
        JSONSTRUCT_REGISTER(SniffingObject, F(enabled, destOverride))
    };
    //
    //
    struct StreamSettingsObject
    {
        QString network;
        QString security;
        transfer::SockoptObject sockopt;
        transfer::TLSObject tlsSettings;
        transfer::TCPObject tcpSettings;
        transfer::KCPObject kcpSettings;
        transfer::WebSocketObject wsSettings;
        transfer::HttpObject httpSettings;
        transfer::DomainSocketObject dsSettings;
        transfer::QuicObject quicSettings;
        StreamSettingsObject()
            : network("tcp"), security("none"), sockopt(), tlsSettings(), tcpSettings(), kcpSettings(), wsSettings(), httpSettings(),
              dsSettings(), quicSettings(){};
        JSONSTRUCT_REGISTER(StreamSettingsObject, F(network, security, sockopt, tcpSettings, tlsSettings, kcpSettings, wsSettings, httpSettings,
                                                    dsSettings, quicSettings))
    };
    //
    //
    struct MuxObject
    {
        bool enabled;
        int concurrency;
        MuxObject() : enabled(), concurrency(){};
        JSONSTRUCT_REGISTER(MuxObject, F(enabled, concurrency))
    };
    //
    // Some protocols from: https://v2ray.com/chapter_02/02_protocols.html
    namespace protocol
    {
        // DNS, OutBound
        struct DNSOut
        {
            QString network;
            QString address;
            int port;
            DNSOut() : network(""), address("0.0.0.0"), port(0){};
            JSONSTRUCT_REGISTER(DNSOut, F(network, address, port))
        };
        //
        // MTProto, InBound || OutBound
        struct MTProtoIn
        {
            struct UserObject
            {
                QString email;
                int level;
                QString secret;
                UserObject() : email("user@domain.com"), level(0), secret(""){};
                JSONSTRUCT_REGISTER(UserObject, F(email, level, secret))
            };
            QList<UserObject> users;
            JSONSTRUCT_REGISTER(MTProtoIn, F(users))
        };
        //
        // Socks, OutBound
        struct SocksServerObject
        {
            struct UserObject
            {
                QString user;
                QString pass;
                int level;
                UserObject() : user(), pass(), level(0){};
                JSONSTRUCT_REGISTER(UserObject, F(user, pass, level))
            };
            QString address;
            int port;
            QList<UserObject> users;
            SocksServerObject() : address("0.0.0.0"), port(0), users(){};
            JSONSTRUCT_REGISTER(SocksServerObject, F(address, port, users))
        };
        //
        // Http, OutBound
        struct HttpServerObject
        {
            struct UserObject
            {
                QString user;
                QString pass;
                int level;
                UserObject() : user(), pass(), level(0){};
                JSONSTRUCT_REGISTER(UserObject, F(user, pass, level))
            };
            QString address;
            int port;
            QList<UserObject> users;
            HttpServerObject() : address("0.0.0.0"), port(0), users(){};
            JSONSTRUCT_REGISTER(HttpServerObject, F(address, port, users))
        };
        //
        // VMess Server
        constexpr auto VMESS_USER_ALTERID_DEFAULT = 0;
        struct VMessServerObject
        {
            struct UserObject
            {
                QString id;
                int alterId;
                QString security;
                int level;
                QString testsEnabled;
                UserObject() : id(), alterId(VMESS_USER_ALTERID_DEFAULT), security("auto"), level(0), testsEnabled("none"){};
                JSONSTRUCT_REGISTER(UserObject, F(id, alterId, security, level, testsEnabled))
            };

            QString address;
            int port;
            QList<UserObject> users;
            VMessServerObject() : address(""), port(0), users(){};
            JSONSTRUCT_REGISTER(VMessServerObject, F(address, port, users))
        };
        //
        // VLess Server
        struct VLessServerObject
        {
            struct UserObject
            {
                QString id;
                QString encryption;
                UserObject() : id(), encryption("none"){};
                JSONSTRUCT_REGISTER(UserObject, F(id, encryption))
            };

            QString address;
            int port;
            QList<UserObject> users;
            VLessServerObject() : address(""), port(0), users(){};
            JSONSTRUCT_REGISTER(VLessServerObject, F(address, port, users))
        };
        //
        // ShadowSocks Server
        struct ShadowSocksServerObject
        {
            QString email;
            QString address;
            QString method;
            QString password;
            bool ota;
            int level;
            int port;
            ShadowSocksServerObject() : email(""), address("0.0.0.0"), method("aes-256-cfb"), password(""), ota(false), level(0), port(0){};
            JSONSTRUCT_REGISTER(ShadowSocksServerObject, F(email, address, port, method, password, ota, level))
        };
    } // namespace protocol
} // namespace Qv2ray::base::objects
