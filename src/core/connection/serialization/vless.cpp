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

            // handle type
            const auto hasType = query.hasQueryItem("type");
            const auto type = hasType ? query.queryItemValue("type") : "tcp";
            if (type != "tcp")
                QJsonIO::SetValue(stream, type, "network");

            // handle encryption
            const auto hasEncryption = query.hasQueryItem("encryption");
            const auto encryption = hasEncryption ? query.queryItemValue("encryption") : "none";
            QJsonIO::SetValue(outbound, encryption, { "settings", "vnext", 0, "users", 0, "encryption" });

            // type-wise settings
            if (type == "kcp")
            {
                const auto hasSeed = query.hasQueryItem("seed");
                if (hasSeed)
                    QJsonIO::SetValue(stream, query.queryItemValue("seed"), { "kcpSettings", "seed" });

                const auto hasHeaderType = query.hasQueryItem("headerType");
                const auto headerType = hasHeaderType ? query.queryItemValue("headerType") : "none";
                if (headerType != "none")
                    QJsonIO::SetValue(stream, headerType, { "kcpSettings", "header", "type" });
            }
            else if (type == "http")
            {
                const auto hasPath = query.hasQueryItem("path");
                const auto path = hasPath ? QUrl::fromPercentEncoding(query.queryItemValue("path").toUtf8()) : "/";
                if (path != "/")
                    QJsonIO::SetValue(stream, path, { "httpSettings", "path" });

                const auto hasHost = query.hasQueryItem("host");
                if (hasHost)
                {
                    const auto hosts = QJsonArray::fromStringList(query.queryItemValue("host").split(","));
                    QJsonIO::SetValue(stream, hosts, { "httpSettings", "host" });
                }
            }
            else if (type == "ws")
            {
                const auto hasPath = query.hasQueryItem("path");
                const auto path = hasPath ? QUrl::fromPercentEncoding(query.queryItemValue("path").toUtf8()) : "/";
                if (path != "/")
                    QJsonIO::SetValue(stream, path, { "wsSettings", "path" });

                const auto hasHost = query.hasQueryItem("host");
                if (hasHost)
                {
                    QJsonIO::SetValue(stream, query.queryItemValue("host"), { "wsSettings", "headers", "Host" });
                }
            }
            else if (type == "quic")
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
            else if (type == "grpc")
            {
                const auto hasServiceName = query.hasQueryItem("serviceName");
                if (hasServiceName)
                {
                    const auto serviceName = QUrl::fromPercentEncoding(query.queryItemValue("serviceName").toUtf8());
                    QJsonIO::SetValue(stream, serviceName, { "grpcSettings", "serviceName" });
                }

                const auto hasMode = query.hasQueryItem("mode");
                if (hasMode)
                {
                    const auto multiMode = QUrl::fromPercentEncoding(query.queryItemValue("mode").toUtf8()) == "multi";
                    QJsonIO::SetValue(stream, multiMode, { "grpcSettings", "multiMode" });
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
            // alpn
            const auto hasALPN = query.hasQueryItem("alpn");
            if (hasALPN)
            {
                const auto alpnRaw = QUrl::fromPercentEncoding(query.queryItemValue("alpn").toUtf8());
                const auto alpnArray = QJsonArray::fromStringList(alpnRaw.split(","));
                QJsonIO::SetValue(stream, alpnArray, { tlsKey, "alpn" });
            }
            // xtls-specific
            if (security == "xtls")
            {
                const auto flow = query.queryItemValue("flow");
                QJsonIO::SetValue(outbound, flow, { "settings", "vnext", 0, "users", 0, "flow" });
            }

            // assembling config
            CONFIGROOT root;
            outbound["streamSettings"] = stream;
            root["outbounds"] = QJsonArray{ outbound };

            // return
            return root;
        }
    } // namespace serialization::vless
} // namespace Qv2ray::core::connection
