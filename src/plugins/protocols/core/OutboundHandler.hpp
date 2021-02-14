#pragma once
#include "CommonTypes.hpp"
#include "QvPluginInterface.hpp"

class BuiltinSerializer : public PluginOutboundHandler
{
  public:
    explicit BuiltinSerializer() : PluginOutboundHandler(){};
    const QString SerializeOutbound(const QString &protocol, const QString &name, const QString &group, const QJsonObject &obj,
                                    const QJsonObject &stream) const override;
    const QPair<QString, QJsonObject> DeserializeOutbound(const QString &link, QString *alias, QString *errorMessage) const override;
    const OutboundInfoObject GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const override;
    void SetOutboundInfo(const QString &protocol, const OutboundInfoObject &info, QJsonObject &outbound) const override;
    const QList<QString> SupportedLinkPrefixes() const override;
    const QList<QString> SupportedProtocols() const override
    {
        return { "http", "socks", "shadowsocks", "vmess", "vless" };
    }
};
