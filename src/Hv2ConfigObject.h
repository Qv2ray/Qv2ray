#include <QDir>
#include <list>
#include <string>
#include <jsoncons/json.hpp>
#include <string>

#define USE_JSON_CONS
#ifdef USE_JSON_CONS
#include <jsoncons/json.hpp>
#else
#include <x2struct/x2struct.hpp>
using namespace x2struct;
#endif
#ifndef V2CONFIG_H
#define V2CONFIG_H

// Macros
#define HV2RAY_CONFIG_DIR_NAME "/.hv2ray"

using namespace std;
using namespace jsoncons; // for convenience
/*------------------------------------------------------------------------------------------------------------*/

namespace Hv2ray
{
    namespace V2ConfigModels
    {

        struct LogObject {
            string access;
            string error;
            string loglevel;
            LogObject(): access(), error(), loglevel() {}
            //XTOSTRUCT(O(access, error, loglevel))
        };

        struct ApiObject {
            string tag;
            list<string> services;
            ApiObject() : tag(), services() {}
            //XTOSTRUCT(O(tag, services))
        };

        struct ServerObject {
            string address;
            int port;
            list<string> domains;
            ServerObject(): address(), port(), domains() {}
            //XTOSTRUCT(O(address, port, domains))
        };

        struct DnsObject {
            map<string, string> hosts;
            tuple<string, string, list<ServerObject>> servers;
            DnsObject(): hosts(), servers() {}
            //XTOSTRUCT(O(hosts, servers))
        };

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
            //XTOSTRUCT(O(type, domain, ip, port, network, source, user, inboundTag, protocol, attrs))
        };
        struct BalancerObject {
            string tag ;
            list<string> selector;
            BalancerObject() : tag(), selector() {}
            //XTOSTRUCT(O(tag, selector))
        };

        struct RoutingObject {
            string domainStrategy;
            list<RuleObject> rules;
            list<BalancerObject> balancers;
            RoutingObject() : domainStrategy(), rules(), balancers() {}
            //XTOSTRUCT(O(domainStrategy, rules, balancers))
        };

        struct SystemPolicyObject {
            bool statsInboundUplink;
            bool statsInboundDownlink;
            SystemPolicyObject() : statsInboundUplink(), statsInboundDownlink() {}
            //XTOSTRUCT(O(statsInboundUplink, statsInboundDownlink))
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
            //XTOSTRUCT(O(handshake, connIdle, uplinkOnly, downlinkOnly, statsUserUplink, statsUserDownlink, bufferSize))
        };
        struct PolicyObject {
            map<int, LevelPolicyObject> level;
            list<SystemPolicyObject> system;
            PolicyObject(): level(), system() {}
            //XTOSTRUCT(O(level, system))
        };

        struct HTTPRequestObject {
            string version;
            string method;
            list<string> path;
            map<string, list<string>> headers;
            HTTPRequestObject(): version(), method(), path(), headers() {}
            //XTOSTRUCT(O(version, method, path, headers))
        };

        struct HTTPResponseObject {
            string version;
            string status;
            string reason;
            map<string, list<string>> headers;
            HTTPResponseObject(): version(), status(), reason(), headers() {}
            //XTOSTRUCT(O(version, status, reason, headers))
        };

        struct TCPHeader_M_Object {
            string type;
            HTTPRequestObject request;
            HTTPResponseObject response;
            TCPHeader_M_Object(): type(), request(), response() {}
            //XTOSTRUCT(O(type, request, response))
        };

        struct TCPObject {
            TCPHeader_M_Object header;
            TCPObject(): header() {}
            //XTOSTRUCT(O(header))
        };

        struct HeaderObject {
            string type;
            HeaderObject(): type() {}
            //XTOSTRUCT(O(type))
        };

        struct KCPObject {
            int mtu;
            int tti;
            int uplinkCapacity;
            int downlinkCapacity;
            bool congestion;
            int readBufferSize;
            int writeBufferSize;
            HeaderObject header;
            KCPObject(): mtu(), tti(), uplinkCapacity(), downlinkCapacity(), congestion(), readBufferSize(), writeBufferSize(), header() {}
            //XTOSTRUCT(O(mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header))
        };


