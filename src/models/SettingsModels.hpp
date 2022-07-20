#pragma once

#include "QvPlugin/Common/CommonTypes.hpp"
#include "src/plugins/PluginsCommon/V2RayModels.hpp"

#include <QJsonObject>
#include <QString>

namespace Qv2ray::Models
{
    struct Qv2rayAppearanceConfig
    {
        enum UIStyleType
        {
            AUTO,
            ALWAYS_ON,
            ALWAYS_OFF
        };
#ifdef Q_OS_WIN
        const static inline QString _system_theme = "windowsvista";
#elif defined(Q_OS_MACOS)
        const static inline QString _system_theme = "macintosh";
#else
        const static inline QString _system_theme = "Fusion";
#endif
        Bindable<UIStyleType> DarkModeTrayIcon;
        Bindable<QString> UITheme{ _system_theme };
        Bindable<qsizetype> RecentJumpListSize{ 10 };
        Bindable<QList<ProfileId>> RecentConnections;
        Bindable<int> MaximizeLogLines{ 500 };
        Bindable<bool> ShowTrayIcon{ true };
        QJS_COMPARE(Qv2rayAppearanceConfig, DarkModeTrayIcon, UITheme, RecentJumpListSize, RecentConnections, MaximizeLogLines, ShowTrayIcon)
        QJS_JSON(P(DarkModeTrayIcon, UITheme, RecentJumpListSize, RecentConnections, MaximizeLogLines, ShowTrayIcon))
    };

    struct Qv2rayBehaviorConfig
    {
        enum AutoConnectBehavior
        {
            AUTOCONNECT_NONE = 0,
            AUTOCONNECT_FIXED = 1,
            AUTOCONNECT_LAST_CONNECTED = 2
        };
        Bindable<LatencyTestEngineId> DefaultLatencyTestEngine;
        Bindable<KernelId> DefaultKernelId;
        Bindable<AutoConnectBehavior> AutoConnectBehavior{ AUTOCONNECT_LAST_CONNECTED };
        Bindable<bool> QuietMode{ false };
        Bindable<ProfileId> AutoConnectProfileId;
        Bindable<ProfileId> LastConnectedId;
        Bindable<QString> GeoIPPath;
        Bindable<QString> GeoSitePath;
        QJS_COMPARE(Qv2rayBehaviorConfig, DefaultLatencyTestEngine, DefaultKernelId, AutoConnectBehavior, QuietMode, AutoConnectProfileId, LastConnectedId, GeoIPPath,
                    GeoSitePath)
        QJS_JSON(P(DefaultLatencyTestEngine, DefaultKernelId, AutoConnectBehavior, QuietMode, AutoConnectProfileId, LastConnectedId, GeoIPPath, GeoSitePath))
    };

    struct ProtocolInboundBase
    {
        enum SniffingBehavior
        {
            SNIFFING_OFF = 0,
            SNIFFING_METADATA_ONLY = 1,
            SNIFFING_FULL = 2
        };

        Bindable<int> ListenPort;
        Bindable<SniffingBehavior> Sniffing{ SNIFFING_OFF };
        Bindable<QList<QString>> DestinationOverride{ { "http", "tls" } };
        ProtocolInboundBase(int port = 0) : ListenPort(port){};
        QJS_JSON(P(ListenPort, Sniffing, DestinationOverride))

        virtual void Propagate(InboundObject &in) const
        {
            in.inboundSettings.port = ListenPort;
            in.options[u"sniffing"_qs] = QJsonObject{
                { u"enabled"_qs, Sniffing != SNIFFING_OFF },                             //
                { u"metadataOnly"_qs, Sniffing == SNIFFING_METADATA_ONLY },              //
                { u"destOverride"_qs, QJsonArray::fromStringList(DestinationOverride) }, //
            };
        }
    };

    struct SocksInboundConfig : public ProtocolInboundBase
    {
        Bindable<bool> EnableUDP{ true };
        Bindable<QString> UDPLocalAddress;
        SocksInboundConfig() : ProtocolInboundBase(1089){};

        QJS_COMPARE(SocksInboundConfig, EnableUDP, UDPLocalAddress, ListenPort, Sniffing, DestinationOverride)
        QJS_JSON(P(EnableUDP, UDPLocalAddress), B(ProtocolInboundBase))

