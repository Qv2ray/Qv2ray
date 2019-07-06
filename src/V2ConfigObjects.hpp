#include <list>
#include <string>

// TODO Features
#define USE_TODO_FEATURES true

#include <x2struct/x2struct.hpp>
using namespace x2struct;

#ifndef V2CONFIG_H
#define V2CONFIG_H

using namespace std;
/*------------------------------------------------------------------------------------------------------------*/

namespace Qv2ray
{
    namespace V2ConfigModels
    {
#if USE_TODO_FEATURES
        struct nullStruct {
            string _;
            XTOSTRUCT(O(_))
        };
#endif
        struct VMessProtocolConfigObject {
            string v, ps, add, port, id, aid, net, type, host, path, tls;
            XTOSTRUCT(O(v, ps, add, port, id, aid, net, type, host, path, tls))
        };
        // Two struct defining TYPE parameter to be passed into inbound configs and outbound configs.
        struct XOutBoundsType {
        };
        struct XInBoundsType {
        };

        struct LogObject {
            string access;
            string error;
            string loglevel;
            LogObject(): access(), error(), loglevel() {}
            XTOSTRUCT(O(access, error, loglevel))
        };

        struct ApiObject {
            string tag;
            list<string> services;
            ApiObject() : tag(), services() {}
            XTOSTRUCT(O(tag, services))
        };
        struct DNSServerObject {
            string address;
            int port;
            list<string> domains;
            DNSServerObject(): address(), port(), domains() {}
            XTOSTRUCT(O(address, port, domains))
        };
        struct DnsObject {
            map<string, string> hosts;
#if USE_TODO_FEATURES
            nullStruct servers;
            //tuple<string, string, list<DNSObjects::ServerObject>> servers;
#else
            // Currently does not support ServerObject as tuple is.... quite complicated...
            list<string> servers;
#endif
            DnsObject(): hosts(), servers() {}
            XTOSTRUCT(O(hosts, servers))
        };
        namespace ROUTINGObjects
        {
            struct RuleObject {
                string type = "field";
                list<string> domain;
                list<string> ip;
                string port;
                string network;
                list<string> source;
                list<string> user;
                string inboundTag;
                string protocol;
                string attrs;
                RuleObject() : type(), domain(), ip(), port(), network(), source(), user(), inboundTag(), protocol(), attrs() {}
                XTOSTRUCT(O(type, domain, ip, port, network, source, user, inboundTag, protocol, attrs))
            };
        }

        struct RoutingObject {
            string domainStrategy;
            list<ROUTINGObjects::RuleObject> rules;
            RoutingObject() : domainStrategy(), rules() {}
            XTOSTRUCT(O(domainStrategy, rules))
        };
        namespace POLICYObjects
        {
            struct SystemPolicyObject {
                bool statsInboundUplink;
                bool statsInboundDownlink;
                SystemPolicyObject() : statsInboundUplink(), statsInboundDownlink() {}
                XTOSTRUCT(O(statsInboundUplink, statsInboundDownlink))
            };

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
        }
        struct PolicyObject {
            map<string, POLICYObjects::LevelPolicyObject> level;
            list<POLICYObjects::SystemPolicyObject> system;
            PolicyObject(): level(), system() {}
            XTOSTRUCT(O(level, system))
        };
        namespace TRANSFERObjects
        {
            namespace TRANSFERObjectsInternal
            {
                struct HTTPRequestObject {
                    string version;
                    string method;
                    list<string> path;
                    map<string, list<string>> headers;
                    HTTPRequestObject(): version(), method(), path(), headers() {}
                    XTOSTRUCT(O(version, method, path, headers))
                };

                struct HTTPResponseObject {
                    string version;
                    string status;
                    string reason;
                    map<string, list<string>> headers;
                    HTTPResponseObject(): version(), status(), reason(), headers() {}
                    XTOSTRUCT(O(version, status, reason, headers))
                };
                struct TCPHeader_M_Object {
                    string type;
                    HTTPRequestObject request;
                    HTTPResponseObject response;
                    TCPHeader_M_Object(): type(), request(), response() {}
                    XTOSTRUCT(O(type, request, response))
                };
                struct HeaderObject {
                    string type;
                    HeaderObject(): type() {}
                    XTOSTRUCT(O(type))
                };
            }


            struct TCPObject {
                TRANSFERObjectsInternal:: TCPHeader_M_Object header;
                TCPObject(): header() {}
                XTOSTRUCT(O(header))
            };


            struct KCPObject {
                int mtu;
                int tti;
                int uplinkCapacity;
                int downlinkCapacity;
                bool congestion;
                int readBufferSize;
                int writeBufferSize;
                TRANSFERObjectsInternal::  HeaderObject header;
                KCPObject(): mtu(), tti(), uplinkCapacity(), downlinkCapacity(), congestion(), readBufferSize(), writeBufferSize(), header() {}
                XTOSTRUCT(O(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header))
            };