        struct WebSocketObject {
            string path;
            map<string, string> headers;
            WebSocketObject(): path(), headers() {}
            //XTOSTRUCT(O(path, headers))
        };

        struct HttpObject {
            list<string> host;
            string path;
            HttpObject() : host(), path() {}
            //XTOSTRUCT(O(host, path))
        };

        struct DomainSocketObject {
            string path;
            DomainSocketObject(): path() {}
            //XTOSTRUCT(O(path))
        };

        struct QuicObject {
            string security;
            string key;
            HeaderObject header;
            QuicObject(): security(), key(), header() {}
            //XTOSTRUCT(O(security, key, header))
        };

        struct TransportObject {
            TCPObject tcpSettings;
            KCPObject kcpSettings;
            WebSocketObject wsSettings;
            HttpObject httpSettings;
            DomainSocketObject dsSettings;
            QuicObject quicSettings;
            TransportObject(): tcpSettings(), kcpSettings(), wsSettings(), httpSettings(), dsSettings(), quicSettings() {}
            //XTOSTRUCT(O(tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings))
        };

        struct SniffingObject {
            bool enabled;
            string destOverride;
            SniffingObject(): enabled(), destOverride() {}
            //XTOSTRUCT(O(enabled, destOverride))
        };

        struct AllocateObject {
            string strategy;
            int refresh;
            int concurrency;
            AllocateObject(): strategy(), refresh(), concurrency() {}
            //XTOSTRUCT(O(strategy, refresh, concurrency))
        };

        struct InboundObject {
            int port;
            string listen;
            string protocol;
            string settings;
            TransportObject streamSettings;
            string tag;
            SniffingObject sniffing;
            AllocateObject allocate;
            InboundObject(): port(), listen(), protocol(), settings(), streamSettings(), tag(), sniffing(), allocate() {}
            //XTOSTRUCT(O(port, listen, protocol, settings, streamSettings, tag, sniffing, allocate))
        };

        struct ProxySettingsObject {
            string tag;
            ProxySettingsObject(): tag() {}
            //XTOSTRUCT(O(tag))
        };

        struct MuxObject {
            bool enabled;
            int concurrency;
            MuxObject(): enabled(), concurrency() {}
            //XTOSTRUCT(O(enabled, concurrency))
        };

        struct OutboundObject {
            string sendThrough;
            string protocol;
            string settings;
            string tag;
            TransportObject streamSettings;
            ProxySettingsObject proxySettings;
            MuxObject mux;
            OutboundObject(): sendThrough(), protocol(), settings(), tag(), streamSettings(), proxySettings(), mux() {}
            //XTOSTRUCT(O(sendThrough, protocol, settings, tag, streamSettings, proxySettings, mux))
        };

        struct StatsObject {
            bool _; // Placeholder...
            StatsObject(): _() {}
        };

        struct BridgeObject {
            string tag;
            string domain;
            BridgeObject() : tag(), domain() {}
            //XTOSTRUCT(O(tag, domain))
        };

        struct PortalObject {
            string tag;
            string domain;
            PortalObject() : tag(), domain() {}
            //XTOSTRUCT(O(tag, domain))
        };