        virtual void Propagate(InboundObject &in) const
        {
            ProtocolInboundBase::Propagate(in);
            in.inboundSettings.protocolSettings[u"udp"_qs] = *EnableUDP;
            if (!UDPLocalAddress->isEmpty())
            {
                in.inboundSettings.protocolSettings[u"ip"_qs] = *UDPLocalAddress;
            }
        }
    };

    struct HTTPInboundConfig : public ProtocolInboundBase
    {
        HTTPInboundConfig() : ProtocolInboundBase(8889){};
        QJS_COMPARE(HTTPInboundConfig, ListenPort, Sniffing, DestinationOverride)
        QJS_JSON(B(ProtocolInboundBase))
    };

    struct DokodemoDoorInboundConfig : public ProtocolInboundBase
    {
        enum DokoWorkingMode
        {
            REDIRECT,
            TPROXY,
        };
        Bindable<DokoWorkingMode> WorkingMode{ TPROXY };
        DokodemoDoorInboundConfig() : ProtocolInboundBase(12345){};

        QJS_COMPARE(DokodemoDoorInboundConfig, WorkingMode, ListenPort, Sniffing, DestinationOverride)
        QJS_JSON(P(WorkingMode), B(ProtocolInboundBase))

        virtual void Propagate(InboundObject &in) const
        {
            ProtocolInboundBase::Propagate(in);
            in.inboundSettings.protocolSettings[u"network"_qs] = u"tcp,udp"_qs;
            in.inboundSettings.protocolSettings[u"followRedirect"_qs] = true;
        }
    };

    struct Qv2rayInboundConfig
    {
        Bindable<QString> ListenAddress1{ "127.0.0.1" };
        Bindable<QString> ListenAddress2{ "::1" };

        Bindable<bool> HasSOCKS{ true };
        Bindable<SocksInboundConfig> SOCKSConfig;

        Bindable<bool> HasHTTP{ true };
        Bindable<HTTPInboundConfig> HTTPConfig;

        Bindable<bool> HasDokodemoDoor{ false };
        Bindable<DokodemoDoorInboundConfig> DokodemoDoorConfig;

        QJS_COMPARE(Qv2rayInboundConfig, ListenAddress1, ListenAddress2, HasSOCKS, SOCKSConfig, HasHTTP, HTTPConfig, HasDokodemoDoor, DokodemoDoorConfig)
        QJS_JSON(P(ListenAddress1, ListenAddress2, HasSOCKS, SOCKSConfig, HasHTTP, HTTPConfig, HasDokodemoDoor, DokodemoDoorConfig))
    };

    struct Qv2rayConnectionConfig
    {
        Bindable<bool> BypassLAN{ true };
        Bindable<bool> BypassCN{ true };
        Bindable<bool> BypassBittorrent{ false };
        Bindable<bool> ForceDirectConnection{ false };
        Bindable<bool> UseDirectOutboundAsPrimary{ false };
        Bindable<bool> DNSInterception{ false };

        QJS_COMPARE(Qv2rayConnectionConfig, BypassLAN, BypassCN, BypassBittorrent, ForceDirectConnection, UseDirectOutboundAsPrimary, DNSInterception)
        QJS_JSON(P(BypassLAN, BypassCN, BypassBittorrent, ForceDirectConnection, UseDirectOutboundAsPrimary, DNSInterception))
    };

    struct Qv2rayUpdateConfig
    {
        Bindable<QString> IgnoredVersion;
        Bindable<int> UpdateChannel;
        QJS_COMPARE(Qv2rayUpdateConfig, IgnoredVersion, UpdateChannel)
        QJS_JSON(P(IgnoredVersion, UpdateChannel))
    };

    struct Qv2rayApplicationConfigObject
    {
        Bindable<Qv2rayAppearanceConfig> appearanceConfig;
        Bindable<Qv2rayBehaviorConfig> behaviorConfig;
        Bindable<Qv2rayConnectionConfig> connectionConfig;
        Bindable<Qv2rayInboundConfig> inboundConfig;
        Bindable<Qv2rayUpdateConfig> updateConfig;

        QJS_COMPARE(Qv2rayApplicationConfigObject, appearanceConfig, behaviorConfig, connectionConfig, inboundConfig, updateConfig)
        QJS_JSON(P(appearanceConfig, behaviorConfig, connectionConfig, inboundConfig, updateConfig))
    };

} // namespace Qv2ray::Models

using namespace Qv2ray::Models;
