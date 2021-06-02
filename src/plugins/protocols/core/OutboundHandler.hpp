#pragma once
#include "CommonTypes.hpp"
#include "QvPluginInterface.hpp"

class BuiltinSerializer : public PluginOutboundHandler
{
  public:
    explicit BuiltinSerializer() : PluginOutboundHandler(){};

    std::optional<QString> Serialize(const PluginOutboundDescriptor &info) const override;
    std::optional<PluginOutboundDescriptor> Deserialize(const QString &link) const override;

    std::optional<PluginIOBoundData> GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const override;
    bool SetOutboundInfo(const QString &protocol, QJsonObject &outbound, const PluginIOBoundData &info) const override;

    QList<QString> SupportedLinkPrefixes() const override
    {
        return { "http", "socks" };
    }

    QList<QString> SupportedProtocols() const override
    {
        return { "http", "socks", "shadowsocks", "vmess", "vless" };
    }
};
