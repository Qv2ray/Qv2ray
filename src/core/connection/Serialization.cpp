#include "Serialization.hpp"

#include "Generation.hpp"
#include "common/QvHelpers.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "core/CoreUtils.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "libs/QJsonStruct/QJsonIO.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        QMultiHash<QString, CONFIGROOT> ConvertConfigFromString(const QString &link, QString *prefix, QString *errMessage, QString *newGroupName)
        {
            QMultiHash<QString, CONFIGROOT> connectionConf;
            if (link.startsWith("vmess://"))
            {
                auto conf = vmess::Deserialize(link, prefix, errMessage);
                //
                auto allowIC = GlobalConfig.advancedConfig.setAllowInsecureCiphers;
                auto allowI = GlobalConfig.advancedConfig.setAllowInsecure;
                if (allowI || allowIC)
                {
                    QJsonIO::SetValue(conf, allowI, "outbounds", 0, "streamSettings", "tlsSettings", "allowInsecure");
                    QJsonIO::SetValue(conf, allowIC, "outbounds", 0, "streamSettings", "tlsSettings", "allowInsecureCiphers");
                }
                connectionConf.insert(*prefix, conf);
            }
            else if (link.startsWith("ss://"))
            {
                auto conf = ss::Deserialize(link, prefix, errMessage);
                connectionConf.insert(*prefix, conf);
            }
            else if (link.startsWith("ssd://"))
            {
                QStringList errMessageList;
                connectionConf = ssd::Deserialize(link, newGroupName, &errMessageList);
                *errMessage = errMessageList.join(NEWLINE);
            }
            else
            {
                bool ok = false;
                auto configs = PluginHost->TryDeserializeShareLink(link, prefix, errMessage, newGroupName, &ok);
                for (const auto &key : configs.keys())
                {
                    auto vals = configs.values(key);
                    for (const auto &val : vals)
                    {
                        CONFIGROOT root;
                        auto outbound = GenerateOutboundEntry(val.first, OUTBOUNDSETTING(val.second), {});
                        root.insert("outbounds", QJsonArray{ outbound });
                        connectionConf.insert(key, root);
                    }
                }
                if (!ok)
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
                sharelink = vmess::Serialize(transport, vmessServer, alias);
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

        QString DecodeSubscriptionString(const QByteArray &arr)
        {
            // String may start with: vmess:// and ss://
            // We only process vmess:// here
            // Some subscription providers may use plain vmess:// saperated by
            // lines But others may use base64 of above.
            auto result = QString::fromUtf8(arr).trimmed();
            return result.contains("://") ? result : SafeBase64Decode(result);
        }
    } // namespace Serialization
} // namespace Qv2ray::core::connection
