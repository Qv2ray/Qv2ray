#include <list>
#include <string>

// TODO Features
#define USE_TODO_FEATURES false

#if USE_TODO_FEATURES
#include <jsoncons/json.hpp>
using namespace jsoncons;
#else
#include <x2struct/x2struct.hpp>
using namespace x2struct;
#endif

#ifndef V2CONFIG_H
#define V2CONFIG_H

using namespace std;
/*------------------------------------------------------------------------------------------------------------*/

namespace Hv2ray
{
    namespace V2ConfigModels
    {
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
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(access, error, loglevel))
#endif
        };

        struct ApiObject {
            string tag;
            list<string> services;
            ApiObject() : tag(), services() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(tag, services))
#endif
        };
        namespace DNSObjects
        {
            struct ServerObject {
                string address;
                int port;
                list<string> domains;
                ServerObject(): address(), port(), domains() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(address, port, domains))
#endif
            };
        }
        struct DnsObject {
            map<string, string> hosts;
#if USE_TODO_FEATURES
            tuple<string, string, list<DNSObjects::ServerObject>> servers;
#else
            // Currently does not support ServerObject as tuple is.... quite complicated...
            list<string> servers;
#endif
            DnsObject(): hosts(), servers() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(hosts, servers))
#endif
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
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(type, domain, ip, port, network, source, user, inboundTag, protocol, attrs))
#endif
            };
            struct BalancerObject {
                string tag ;
                list<string> selector;
                BalancerObject() : tag(), selector() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(tag, selector))
#endif
            };
        }

        struct RoutingObject {
            string domainStrategy;
            list<ROUTINGObjects::RuleObject> rules;
            list<ROUTINGObjects::BalancerObject> balancers;
            RoutingObject() : domainStrategy(), rules(), balancers() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(domainStrategy, rules, balancers))
#endif
        };
        namespace POLICYObjects
        {
            struct SystemPolicyObject {
                bool statsInboundUplink;
                bool statsInboundDownlink;
                SystemPolicyObject() : statsInboundUplink(), statsInboundDownlink() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(statsInboundUplink, statsInboundDownlink))
#endif
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
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(handshake, connIdle, uplinkOnly, downlinkOnly, statsUserUplink, statsUserDownlink, bufferSize))
#endif
            };
        }
        struct PolicyObject {
            map<int, POLICYObjects::LevelPolicyObject> level;
            list<POLICYObjects::SystemPolicyObject> system;
            PolicyObject(): level(), system() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(level, system))
#endif
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
#if USE_TODO_FEATURES == false
                    XTOSTRUCT(O(version, method, path, headers))
#endif
                };

                struct HTTPResponseObject {
                    string version;
                    string status;
                    string reason;
                    map<string, list<string>> headers;
                    HTTPResponseObject(): version(), status(), reason(), headers() {}
#if USE_TODO_FEATURES == false
                    XTOSTRUCT(O(version, status, reason, headers))
#endif
                };
                struct TCPHeader_M_Object {
                    string type;
                    HTTPRequestObject request;
                    HTTPResponseObject response;
                    TCPHeader_M_Object(): type(), request(), response() {}
#if USE_TODO_FEATURES == false
                    XTOSTRUCT(O(type, request, response))
#endif
                };
                struct HeaderObject {
                    string type;
                    HeaderObject(): type() {}
#if USE_TODO_FEATURES == false
                    XTOSTRUCT(O(type))
#endif
                };
            }


            struct TCPObject {
                TRANSFERObjectsInternal:: TCPHeader_M_Object header;
                TCPObject(): header() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(header))
#endif
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
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header))
#endif
            };


            struct WebSocketObject {
                string path;
                map<string, string> headers;
                WebSocketObject(): path(), headers() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(path, headers))
#endif
            };

            struct HttpObject {
                list<string> host;
                string path;
                HttpObject() : host(), path() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(host, path))
#endif
            };

            struct DomainSocketObject {
                string path;
                DomainSocketObject(): path() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(path))
#endif
            };

            struct QuicObject {
                string security;
                string key;
                TRANSFERObjectsInternal::HeaderObject header;
                QuicObject(): security(), key(), header() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(security, key, header))
