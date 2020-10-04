#include "Serialization.hpp"

#include "Generation.hpp"
#include "components/plugins/QvPluginHost.hpp"

namespace Qv2ray::core::connection
{
    namespace serialization
    {
        QList<QPair<QString, CONFIGROOT>> ConvertConfigFromString(const QString &link, QString *aliasPrefix, QString *errMessage, QString *newGroup)
        {
            const auto TLSOptionsFilter = [](QJsonObject &conf) {
                auto allowInsecure = GlobalConfig.advancedConfig.setAllowInsecure;
                auto setSR = GlobalConfig.advancedConfig.setSessionResumption;
                auto disableSystemRoot = GlobalConfig.advancedConfig.disableSystemRoot;
                for (const QString &prefix : { "tls", "xtls" })
                {
                    QJsonIO::SetValue(conf, allowInsecure, { "outbounds", 0, "streamSettings", prefix + "Settings", "allowInsecure" });
                    QJsonIO::SetValue(conf, !setSR, { "outbounds", 0, "streamSettings", prefix + "Settings", "disableSessionResumption" });
                    QJsonIO::SetValue(conf, !disableSystemRoot, { "outbounds", 0, "streamSettings", prefix + "Settings", "disableSystemRoot" });
                }
            };

            QList<QPair<QString, CONFIGROOT>> connectionConf;
            if (link.startsWith("vmess://") && link.contains("@"))
            {
                auto conf = vmess_new::Deserialize(link, aliasPrefix, errMessage);
                TLSOptionsFilter(conf);
                connectionConf << QPair{ *aliasPrefix, conf };
            }
            else if (link.startsWith("vmess://"))
            {
                auto conf = vmess::Deserialize(link, aliasPrefix, errMessage);
                TLSOptionsFilter(conf);
                connectionConf << QPair{ *aliasPrefix, conf };
            }
            else if (link.startsWith("ss://") && !link.contains("plugin="))
            {
                auto conf = ss::Deserialize(link, aliasPrefix, errMessage);
                connectionConf << QPair{ *aliasPrefix, conf };
            }
            else if (link.startsWith("ssd://"))
            {
                QStringList errMessageList;
                connectionConf << ssd::Deserialize(link, newGroup, &errMessageList);
                *errMessage = errMessageList.join(NEWLINE);
            }
            else
            {
                bool ok = false;
                const auto configs = PluginHost->TryDeserializeShareLink(link, aliasPrefix, errMessage, newGroup, &ok);
                if (ok)
                {
                    errMessage->clear();
                    for (const auto &[_alias, _protocol, _outbound] : configs)
                    {
                        CONFIGROOT root;
                        auto outbound = GenerateOutboundEntry(OUTBOUND_TAG_PROXY, _protocol, OUTBOUNDSETTING(_outbound), {});
                        QJsonIO::SetValue(root, outbound, "outbounds", 0);
                        connectionConf << QPair{ _alias, root };
                    }
                }
                else if (errMessage->isEmpty())
                {
                    *errMessage = QObject::tr("Unsupported share link format.");
                }
            }

            return connectionConf;
        }

        const QString ConvertConfigToString(const ConnectionGroupPair &identifier, bool isSip002)
        {
            auto alias = GetDisplayName(identifier.connectionId);
            if (IsComplexConfig(identifier.connectionId))
            {
                DEBUG(MODULE_CONNECTION, "Ignored an complex config: " + alias)
                return QV2RAY_SERIALIZATION_COMPLEX_CONFIG_PLACEHOLDER;
            }
            auto server = ConnectionManager->GetConnectionRoot(identifier.connectionId);
            return ConvertConfigToString(alias, GetDisplayName(identifier.groupId), server, isSip002);
        }

        const QString ConvertConfigToString(const QString &alias, const QString &groupName, const CONFIGROOT &server, bool isSip002)
        {
            const auto outbound = OUTBOUND(server["outbounds"].toArray().first().toObject());
            const auto type = outbound["protocol"].toString();
            const auto settings = outbound["settings"].toObject();

            QString sharelink;

            if (type.isEmpty())
            {
                DEBUG(MODULE_CONNECTION, "WARNING: Empty outbound type.")
                return "";
            }

            if (type == "vmess")
            {
                const auto vmessServer = VMessServerObject::fromJson(settings["vnext"].toArray().first().toObject());
                const auto transport = StreamSettingsObject::fromJson(outbound["streamSettings"].toObject());
                if (GlobalConfig.uiConfig.useOldShareLinkFormat)
                    sharelink = vmess::Serialize(transport, vmessServer, alias);
                else
                    sharelink = vmess_new::Serialize(transport, vmessServer, alias);
            }
            else if (type == "shadowsocks")
            {
                auto ssServer = ShadowSocksServerObject::fromJson(settings["servers"].toArray().first().toObject());
                sharelink = ss::Serialize(ssServer, alias, isSip002);
            }
            else
            {
                bool ok = false;
                sharelink = PluginHost->SerializeOutbound(type, settings, alias, groupName, &ok);
                Q_UNUSED(ok)
            }

            return sharelink;
        }

    } // namespace serialization
} // namespace Qv2ray::core::connection
