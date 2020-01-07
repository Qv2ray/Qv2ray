#ifndef V2CONFIG_H
#define V2CONFIG_H
#include <list>
#include <string>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <x2struct/x2struct.hpp>
using namespace x2struct;
using namespace std;

#define SAFE_TYPEDEF(Base, name) \
    class name : public Base { \
        public: \
            template <class... Args> \
            explicit name (Args... args) : Base(args...) {} \
            const Base& raw() const { return *this; } \
    };

struct QvConfigIdentifier {
    QString subscriptionName;
    QString connectionName;
    QvConfigIdentifier() { };
    //
    bool isEmpty()
    {
        return connectionName.isEmpty();
    }
    QvConfigIdentifier(QString connectionName)
    {
        this->connectionName = connectionName;
    }
    QvConfigIdentifier(QString connectionName, QString subscriptionName)
    {
        this->connectionName = connectionName;
        this->subscriptionName = subscriptionName;
    }
    const QString IdentifierString() const
    {
        return connectionName + (subscriptionName.isEmpty() ? "" : " (" + subscriptionName + ")");
    }
    friend bool operator==(QvConfigIdentifier &left, QvConfigIdentifier &right)
    {
        return left.subscriptionName == right.subscriptionName && left.connectionName == right.connectionName;
    }
    friend bool operator!=(QvConfigIdentifier &left, QvConfigIdentifier &right)
    {
        return !(left == right);
    }
    friend bool operator==(QvConfigIdentifier &left, QString &right)
    {
        return left.IdentifierString() == right;
    }
    friend bool operator!=(QvConfigIdentifier &left, QString &right)
    {
        return !(left.IdentifierString() == right);
    }
    // To make QMap happy
    friend bool operator<(const QvConfigIdentifier left, const QvConfigIdentifier right)
    {
        return left.IdentifierString() < right.IdentifierString();
    }
    friend bool operator>(const QvConfigIdentifier left, const QvConfigIdentifier right)
    {
        return left.IdentifierString() > right.IdentifierString();
    }
    XTOSTRUCT(O(subscriptionName, connectionName))
};

Q_DECLARE_METATYPE(QvConfigIdentifier);

namespace Qv2ray
{
    // To prevent anonying QJsonObject misuse
    SAFE_TYPEDEF(QJsonObject, INBOUNDSETTING)
    SAFE_TYPEDEF(QJsonObject, OUTBOUNDSETTING)
    SAFE_TYPEDEF(QJsonObject, INBOUND)
    SAFE_TYPEDEF(QJsonObject, OUTBOUND)
    SAFE_TYPEDEF(QJsonObject, CONFIGROOT)
    //
    SAFE_TYPEDEF(QJsonArray, INOUTLIST)
    SAFE_TYPEDEF(INOUTLIST, OUTBOUNDS)
    SAFE_TYPEDEF(INOUTLIST, INBOUNDS)
    SAFE_TYPEDEF(QJsonObject, ROUTING)
    SAFE_TYPEDEF(QJsonObject, ROUTERULE)
    SAFE_TYPEDEF(QJsonArray, ROUTERULELIST)
}

/* ----------------------------------------- */

