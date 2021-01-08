#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "VLESSImporter"

namespace Qv2ray::core::connection
{
    namespace serialization::vless
    {
        CONFIGROOT Deserialize(const QString &str, QString *alias, QString *errMessage)
        {
            // must start with vless://
            if (!str.startsWith("vless://"))
            {
                *errMessage = QObject::tr("VLESS link should start with vless://");
                return CONFIGROOT();
            }

            // parse url
            QUrl url(str);
            if (!url.isValid())
            {
                *errMessage = QObject::tr("link parse failed: %1").arg(url.errorString());
                return CONFIGROOT();
            }

            // fetch host
            const auto hostRaw = url.host();
            if (hostRaw.isEmpty())
            {
                *errMessage = QObject::tr("empty host");
                return CONFIGROOT();
            }
            const auto host = (hostRaw.startsWith('[') && hostRaw.endsWith(']')) ? hostRaw.mid(1, hostRaw.length() - 2) : hostRaw;

            // fetch port
            const auto port = url.port();
            if (port == -1)
            {
                *errMessage = QObject::tr("missing port");
                return CONFIGROOT();
            }

            // fetch remarks
            const auto remarks = url.fragment();
            if (!remarks.isEmpty())
            {
                *alias = remarks;
            }

            // fetch uuid
            const auto uuid = url.userInfo();
            if (uuid.isEmpty())
            {
                *errMessage = QObject::tr("missing uuid");
                return CONFIGROOT();
            }

            // initialize QJsonObject with basic info
            QJsonObject outbound;
            QJsonObject stream;

            QJsonIO::SetValue(outbound, "vless", "protocol");
            QJsonIO::SetValue(outbound, host, { "settings", "vnext", 0, "address" });
            QJsonIO::SetValue(outbound, port, { "settings", "vnext", 0, "port" });
            QJsonIO::SetValue(outbound, uuid, { "settings", "vnext", 0, "users", 0, "id" });

            // parse query
            QUrlQuery query(url.query());

            // handle network
            const auto hasNetwork = query.hasQueryItem("network");
            const auto network = hasNetwork ? query.queryItemValue("network") : "tcp";
            if (network != "tcp")
                QJsonIO::SetValue(stream, network, "network");

            // handle encryption
            const auto hasEncryption = query.hasQueryItem("encryption");
            const auto encryption = hasEncryption ? query.queryItemValue("encryption") : "none";
            QJsonIO::SetValue(outbound, encryption, { "settings", "vnext", 0, "users", 0, "encryption" });

            // network-wise settings
            if (network == "kcp")
            {
                const auto hasSeed = query.hasQueryItem("seed");
                if (hasSeed)
                    QJsonIO::SetValue(stream, query.queryItemValue("seed"), { "kcpSettings", "seed" });

                const auto hasHeaderType = query.hasQueryItem("headerType");
                const auto headerType = hasHeaderType ? query.queryItemValue("headerType") : "none";
                if (headerType != "none")
                    QJsonIO::SetValue(stream, headerType, { "kcpSettings", "header", "type" });
            }
            else if (network == "http")
            {
                const auto hasPath = query.hasQueryItem("path");
                const auto path = hasPath ? query.queryItemValue("path") : "/";
                if (path != "/")
                    QJsonIO::SetValue(stream, path, { "httpSettings", "path" });

                const auto hasHost = query.hasQueryItem("host");
                if (hasHost)
                {
                    const auto hosts = QJsonArray::fromStringList(query.queryItemValue("host").split(","));
                    QJsonIO::SetValue(stream, hosts, { "httpSettings", "host" });
                }
            }
            else if (network == "ws")
            {
                const auto hasPath = query.hasQueryItem("path");
                const auto path = hasPath ? query.queryItemValue("path") : "/";
                if (path != "/")
                    QJsonIO::SetValue(stream, path, { "wsSettings", "path" });

                const auto hasHost = query.hasQueryItem("path");
                if (hasHost)
                {
                    QJsonIO::SetValue(stream, query.queryItemValue("host"), { "wsSettings", "headers", "Host" });
                }
            }
            else if (network == "quic")
            {
                const auto hasQuicSecurity = query.hasQueryItem("quicSecurity");
                if (hasQuicSecurity)
                {
                    const auto quicSecurity = query.queryItemValue("quicSecurity");
                    QJsonIO::SetValue(stream, quicSecurity, { "quicSettings", "security" });

                    if (quicSecurity != "none")
                    {
                        const auto key = query.queryItemValue("key");
                        QJsonIO::SetValue(stream, key, { "quicSettings", "key" });
                    }

                    const auto hasHeaderType = query.hasQueryItem("headerType");
                    const auto headerType = hasHeaderType ? query.queryItemValue("headerType") : "none";
                    if (headerType != "none")
                        QJsonIO::SetValue(stream, headerType, { "quicSettings", "header", "type" });
                }
            }

            // tls-wise settings
            const auto hasSecurity = query.hasQueryItem("security");
            const auto security = hasSecurity ? query.queryItemValue("security") : "none";
            const auto tlsKey = security == "xtls" ? "xtlsSettings" : "tlsSettings";
            if (security != "none")
            {
                QJsonIO::SetValue(stream, security, "security");
            }
            // sni
            const auto hasSNI = query.hasQueryItem("sni");
            if (hasSNI)
            {
                const auto sni = query.queryItemValue("sni");
                QJsonIO::SetValue(stream, sni, { tlsKey, "serverName" });
            }
            // xtls-specific
            if (security == "xtls")
            {
                const auto flow = query.queryItemValue("flow");
                QJsonIO::SetValue(outbound, flow, { "vnext", 0, "users", 0, "flow" });
            }

            // assembling config
            CONFIGROOT root;
            root["outbounds"] = QJsonArray{ outbound };
            root["streamSettings"] = stream;

            // return
            return root;
        }
    } // namespace serialization::vless
} // namespace Qv2ray::core::connection