            struct WebSocketObject {
                string path;
                map<string, string> headers;
                WebSocketObject(): path(), headers() {}
                XTOSTRUCT(O(path, headers))
            };

            struct HttpObject {
                list<string> host;
                string path;
                HttpObject() : host(), path() {}
                XTOSTRUCT(O(host, path))
            };

            struct DomainSocketObject {
                string path;
                DomainSocketObject(): path() {}
                XTOSTRUCT(O(path))
            };

            struct QuicObject {
                string security;
                string key;
                TRANSFERObjectsInternal::HeaderObject header;
                QuicObject(): security(), key(), header() {}
                XTOSTRUCT(O(security, key, header))
            };


        }
        struct TransportObject {
            TRANSFERObjects::TCPObject tcpSettings;
            TRANSFERObjects::KCPObject kcpSettings;
            TRANSFERObjects::WebSocketObject wsSettings;
            TRANSFERObjects::HttpObject httpSettings;
            TRANSFERObjects::DomainSocketObject dsSettings;
            TRANSFERObjects::QuicObject quicSettings;
            TransportObject(): tcpSettings(), kcpSettings(), wsSettings(), httpSettings(), dsSettings(), quicSettings() {}
            XTOSTRUCT(O(tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings))
        };
        namespace INBOUNDObjects
        {
            struct SniffingObject {
                bool enabled;
                string destOverride;
                SniffingObject(): enabled(), destOverride() {}
                XTOSTRUCT(O(enabled, destOverride))
            };

        }
        namespace STREAMSETTINGSObjects
        {
            struct SockoptObject {
                int mark;
                bool tcpFastOpen;
                string tproxy;
                SockoptObject(): mark(), tcpFastOpen(), tproxy() {}
                XTOSTRUCT(O(mark, tcpFastOpen, tproxy))
            };

            struct CertificateObject {
                string usage;
                string certificateFile;
                string keyFile;
                list<string> certificate;
                list<string> key;
                CertificateObject(): usage(), certificateFile(), keyFile(), certificate(), key() {}
                XTOSTRUCT(O(usage, certificateFile, keyFile, certificate, key))
            };

            struct TLSObject {
                string serverName;
                bool allowInsecure;
                list<string> alpn;
                list<CertificateObject> certificates;
                bool disableSystemRoot;
                TLSObject(): serverName(), allowInsecure(), certificates(), disableSystemRoot() {}
                XTOSTRUCT(O(serverName, allowInsecure, alpn, certificates, disableSystemRoot))
            };
        }

        struct StreamSettingsObject  {
            string network;
            string security;
            STREAMSETTINGSObjects::SockoptObject sockopt;
            STREAMSETTINGSObjects::TLSObject tlsSettings;
            TRANSFERObjects::TCPObject tcpSettings;
            TRANSFERObjects::KCPObject kcpSettings;
            TRANSFERObjects::WebSocketObject wsSettings;
            TRANSFERObjects::HttpObject httpSettings;
            TRANSFERObjects::DomainSocketObject dsSettings;
            TRANSFERObjects::QuicObject quicSettings;
            StreamSettingsObject(): network(), security(), sockopt(),  tlsSettings(), tcpSettings(), kcpSettings(), wsSettings(), httpSettings(), dsSettings(), quicSettings() {}
            XTOSTRUCT(O(network, security, sockopt, tcpSettings, tlsSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings))
        };

        // THIS WILL BE CONVERTED TO JSON!
        template<typename XINBOUNDSETTINGOBJECT>
        struct InboundObject {
            static_assert(std::is_base_of<XInBoundsType, XINBOUNDSETTINGOBJECT>::value, "XINBOUNDSETTINGOBJECT must extend XInBoundsType");
            int port;
            string listen;
            string protocol;
            XINBOUNDSETTINGOBJECT settings;
            StreamSettingsObject streamSettings;
            string tag;
            INBOUNDObjects::SniffingObject sniffing;
            InboundObject(): port(), listen(), protocol(), settings(), streamSettings(), tag(), sniffing() {}
            XTOSTRUCT(O(port, listen, protocol, settings, streamSettings, tag, sniffing))
        };
        struct MuxObject {
            bool enabled;
            int concurrency;
            MuxObject(): enabled(), concurrency() {}
            XTOSTRUCT(O(enabled, concurrency))
        };
        // THIS WILL BE CONVERTED TO JSON!
        template <typename XOUTBOUNDSETTINGOBJECT>
        struct OutboundObject {
            static_assert(std::is_base_of<XOutBoundsType, XOUTBOUNDSETTINGOBJECT>::value, "XOUTBOUNDSETTINGOBJECT must extend XOutBoundsType");
            string sendThrough;
            string protocol;
            XOUTBOUNDSETTINGOBJECT settings;
            string tag;
            StreamSettingsObject streamSettings;
            MuxObject mux;
            OutboundObject(): sendThrough(), protocol(), settings(), tag(), streamSettings(), mux() {}
            XTOSTRUCT(O(sendThrough, protocol, settings, tag, streamSettings, mux))
        };


