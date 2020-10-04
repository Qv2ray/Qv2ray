#pragma once
#include "CommonTypes.hpp"
#include "QvPluginProcessor.hpp"

class BuiltinSerializer : public Qv2rayPlugin::PluginOutboundHandler
{
  public:
    explicit BuiltinSerializer() : Qv2rayPlugin::PluginOutboundHandler(){};
    const QString SerializeOutbound(const QString &protocol, const QString &alias, const QString &group, const QJsonObject &obj) const override;
    const QPair<QString, QJsonObject> DeserializeOutbound(const QString &link, QString *alias, QString *errorMessage) const override;
    const Qv2rayPlugin::OutboundInfoObject GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const override;
    const QList<QString> SupportedLinkPrefixes() const override;
    const QList<QString> SupportedProtocols() const override
    {
        return { "http", "socks", "shadowsocks", "vmess", "vless" };
    }
};
