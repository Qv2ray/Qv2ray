#pragma once
#include "QvPluginProcessor.hpp"
#include "common/CommonTypes.hpp"

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
            const auto http = HttpServerObject::fromJson(outbound);
            obj[Qv2rayPlugin::INFO_SERVER] = http.address;
            obj[Qv2rayPlugin::INFO_PORT] = http.port;
        }
        else if (protocol == "socks")
        {
            const auto socks = SocksServerObject::fromJson(outbound);
            obj[Qv2rayPlugin::INFO_SERVER] = socks.address;
            obj[Qv2rayPlugin::INFO_PORT] = socks.port;
        }
        return obj;
    }
    const QList<QString> SupportedProtocols() const override
    {
        return { "dokodemo-door", "dns", "freedom", "http", "socks" };
    }
    const QList<QString> SupportedLinkPrefixes() const override
    {
        return {};
    }
};
