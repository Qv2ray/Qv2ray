#pragma once
#include "3rdparty/x2struct/x2struct.hpp"

#include <QList>
#include <QMap>
#include <QString>

namespace Qv2ray::base::objects
{
    //
    // Used in config generation
    struct AccountObject
    {
        QString user;
        QString pass;
        XTOSTRUCT(O(user, pass))
    };
    //
    //
    struct ApiObject
    {
        QString tag;
        QList<QString> services;
        ApiObject() : tag("api"), services()
        {
        }
        XTOSTRUCT(O(tag, services))
    };
    //
    //
    struct SystemPolicyObject
    {
        bool statsInboundUplink;
        bool statsInboundDownlink;
        SystemPolicyObject() : statsInboundUplink(), statsInboundDownlink()
        {
        }
        XTOSTRUCT(O(statsInboundUplink, statsInboundDownlink))
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
        LevelPolicyObject() : handshake(), connIdle(), uplinkOnly(), downlinkOnly(), statsUserUplink(), statsUserDownlink(), bufferSize()
        {
        }
        XTOSTRUCT(O(handshake, connIdle, uplinkOnly, downlinkOnly, statsUserUplink, statsUserDownlink, bufferSize))
    };
    //
    //
    struct PolicyObject
    {
        QMap<QString, LevelPolicyObject> level;
        QList<SystemPolicyObject> system;
        PolicyObject() : level(), system()
        {
        }
        XTOSTRUCT(O(level, system))
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
              port("1-65535"), network(""), source(), user(), inboundTag(), protocol(), attrs(), outboundTag(""), balancerTag("")
        {
        }
        XTOSTRUCT(O(QV2RAY_RULE_ENABLED, QV2RAY_RULE_USE_BALANCER, QV2RAY_RULE_TAG, type, domain, ip, port, network, source, user, inboundTag,
                    protocol, attrs, outboundTag, balancerTag))
    };
    //
    //
    struct BalancerObject
    {
        QString tag;
        QList<QString> selector;
        BalancerObject() : tag(), selector()
        {
        }
        XTOSTRUCT(O(tag, selector))
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
            HTTPRequestObject() : version("1.1"), method("GET"), path(), headers()
            {
            }
            XTOSTRUCT(O(version, method, path, headers))
        };
        //
        //
        struct HTTPResponseObject
        {
            QString version;
            QString status;
            QString reason;
            QMap<QString, QList<QString>> headers;
            HTTPResponseObject() : version("1.1"), status("200"), reason("OK"), headers()
            {
            }
            XTOSTRUCT(O(version, status, reason, headers))
        };
        //
        //
        struct TCPHeader_M_Object
        {
            QString type;
            HTTPRequestObject request;
            HTTPResponseObject response;
            TCPHeader_M_Object() : type("none"), request(), response()
            {
            }
            XTOSTRUCT(O(type, request, response))
        };
        //
        //
        struct HeaderObject
        {
            QString type;
            HeaderObject() : type("none")
            {
            }
            XTOSTRUCT(O(type))
        };
        //
        //
        struct TCPObject
        {
            TCPHeader_M_Object header;
            TCPObject() : header()
            {
            }
            XTOSTRUCT(O(header))
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
            HeaderObject header;
            KCPObject() : header()
            {
            }
            XTOSTRUCT(O(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header))
        };
        //
        //
        struct WebSocketObject
        {
            QString path;
            QMap<QString, QString> headers;
            WebSocketObject() : path("/"), headers()
            {
            }
            XTOSTRUCT(O(path, headers))
        };
        //
        //
        struct HttpObject
        {
            QList<QString> host;
            QString path;
            HttpObject() : host(), path("/")
            {
            }
            XTOSTRUCT(O(host, path))
        };
        //
        //
        struct DomainSocketObject
        {
            QString path;
            DomainSocketObject() : path("/")
            {
            }
            XTOSTRUCT(O(path))
        };
        //
        //
        struct QuicObject
        {
            QString security;
            QString key;
            HeaderObject header;
            QuicObject() : security(""), key(""), header()
            {
            }
            XTOSTRUCT(O(security, key, header))
        };
        //
        //
        struct SockoptObject
        {
            int mark;
            bool tcpFastOpen;
            QString tproxy;
            SockoptObject() : mark(0), tcpFastOpen(false), tproxy("off")
            {
            }
            XTOSTRUCT(O(mark, tcpFastOpen, tproxy))
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
            CertificateObject() : usage(), certificateFile(), keyFile(), certificate(), key()
            {
            }
            XTOSTRUCT(O(usage, certificateFile, keyFile, certificate, key))
        };
        //
        //
        struct TLSObject
        {
            QString serverName;
            bool allowInsecure;
            QList<QString> alpn;
            QList<CertificateObject> certificates;
            bool disableSystemRoot;
            TLSObject() : serverName(), allowInsecure(), certificates(), disableSystemRoot()
            {
            }
            XTOSTRUCT(O(serverName, allowInsecure, alpn, certificates, disableSystemRoot))
        };
    } // namespace transfer
    //
    //
    struct SniffingObject
    {
        bool enabled = false;
        QList<QString> destOverride;
        SniffingObject() : enabled(), destOverride()
        {
        }
        XTOSTRUCT(O(enabled, destOverride))
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
              dsSettings(), quicSettings()
        {
        }
        XTOSTRUCT(O(network, security, sockopt, tcpSettings, tlsSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings))
    };
    //
    //
    struct MuxObject
    {
        bool enabled;
        int concurrency;
        MuxObject() : enabled(), concurrency()
        {
        }
        XTOSTRUCT(O(enabled, concurrency))
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
            DNSOut() : network(""), address("0.0.0.0"), port(0)
            {
            }
            XTOSTRUCT(O(network, address, port))
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
                UserObject() : email("user@domain.com"), level(0), secret("")
                {
                }
                XTOSTRUCT(O(email, level, secret))
            };
            QList<UserObject> users;
            XTOSTRUCT(O(users))
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
                UserObject() : user("username"), pass("password"), level(0)
                {
                }
                XTOSTRUCT(O(user, pass, level))
            };

            QString address;
            int port;
            QList<UserObject> users;
            SocksServerObject() : address("0.0.0.0"), port(0), users()
            {
            }
            XTOSTRUCT(O(address, port, users))
        };
        //
        // VMess Server
        struct VMessServerObject
        {
            struct UserObject
            {
                QString id;
                int alterId;
                QString security;
                int level;
                UserObject() : id(""), alterId(64), security("auto"), level(0)
                {
                }
                XTOSTRUCT(O(id, alterId, security, level))
            };

            QString address;
            int port;
            QList<UserObject> users;
            VMessServerObject() : address(""), port(0), users()
            {
            }
            XTOSTRUCT(O(address, port, users))
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
            ShadowSocksServerObject()
                : email("user@domain.com"), address("0.0.0.0"), method("aes-256-cfb"), password(""), ota(false), level(0), port(0)
            {
            }
            XTOSTRUCT(O(email, address, port, method, password, ota, level))
        };
	//Shadowsocksr server
        struct ShadowSocksRServerObject {
            QString address;
            QString method;
            QString password;
            QString protocol;
            QString protocol_param;
            QString obfs;
            QString obfs_param;
            QString remarks;
            QString group;
            int port;
            ShadowSocksRServerObject():  address("0.0.0.0"), method("aes-256-cfb"), password(""),  protocol(""),protocol_param(""),obfs(""),obfs_param(""),remarks(""),group(""),port(0) {}
            XTOSTRUCT(O( address, method, password,  protocol,protocol_param,obfs,obfs_param,remarks,group,port))
        };
    } // namespace protocol
} // namespace Qv2ray::base::objects