#endif
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
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings))
#endif
        };
        namespace INBOUNDObjects
        {

            struct SniffingObject {
                bool enabled;
                string destOverride;
                SniffingObject(): enabled(), destOverride() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(enabled, destOverride))
#endif
            };

            struct AllocateObject {
                string strategy;
                int refresh;
                int concurrency;
                AllocateObject(): strategy(), refresh(), concurrency() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(strategy, refresh, concurrency))
#endif
            };
        }
        namespace STREAMSETTINGSObjects
        {
            struct SockoptObject {
                int mark;
                bool tcpFastOpen;
                string tproxy;
                SockoptObject(): mark(), tcpFastOpen(), tproxy() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(mark, tcpFastOpen, tproxy))
#endif
            };

            struct CertificateObject {
                string usage;
                string certificateFile;
                string keyFile;
                list<string> certificate;
                list<string> key;
                CertificateObject(): usage(), certificateFile(), keyFile(), certificate(), key() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(usage, certificateFile, keyFile, certificate, key))
#endif
            };

            struct TLSObject {
                string serverName;
                bool allowInsecure;
                list<string> alpn;
                list<CertificateObject> certificates;
                bool disableSystemRoot;
                TLSObject(): serverName(), allowInsecure(), certificates(), disableSystemRoot() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(serverName, allowInsecure, alpn, certificates, disableSystemRoot))
#endif
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
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(network, security, sockopt, tcpSettings, tlsSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings))
#endif
        };

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
            INBOUNDObjects::AllocateObject allocate;
            InboundObject(): port(), listen(), protocol(), settings(), streamSettings(), tag(), sniffing(), allocate() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(port, listen, protocol, settings, streamSettings, tag, sniffing, allocate))
#endif
        };
        namespace OUTBOUNDObjects
        {

            struct ProxySettingsObject {
                string tag;
                ProxySettingsObject(): tag() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(tag))
#endif
            };

            struct MuxObject {
                bool enabled;
                int concurrency;
                MuxObject(): enabled(), concurrency() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(enabled, concurrency))
#endif
            };
        }
        template <typename XOUTBOUNDSETTINGOBJECT>
        struct OutboundObject {
            static_assert(std::is_base_of<XOutBoundsType, XOUTBOUNDSETTINGOBJECT>::value, "XOUTBOUNDSETTINGOBJECT must extend XOutBoundsType");
            string sendThrough;
            string protocol;
            XOUTBOUNDSETTINGOBJECT settings;
            string tag;
            StreamSettingsObject streamSettings;
            OUTBOUNDObjects::ProxySettingsObject proxySettings;
            OUTBOUNDObjects::MuxObject mux;
            OutboundObject(): sendThrough(), protocol(), settings(), tag(), streamSettings(), proxySettings(), mux() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(sendThrough, protocol, settings, tag, streamSettings, proxySettings, mux))
#endif
        };

        struct StatsObject {
            bool _; // Placeholder...
            StatsObject(): _() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(_))
#endif
        };
        namespace REVERSEObjects
        {

            struct BridgeObject {
                string tag;
                string domain;
                BridgeObject() : tag(), domain() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(tag, domain))
#endif
            };

            struct PortalObject {
                string tag;
                string domain;
                PortalObject() : tag(), domain() {}
#if USE_TODO_FEATURES == false
                XTOSTRUCT(O(tag, domain))
#endif
            };

        }
        struct ReverseObject {
            list<REVERSEObjects::BridgeObject> bridges;
            list<REVERSEObjects::PortalObject> portals;
            ReverseObject() : bridges(), portals() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(bridges, portals))
#endif
        };
#if USE_TODO_FEATURES
        template<typename XIN1, typename XIN2, typename XIN3, typename XIN4, typename XIN5, typename XOUT1, typename XOUT2, typename XOUT3, typename XOUT4, typename XOUT5>
#else
        template <typename XInBound_T, typename XOutBound_T>
