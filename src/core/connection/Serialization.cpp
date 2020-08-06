#include "Serialization.hpp"

#include "Generation.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "core/CoreUtils.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "libs/QJsonStruct/QJsonIO.hpp"

namespace Qv2ray::core::connection
{
    namespace serialization
    {
        QList<QPair<QString, CONFIGROOT>> ConvertConfigFromString(const QString &link, QString *aliasPrefix, QString *errMessage,
                                                                  QString *newGroupName)
        {
            const auto mkAllowInsecure = [](QJsonObject &conf) {
                auto allowI = GlobalConfig.advancedConfig.setAllowInsecure;
                auto allowSR = GlobalConfig.advancedConfig.setSessionResumption;
                if (allowI || allowSR)
                {
                    QJsonIO::SetValue(conf, allowI, "outbounds", 0, "streamSettings", "tlsSettings", "allowInsecure");
                    QJsonIO::SetValue(conf, !allowSR, "outbounds", 0, "streamSettings", "tlsSettings", "disableSessionResumption");
                }
            };

            QList<QPair<QString, CONFIGROOT>> connectionConf;
            if (link.startsWith("vmess://") && link.contains("@"))
            {
                auto conf = vmess_new::Deserialize(link, aliasPrefix, errMessage);
                mkAllowInsecure(conf);
                connectionConf << QPair{ *aliasPrefix, conf };
            }
            else if (link.startsWith("vmess://"))
            {
                auto conf = vmess::Deserialize(link, aliasPrefix, errMessage);
                mkAllowInsecure(conf);
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
                connectionConf << ssd::Deserialize(link, newGroupName, &errMessageList);
                *errMessage = errMessageList.join(NEWLINE);
            }
            else
            {
                bool ok = false;
                const auto configs = PluginHost->TryDeserializeShareLink(link, aliasPrefix, errMessage, newGroupName, &ok);
                if (ok)
                {
                    errMessage->clear();
                    for (const auto &[_alias, _protocol, _outbound] : configs)
                    {
                        CONFIGROOT root;
                        auto outbound = GenerateOutboundEntry(_protocol, OUTBOUNDSETTING(_outbound), {});
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
            const auto &settings = outbound["settings"].toObject();
            QString sharelink = "";
            if (type == "vmess")
            {
                auto vmessServer = VMessServerObject::fromJson(settings["vnext"].toArray().first().toObject());
                auto transport = StreamSettingsObject::fromJson(outbound["streamSettings"].toObject());
                if (GlobalConfig.uiConfig.useOldShareLinkFormat)
                {
                    sharelink = vmess::Serialize(transport, vmessServer, alias);
                }
                else
                {
                    sharelink = vmess_new::Serialize(transport, vmessServer, alias);
                }
            }
            else if (type == "shadowsocks")
            {
                auto ssServer = ShadowSocksServerObject::fromJson(settings["servers"].toArray().first().toObject());
                sharelink = ss::Serialize(ssServer, alias, isSip002);
            }
            else
            {
                if (type.isEmpty())
                {
                    DEBUG(MODULE_CONNECTION, "WARNING: Empty outbound type.")
                }
                else
                {
                    bool ok = false;
                    sharelink = PluginHost->TrySerializeShareLink(type, settings, alias, groupName, &ok);
                    Q_UNUSED(ok)
                }
            }

            return sharelink;
        }

    } // namespace serialization
} // namespace Qv2ray::core::connection