        // THIS WILL BE CONVERTED TO JSON!
        struct StatsObject {
            bool _; // Placeholder...
            StatsObject(): _() {}
            XTOSTRUCT(O(_))
        };


#if USE_TODO_FEATURES
#else
        template <typename XInBound_T, typename XOutBound_T>
#endif
        struct RootObject {
            LogObject log;
            ApiObject api;
            DnsObject dns;
            RoutingObject routing;
#if USE_TODO_FEATURES
            // THIS WILL BE CONVERTED TO JSON!
            // Place holder for in/out bounds
            nullStruct inbounds;
            nullStruct outbounds;
#else
            list<InboundObject<XInBound_T>> inbounds;
            list<OutboundObject<XOutBound_T>> outbounds;
            TransportObject transport;
#endif
            StatsObject stats;
            PolicyObject policy;
#if USE_TODO_FEATURES
            RootObject(): log(), api(), dns(), routing(), inbounds(), outbounds(), stats(), policy() {}
            XTOSTRUCT(O(log, api, dns, routing, inbounds, outbounds, stats, policy))
#else
            RootObject(): log(), api(), dns(), routing(), inbounds(), outbounds(), transport(), stats(), policy() {}
            XTOSTRUCT(O(log, api, dns, routing, inbounds, outbounds, transport, stats, policy))
#endif
        };
    }
}

namespace Qv2ray
{
    namespace V2ConfigModels
    {
        /// Some protocols from: https://v2ray.com/chapter_02/02_protocols.html
        namespace Protocols
        {
            /// BlackHole Protocol, OutBound
            struct BlackHole : XOutBoundsType {
                struct ResponseObject {
                    string type;
                };
                ResponseObject response;
            };

            /// DNS, OutBound
            struct DNS: XOutBoundsType {
                string network;
                string address;
                int port;
            };

            /// Dokodemo-door, InBound
            struct Dokodemo_door : XInBoundsType {
                string address;
                int port;
                string network;
                int timeout;
                bool followRedirect;
                int userLevel;
            };

            /// Freedom, OutBound
            struct Freedom: XOutBoundsType {
                string domainStrategy;
                string redirect;
                int userLevel;
            };

            struct AccountObject {
                string user;
                string pass;
                XTOSTRUCT(O(user, pass))
            };

            /// HTTP, InBound
            struct HTTP: XInBoundsType {
                int timeout;
                list<AccountObject> accounts;
                bool allowTransparent;
                int userLevel;
                XTOSTRUCT(O(timeout, accounts, allowTransparent, userLevel))
            };

            /// MTProto, InBound || OutBound
            struct MTProto: XInBoundsType, XOutBoundsType {
                struct UserObject {
                    string email;
                    int level;
                    string secret;
                };
                list<UserObject> users;
            };

            // We don't add shadowsocks, (As it's quite complex and I'm quite lazy...)

            /// Socks, InBound, OutBound
            struct Socks: XInBoundsType, XOutBoundsType {
                struct UserObject {
                };
                struct ServerObject {
                    string address;
                    int port;
                    list<UserObject> users;
                };
                list<ServerObject> servers;
                string auth;
                list<AccountObject> accounts;
                bool udp;
                string ip;
                int userLevel;
            };

            struct VMess: XInBoundsType, XOutBoundsType {
                struct ServerObject {
                    struct UserObject {
                        string id;
                        int alterId;
                        string security;
                        int level;
                        XTOSTRUCT(O(id, alterId, security, level))
                    };
                    // OUTBound;
                    string address;
                    int port;
                    list<UserObject> users;
                    XTOSTRUCT(O(address, port, users))
                };
                list<ServerObject> vnext;
                // INBound;
                struct ClientObject {
                    string id;
                    int level;
                    int alterId;
                    string email;
                    XTOSTRUCT(O(id, level, alterId, email))
                };
                list<ClientObject> clients;
                // detour and default will not be implemented as it's complicated...
                bool disableInsecureEncryption;
                XTOSTRUCT(O(vnext, clients, disableInsecureEncryption))
            };
        }
    }
}

/// Code above has passed these tests.
//using namespace  Qv2ray::V2ConfigModels;
//RootObject<Protocols::HTTP, Protocols::VMess> x;
//InboundObject<Protocols::HTTP> inH;
//x.inbounds.insert(x.inbounds.end(), inH);
//OutboundObject<Protocols::VMess> inV;
//x.outbounds.insert(x.outbounds.end(), inV);
//QString jsonConfig = Utils::StructToJSON(x);
//cout << jsonConfig.toStdString() << endl;
///
#endif
