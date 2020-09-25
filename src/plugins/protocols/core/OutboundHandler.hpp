#pragma once
#include "CommonTypes.hpp"
#include "QvPluginProcessor.hpp"

class BuiltinSerializer : public Qv2rayPlugin::PluginOutboundHandler
{
  public:
    explicit BuiltinSerializer() : Qv2rayPlugin::PluginOutboundHandler(){};
    const QString SerializeOutbound(const QString &protocol, const QString &alias, const QString &group, const QJsonObject &obj) const override
    {
        Q_UNUSED(protocol)
        Q_UNUSED(alias)
        Q_UNUSED(group)
        Q_UNUSED(obj)
        return "(Not Supported)";
    }

    const QPair<QString, QJsonObject> DeserializeOutbound(const QString &link, QString *alias, QString *errorMessage) const override
    {
        Q_UNUSED(link)
        Q_UNUSED(alias)
        Q_UNUSED(errorMessage)
        Q_UNREACHABLE();
    }

    const Qv2rayPlugin::OutboundInfoObject GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const override
    {
        Qv2rayPlugin::OutboundInfoObject obj;
        obj[Qv2rayPlugin::INFO_PROTOCOL] = protocol;
        if (protocol == "http")
        {
            const auto http = HttpServerObject::fromJson(outbound["servers"].toArray().first());
            obj[Qv2rayPlugin::INFO_SERVER] = http.address;
            obj[Qv2rayPlugin::INFO_PORT] = http.port;
        }
        else if (protocol == "socks")
        {
            const auto socks = SocksServerObject::fromJson(outbound["servers"].toArray().first());
            obj[Qv2rayPlugin::INFO_SERVER] = socks.address;
            obj[Qv2rayPlugin::INFO_PORT] = socks.port;
        }
        else if (protocol == "vmess")
        {
            const auto vmess = VMessServerObject::fromJson(outbound["vnext"].toArray().first());
            obj[Qv2rayPlugin::INFO_SERVER] = vmess.address;
            obj[Qv2rayPlugin::INFO_PORT] = vmess.port;
        }
        else if (protocol == "vless")
        {
            const auto vless = VLESSServerObject::fromJson(outbound["vnext"].toArray().first());
            obj[Qv2rayPlugin::INFO_SERVER] = vless.address;
            obj[Qv2rayPlugin::INFO_PORT] = vless.port;
        }
        else if (protocol == "shadowsocks")
        {
            const auto ss = ShadowSocksServerObject::fromJson(outbound["servers"].toArray().first());
            obj[Qv2rayPlugin::INFO_SERVER] = ss.address;
            obj[Qv2rayPlugin::INFO_PORT] = ss.port;
        }
        return obj;
    }

    const QList<QString> SupportedProtocols() const override
    {
        return { "http", "socks", "shadowsocks", "vmess", "vless" };
    }

    const QList<QString> SupportedLinkPrefixes() const override
    {
        return {};
    }
};
