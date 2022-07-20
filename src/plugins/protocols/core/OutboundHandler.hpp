#pragma once

#include "QvPlugin/Handlers/OutboundHandler.hpp"

class BuiltinSerializer : public Qv2rayPlugin::Outbound::IOutboundProcessor
{
  public:
    explicit BuiltinSerializer() : Qv2rayPlugin::Outbound::IOutboundProcessor(){};

    virtual std::optional<QString> Serialize(const QString &name, const IOConnectionSettings &outbound) const override;
    virtual std::optional<std::pair<QString, IOConnectionSettings>> Deserialize(const QString &link) const override;

    virtual std::optional<PluginIOBoundData> GetOutboundInfo(const IOConnectionSettings &) const override;
    virtual bool SetOutboundInfo(IOConnectionSettings &, const PluginIOBoundData &) const override;

    QList<QString> SupportedLinkPrefixes() const override
    {
        return { "http", "socks", "vmess", "ss", "trojan" };
    }

    QList<QString> SupportedProtocols() const override
    {
        return { "http", "socks", "shadowsocks", "vmess", "trojan" };
    }
};
