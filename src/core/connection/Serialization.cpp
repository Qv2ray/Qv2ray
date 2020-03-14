#include "Serialization.hpp"

#include "Generation.hpp"
#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/handler/ConfigHandler.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        const QString ConvertConfigToString(const QString &alias, const CONFIGROOT &server, bool isSip002);
        CONFIGROOT ConvertConfigFromString(const QString &link, QString *alias, QString *errMessage)
        {
            CONFIGROOT config;

            if (link.startsWith("vmess://"))
            {
                config = ConvertConfigFromVMessString(link, alias, errMessage);
            }
            else if (link.startsWith("ss://"))
            {
                config = ConvertConfigFromSSString(link, alias, errMessage);
            } else if(link.startsWith("ssr://")){
                config = ConvertConfigFromSSRString(link, alias, errMessage);
            }
            else
            {
                *errMessage = QObject::tr("Unsupported share link format.");
            }

            return config;
        }

        const QString ConvertConfigToString(const ConnectionId &id, bool isSip002)
        {
            auto alias = GetDisplayName(id);
            if (IsComplexConfig(id))
            {
                DEBUG(MODULE_CONNECTION, "Ignored an complex config: " + alias)
                return QV2RAY_SERIALIZATION_COMPLEX_CONFIG_PLACEHOLDER;
            }
            auto server = ConnectionManager->GetConnectionRoot(id);
            return ConvertConfigToString(alias, server, isSip002);
        }

        const QString ConvertConfigToString(const QString &alias, const CONFIGROOT &server, bool isSip002)
        {
            OUTBOUND outbound = OUTBOUND(server["outbounds"].toArray().first().toObject());
            auto type = outbound["protocol"].toString();
            QString sharelink = "";

            if (type == "vmess")
            {
                auto vmessServer =
                    StructFromJsonString<VMessServerObject>(JsonToString(outbound["settings"].toObject()["vnext"].toArray().first().toObject()));
                auto transport = StructFromJsonString<StreamSettingsObject>(JsonToString(outbound["streamSettings"].toObject()));
                sharelink = vmess::ConvertConfigToVMessString(transport, vmessServer, alias);
            }
            else if (type == "shadowsocks")
            {
                auto ssServer = StructFromJsonString<ShadowSocksServerObject>(
                    JsonToString(outbound["settings"].toObject()["servers"].toArray().first().toObject()));
                sharelink = ss::ConvertConfigToSSString(ssServer, alias, isSip002);
            }
            else if (type == "shadowsocksr")
            {
                auto ssrServer = StructFromJsonString<ShadowSocksRServerObject>(JsonToString(outbound["settings"].toObject()["servers"].toArray().first().toObject()));
                sharelink = ss::ConvertConfigToSSRString(ssrServer);
            }
            else
            {
                if (!type.isEmpty())
                {
                    // DEBUG(MODULE_CONNECTION, "WARNING: Unsupported outbound type: " + type)
                }
                else
                {
                    DEBUG(MODULE_CONNECTION, "WARNING: Empty outbound type.")
                }
            }

            return sharelink;
        }

        QString DecodeSubscriptionString(QByteArray arr)
        {
            // String may start with: vmess:// and ss://
            // We only process vmess:// here
            // Some subscription providers may use plain vmess:// saperated by
            // lines But others may use base64 of above.
            auto result = QString::fromUtf8(arr).trimmed();
            return result.startsWith("vmess://") ? result : Base64Decode(result);
        }
    } // namespace Serialization
} // namespace Qv2ray::core::connection