#endif
        struct RootObject {
            LogObject log;
            ApiObject api;
            DnsObject dns;
            RoutingObject routing;
#if USE_TODO_FEATURES
            // Default support 5 inBounds and 5 outBounds
            tuple<InboundObject<XIN1>, InboundObject<XIN2>, InboundObject<XIN3>, InboundObject<XIN4>, InboundObject<XIN5>> inbounds;
            tuple<OutboundObject<XIN1>, OutboundObject<XIN2>, OutboundObject<XIN3>, OutboundObject<XIN4>, OutboundObject<XIN5>> outbounds;
#else
            list<InboundObject<XInBound_T>> inbounds;
            list<OutboundObject<XOutBound_T>> outbounds;
#endif
            TransportObject transport;
            StatsObject stats;
            ReverseObject reverse;
            PolicyObject policy;
            RootObject(): log(), api(), dns(), routing(), inbounds(), outbounds(), transport(), stats(), reverse(), policy() {}
#if USE_TODO_FEATURES == false
            XTOSTRUCT(O(log, api, dns, routing, inbounds, outbounds, transport, stats, reverse, policy))
#endif
        };
    }
}

namespace Hv2ray
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

#if USE_TODO_FEATURES
using namespace Hv2ray;
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::LogObject, access, error, loglevel)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::ApiObject, tag, services)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::DNSObjects::ServerObject, address, port, domains)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::DnsObject, hosts, servers)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::ROUTINGObjects::RuleObject, type, domain, ip, port, network, source, user, inboundTag, protocol, attrs)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::ROUTINGObjects::BalancerObject, tag, selector)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::RoutingObject, domainStrategy, rules, balancers)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::POLICYObjects::SystemPolicyObject, statsInboundUplink, statsInboundDownlink)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::POLICYObjects::LevelPolicyObject, handshake, connIdle, uplinkOnly, downlinkOnly, statsUserUplink, statsUserDownlink, bufferSize)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::PolicyObject, level, system)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::TRANSFERObjectsInternal::HTTPRequestObject, version, method, path, headers)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::TRANSFERObjectsInternal::HTTPResponseObject, version, status, reason, headers)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::TRANSFERObjectsInternal::TCPHeader_M_Object, type, request, response)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::TRANSFERObjectsInternal::HeaderObject, type)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::TCPObject, header)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::KCPObject, mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::WebSocketObject, path, headers)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::HttpObject, host, path)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::DomainSocketObject, path)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TRANSFERObjects::QuicObject, security, key, header)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::TransportObject, tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::INBOUNDObjects::SniffingObject, enabled, destOverride)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::INBOUNDObjects::AllocateObject, strategy, refresh, concurrency)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::OUTBOUNDObjects::ProxySettingsObject, tag)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::OUTBOUNDObjects::MuxObject, enabled, concurrency)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::REVERSEObjects::BridgeObject, tag, domain)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::REVERSEObjects::PortalObject, tag, domain)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::ReverseObject, bridges, portals)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::StatsObject, _)

JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::StreamSettingsObject, tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings, tlsSettings, sockopt, network, security)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::STREAMSETTINGSObjects::TLSObject, serverName, allowInsecure, alpn, certificates, disableSystemRoot)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::STREAMSETTINGSObjects::CertificateObject, usage, certificateFile, keyFile, certificate, key)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::STREAMSETTINGSObjects::SockoptObject, mark, tcpFastOpen, tproxy)

// These 3 are used as templates.
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::RootObject, log, api, dns, routing, inbounds, outbounds, transport, stats, reverse, policy)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::InboundObject, port, listen, protocol, settings, streamSettings, tag, sniffing, allocate)
JSONCONS_MEMBER_TRAITS_DECL(V2ConfigModels::OutboundObject, sendThrough, protocol, settings, tag, streamSettings, proxySettings, mux)

#endif

/// Code above has passed these tests.
//using namespace  Hv2ray::V2ConfigModels;
//RootObject<Protocols::HTTP, Protocols::VMess> x;
//InboundObject<Protocols::HTTP> inH;
//x.inbounds.insert(x.inbounds.end(), inH);
//OutboundObject<Protocols::VMess> inV;
//x.outbounds.insert(x.outbounds.end(), inV);
//QString jsonConfig = Utils::StructToJSON(x);
//cout << jsonConfig.toStdString() << endl;
///
#endif