namespace Qv2ray
{
    namespace V2ConfigModels
    {
        //
        // Used in config generation
        struct AccountObject {
            QString user;
            QString pass;
            XTOSTRUCT(O(user, pass))
        };
        //
        //
        struct ApiObject {
            QString tag;
            QList<QString> services;
            ApiObject() : tag("api"), services() {}
            XTOSTRUCT(O(tag, services))
        };
        //
        //
        struct SystemPolicyObject {
            bool statsInboundUplink;
            bool statsInboundDownlink;
            SystemPolicyObject() : statsInboundUplink(), statsInboundDownlink() {}
            XTOSTRUCT(O(statsInboundUplink, statsInboundDownlink))
        };
        //
        //
        struct LevelPolicyObject {
            int handshake;
            int connIdle;
            int uplinkOnly;
            int downlinkOnly;
            bool statsUserUplink;
            bool statsUserDownlink;
            int bufferSize;
            LevelPolicyObject(): handshake(), connIdle(), uplinkOnly(), downlinkOnly(), statsUserUplink(), statsUserDownlink(), bufferSize() {}
            XTOSTRUCT(O(handshake, connIdle, uplinkOnly, downlinkOnly, statsUserUplink, statsUserDownlink, bufferSize))
        };
        //
        //
        struct PolicyObject {
            QMap<QString, LevelPolicyObject> level;
            list<SystemPolicyObject> system;
            PolicyObject(): level(), system() {}
            XTOSTRUCT(O(level, system))
        };
        //
        //
        struct RuleObject {
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
            RuleObject() : QV2RAY_RULE_ENABLED(true), QV2RAY_RULE_USE_BALANCER(false), QV2RAY_RULE_TAG("new rule"), type("field"), domain(), ip(), port("1-65535"), network(""), source(), user(), inboundTag(), protocol(), attrs(), outboundTag(""), balancerTag("") {}
            XTOSTRUCT(O(QV2RAY_RULE_ENABLED, QV2RAY_RULE_USE_BALANCER, QV2RAY_RULE_TAG, type, domain, ip, port, network, source, user, inboundTag, protocol, attrs, outboundTag, balancerTag))
        };
        //
        //
        struct BalancerObject {
            QString tag ;
            QList<QString> selector;
            BalancerObject() : tag(), selector() {}
            XTOSTRUCT(O(tag, selector))
        };
        //
        //
        namespace TSObjects
        {
            struct HTTPRequestObject {
                QString version;
                QString method;
                QList<QString> path;
                QMap<QString, QList<QString>> headers;
                HTTPRequestObject(): version("1.1"), method("GET"), path(), headers() {}
                XTOSTRUCT(O(version, method, path, headers))
            };
            //
            //
            struct HTTPResponseObject {
                QString version;
                QString status;
                QString reason;
                QMap<QString, QList<QString>> headers;
                HTTPResponseObject(): version("1.1"), status("200"), reason("OK"), headers() {}
                XTOSTRUCT(O(version, status, reason, headers))
            };
            //
            //
            struct TCPHeader_M_Object {
                QString type;
                HTTPRequestObject request;
                HTTPResponseObject response;
                TCPHeader_M_Object(): type("none"), request(), response() {}
                XTOSTRUCT(O(type, request, response))
            };
            //
            //
            struct HeaderObject {
                QString type;
                HeaderObject(): type("none") {}
                XTOSTRUCT(O(type))
            };
            //
            //
            struct TCPObject {
                TCPHeader_M_Object header;
                TCPObject(): header() {}
                XTOSTRUCT(O(header))
            };
            //
            //
            struct KCPObject {
                int mtu = 1350;
                int tti = 20;
                int uplinkCapacity = 5;
                int downlinkCapacity = 20;
                bool congestion = false;
                int readBufferSize = 1;
                int writeBufferSize = 1;
                HeaderObject header;
                KCPObject(): header() {}
                XTOSTRUCT(O(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header))
            };
            //
            //
            struct WebSocketObject {
                QString path;
                QMap<QString, QString> headers;
                WebSocketObject(): path("/"), headers() {}
                XTOSTRUCT(O(path, headers))
            };
            //
            //
            struct HttpObject {
                QList<QString> host;
                QString path;
                HttpObject() : host(), path("/") {}
                XTOSTRUCT(O(host, path))
            };
            //
            //
            struct DomainSocketObject {
                QString path;
                DomainSocketObject(): path("/") {}
                XTOSTRUCT(O(path))
            };
            //
            //
            struct QuicObject {
                QString security;
                QString key;
                HeaderObject header;
                QuicObject(): security(""), key(""), header() {}
                XTOSTRUCT(O(security, key, header))
            };
            //
            //
            struct SockoptObject {
                int mark;
                bool tcpFastOpen;
                QString tproxy;
                SockoptObject(): mark(0), tcpFastOpen(false), tproxy("off") {}
                XTOSTRUCT(O(mark, tcpFastOpen, tproxy))
            };
            //
            //
            struct CertificateObject {
                QString usage;
                QString certificateFile;
                QString keyFile;
                QList<QString> certificate;
                QList<QString> key;
                CertificateObject(): usage(), certificateFile(), keyFile(), certificate(), key() {}
                XTOSTRUCT(O(usage, certificateFile, keyFile, certificate, key))
            };
            //
            //
            struct TLSObject {
                QString serverName;
                bool allowInsecure;
                QList<QString> alpn;
                list<CertificateObject> certificates;
                bool disableSystemRoot;
                TLSObject(): serverName(), allowInsecure(), certificates(), disableSystemRoot() {}
                XTOSTRUCT(O(serverName, allowInsecure, alpn, certificates, disableSystemRoot))
            };
        }
        //
        //
        struct SniffingObject {
            bool enabled = false;
            QList<QString> destOverride;
            SniffingObject(): enabled(), destOverride() {}
            XTOSTRUCT(O(enabled, destOverride))
        };
        //
        //
        struct StreamSettingsObject  {
            QString network;
            QString security;
            TSObjects::SockoptObject sockopt;
            TSObjects::TLSObject tlsSettings;
            TSObjects::TCPObject tcpSettings;
            TSObjects::KCPObject kcpSettings;
            TSObjects::WebSocketObject wsSettings;
            TSObjects::HttpObject httpSettings;
            TSObjects::DomainSocketObject dsSettings;
            TSObjects::QuicObject quicSettings;
            StreamSettingsObject(): network("tcp"), security("none"), sockopt(),  tlsSettings(), tcpSettings(), kcpSettings(), wsSettings(), httpSettings(), dsSettings(), quicSettings() {}
            XTOSTRUCT(O(network, security, sockopt, tcpSettings, tlsSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings))
        };
        //
        //
        struct MuxObject {
            bool enabled;
            int concurrency;
            MuxObject(): enabled(), concurrency() {}
            XTOSTRUCT(O(enabled, concurrency))
        };
        //
        // Some protocols from: https://v2ray.com/chapter_02/02_protocols.html
        namespace Protocols
        {
            // DNS, OutBound
            struct DNSOut {
                QString network;
                QString address;
                int port;
                DNSOut(): network(""), address("0.0.0.0"), port(0) {}
                XTOSTRUCT(O(network, address, port))
            };
            //
            // MTProto, InBound || OutBound
            struct MTProtoIn {
                struct UserObject {
                    QString email;
                    int level;
                    QString secret;
                    UserObject() : email("user@domain.com"), level(0), secret("") {}
                    XTOSTRUCT(O(email, level, secret))
                };
                list<UserObject> users;
                XTOSTRUCT(O(users))
            };
            //
            // Socks, OutBound
            struct SocksServerObject {
                struct UserObject {
                    QString user;
                    QString pass;
                    int level;
                    UserObject(): user("username"), pass("password"), level(0) {}
                    XTOSTRUCT(O(user, pass, level))
                };

                QString address;
                int port;
                list<UserObject> users;
                SocksServerObject(): address("0.0.0.0"), port(0), users() {}
                XTOSTRUCT(O(address, port, users))
            };
            //
            // VMess Server
            struct VMessServerObject {
                struct UserObject {
                    QString id;
                    int alterId;
                    QString security;
                    int level;
                    UserObject() : id(""), alterId(64), security("auto"), level(0) {}
                    XTOSTRUCT(O(id, alterId, security, level))
                };

                QString address;
                int port;
                list<UserObject> users;
                VMessServerObject(): address(""), port(0), users() {}
                XTOSTRUCT(O(address, port, users))
            };
            //
            // ShadowSocks Server
            struct ShadowSocksServerObject {
                QString email;
                QString address;
                QString method;
                QString password;
                bool ota;
                int level;
                int port;
                ShadowSocksServerObject(): email("user@domain.com"), address("0.0.0.0"), method("aes-256-cfb"), password(""), ota(false), level(0), port(0) {}
                XTOSTRUCT(O(email, address, port, method, password, ota, level))
            };
        }
    }
}

using namespace Qv2ray;
using namespace Qv2ray::V2ConfigModels;
using namespace Qv2ray::V2ConfigModels::Protocols;

#endif