        struct ReverseObject {
            list<BridgeObject> bridges;
            list<PortalObject> portals;
            ReverseObject() : bridges(), portals() {}
            //XTOSTRUCT(O(bridges, portals))
        };
        struct RootObject {
            LogObject log;
            ApiObject api;
            DnsObject dns;
            RoutingObject routing;
            list<InboundObject> inbounds;
            list<OutboundObject> outbounds;
            TransportObject transport;
            StatsObject stats;
            ReverseObject reverse;
            RootObject(): log(), api(), dns(), routing(), inbounds(), outbounds(), transport(), stats(), reverse() {}
            //XTOSTRUCT(O(log, api, dns, routing, inbounds, outbounds, transport, stats, reverse))
        };
    }
}
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::LogObject, access, error, loglevel)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::ApiObject, tag, services)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::ServerObject, address, port, domains)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::DnsObject, hosts, servers)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::RuleObject, type, domain, ip, port, network, source, user, inboundTag, protocol, attrs)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::BalancerObject, tag, selector)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::RoutingObject, domainStrategy, rules, balancers)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::SystemPolicyObject, statsInboundUplink, statsInboundDownlink)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::LevelPolicyObject, handshake, connIdle, uplinkOnly, downlinkOnly, statsUserUplink, statsUserDownlink, bufferSize)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::PolicyObject, level, system)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::HTTPRequestObject, version, method, path, headers)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::HTTPResponseObject, version, status, reason, headers)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::TCPHeader_M_Object, type, request, response)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::TCPObject, header)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::HeaderObject, type)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::KCPObject, mtu, tti, uplinkCapacity, downlinkCapacity, congestion, readBufferSize, writeBufferSize, header)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::WebSocketObject, path, headers)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::HttpObject, host, path)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::DomainSocketObject, path)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::QuicObject, security, key, header)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::TransportObject, tcpSettings, kcpSettings, wsSettings, httpSettings, dsSettings, quicSettings)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::SniffingObject, enabled, destOverride)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::AllocateObject, strategy, refresh, concurrency)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::InboundObject, port, listen, protocol, settings, streamSettings, tag, sniffing, allocate)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::ProxySettingsObject, tag)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::MuxObject, enabled, concurrency)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::OutboundObject, sendThrough, protocol, settings, tag, streamSettings, proxySettings, mux)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::StatsObject, _)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::BridgeObject, tag, domain)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::PortalObject, tag, domain)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::ReverseObject, bridges, portals)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::V2ConfigModels::RootObject, log, api, dns, routing, inbounds, outbounds, transport, stats, reverse)


namespace Hv2ray
{
    namespace HConfigModels
    {
        struct HInbondSetting {
            bool enabled;
            string ip;
            int port;
            bool useAuthentication;
            string authUsername;
            string authPassword;
            HInbondSetting() {}
            HInbondSetting(bool _enabled, string _ip, int _port)
                : ip(_ip), authUsername(""), authPassword("")
            {
                enabled = _enabled;
                port = _port;
                useAuthentication = false;
            }
            HInbondSetting(bool _enabled, string _ip, int _port, string _username, string _password)
                : ip(_ip),  authUsername(_username), authPassword(_password)
            {
                enabled = _enabled;
                port = _port;
                useAuthentication = true;
            }
            //XTOSTRUCT(O(enabled, ip, port, useAuthentication, authUsername, authPassword))
        };

        struct Hv2Config {
            string language;
            bool runAsRoot;
            string logLevel;
            //Hv2ray::V2ConfigModels::MuxObject muxSetting;
            HInbondSetting httpSetting;
            HInbondSetting socksSetting;
            Hv2Config() {}
            Hv2Config(string lang, const bool _runAsRoot, const string _loglevel, HInbondSetting _http, HInbondSetting _socks)
                : httpSetting(_http),
                  socksSetting(_socks)

            {
                language = lang;
                runAsRoot = _runAsRoot;
                logLevel = _loglevel;
            }
            //XTOSTRUCT(O(language, runAsRoot, logLevel, httpSetting, socksSetting))
        };
    }
}
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::HConfigModels::Hv2Config, language, runAsRoot, logLevel, httpSetting, socksSetting)
JSONCONS_MEMBER_TRAITS_DECL(Hv2ray::HConfigModels::HInbondSetting, enabled, ip, port, useAuthentication, authUsername, authPassword)

namespace Hv2ray
{
    /// ConfigGlobalConfigthis is platform-independent as it's solved to be in the best
    /// place at first in main.cpp
    static QDir ConfigDir;
    static HConfigModels::Hv2Config GlobalConfig;
}
#endif // V2CONFIG_H

