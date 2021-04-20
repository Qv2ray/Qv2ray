#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"
#include "utils/QvHelpers.hpp"

#include <QUrl>
#include <QUrlQuery>

namespace Qv2ray::core::connection
{
    namespace serialization::vmess_new
    {
        const static QStringList NetworkType{ "tcp", "http", "ws", "kcp", "quic", "grpc" };
        const static QStringList QuicSecurityTypes{ "none", "aes-128-gcm", "chacha20-poly1305" };
        const static QStringList QuicKcpHeaderTypes{ "none", "srtp", "utp", "wechat-video", "dtls", "wireguard" };
        const static QStringList FalseTypes{ "false", "False", "No", "Off", "0" };

        CONFIGROOT Deserialize(const QString &vmessStr, QString *alias, QString *errMessage)
        {
            QUrl url{ vmessStr };
            QUrlQuery query{ url };
            //
#define default CONFIGROOT()
            if (!url.isValid())
            {
                *errMessage = QObject::tr("vmess:// url is invalid");
                return default;
            }

            // If previous alias is empty, just the PS is needed, else, append a "_"
            const auto name = url.fragment(QUrl::FullyDecoded).trimmed();
            *alias = alias->isEmpty() ? name : (*alias + "_" + name);

            VMessServerObject server;
            server.users << VMessServerObject::UserObject{};

            StreamSettingsObject stream;
            QString net;
            bool tls = false;
            // Check streamSettings
            {
                for (const auto &_protocol : url.userName().split("+"))
                {
                    if (_protocol == "tls")
                        tls = true;
                    else
                        net = _protocol;
                }
                if (!NetworkType.contains(net))
                {
                    *errMessage = QObject::tr("Invalid streamSettings protocol: ") + net;
                    return default;
                }
                // L("net: " << net.toStdString());
                // L("tls: " << tls);
                stream.network = net;
                stream.security = tls ? "tls" : "";
            }
            // Host Port UUID AlterID
            {
                const auto host = url.host();
                int port = url.port();
                QString uuid;
                int aid;
                {
                    const auto pswd = url.password();
                    const auto index = pswd.lastIndexOf("-");
                    uuid = pswd.mid(0, index);
                    aid = pswd.right(pswd.length() - index - 1).toInt();
                }
                server.address = host;
                server.port = port;
                server.users.first().id = uuid;
                server.users.first().alterId = aid;
                server.users.first().security = "auto";
            }

            const static auto getQueryValue = [&query](const QString &key, const QString &defaultValue) {
                if (query.hasQueryItem(key))
                    return query.queryItemValue(key, QUrl::FullyDecoded);
                else
                    return defaultValue;
            };

            //
            // Begin transport settings parser
            {
                if (net == "tcp")
                {
                    stream.tcpSettings.header.type = getQueryValue("type", "none");
                }
                else if (net == "http")
                {
                    stream.httpSettings.host.append(getQueryValue("host", ""));
                    stream.httpSettings.path = getQueryValue("path", "/");
                }
                else if (net == "ws")
                {
                    stream.wsSettings.headers["Host"] = getQueryValue("host", "");
                    stream.wsSettings.path = getQueryValue("path", "/");
                }
                else if (net == "kcp")
                {
                    stream.kcpSettings.seed = getQueryValue("seed", "");
                    stream.kcpSettings.header.type = getQueryValue("type", "none");
                }
                else if (net == "quic")
                {
                    stream.quicSettings.security = getQueryValue("security", "none");
                    stream.quicSettings.key = getQueryValue("key", "");
                    stream.quicSettings.header.type = getQueryValue("type", "none");
                }
                else if (net == "grpc")
                {
                    stream.grpcSettings.serviceName = getQueryValue("serviceName", "");
                }
                else
                {
                    *errMessage = QObject::tr("Unknown transport method: ") + net;
                    return default;
                }
            }
#undef default
            if (tls)
            {
                stream.tlsSettings.allowInsecure = !FalseTypes.contains(getQueryValue("allowInsecure", "false"));
                stream.tlsSettings.serverName = getQueryValue("tlsServerName", "");
            }
            CONFIGROOT root;
            OUTBOUNDSETTING vConf;
            QJsonArray vnextArray;
            vnextArray.append(server.toJson());
            vConf["vnext"] = vnextArray;
            auto outbound = GenerateOutboundEntry(OUTBOUND_TAG_PROXY, "vmess", vConf, stream.toJson());
            //
            root["outbounds"] = QJsonArray{ outbound };
            return root;
        }

        const QString Serialize(const StreamSettingsObject &stream, const VMessServerObject &server, const QString &alias)
        {
            if (server.users.empty())
                return QObject::tr("(Empty Users)");
            QUrl url;
            QUrlQuery query;
            url.setFragment(alias, QUrl::StrictMode);

            if (stream.network == "tcp")
            {
                if (!stream.tcpSettings.header.type.isEmpty() && stream.tcpSettings.header.type != "none")
                    query.addQueryItem("type", stream.tcpSettings.header.type);
            }
            else if (stream.network == "http")
            {
                if (!stream.httpSettings.host.isEmpty())
                    query.addQueryItem("host", stream.httpSettings.host.first());
                query.addQueryItem("path", stream.httpSettings.path.isEmpty() ? "/" : stream.httpSettings.path);
            }
            else if (stream.network == "ws")
            {
                if (stream.wsSettings.headers.contains("Host") && !stream.wsSettings.headers["Host"].isEmpty())
                    query.addQueryItem("host", stream.wsSettings.headers["Host"]);
                if (!stream.wsSettings.path.isEmpty() && stream.wsSettings.path != "/")
                    query.addQueryItem("path", stream.wsSettings.path);
            }
            else if (stream.network == "kcp")
            {
                if (!stream.kcpSettings.seed.isEmpty() && stream.kcpSettings.seed != "")
                    query.addQueryItem("seed", stream.kcpSettings.seed);
                if (!stream.kcpSettings.header.type.isEmpty() && stream.kcpSettings.header.type != "none")
                    query.addQueryItem("type", stream.kcpSettings.header.type);
            }
            else if (stream.network == "quic")
            {
                if (!stream.quicSettings.security.isEmpty() && stream.quicSettings.security != "none")
                    query.addQueryItem("security", stream.quicSettings.security);
                if (!stream.quicSettings.key.isEmpty())
                    query.addQueryItem("key", stream.quicSettings.key);
                if (!stream.quicSettings.header.type.isEmpty() && stream.quicSettings.header.type != "none")
                    query.addQueryItem("headers", stream.quicSettings.header.type);
            }
            else if (stream.network == "grpc")
            {
                if (!stream.grpcSettings.serviceName.isEmpty())
                    query.addQueryItem("serviceName", stream.grpcSettings.serviceName);
            }
            else
            {
                return {};
            }
            bool hasTLS = stream.security == "tls" || stream.security == "xtls";
            auto protocol = stream.network;
            if (hasTLS)
                protocol += "+tls";
            if (stream.security == "tls")
            {
                if (stream.tlsSettings.allowInsecure)
                    query.addQueryItem("allowInsecure", "true");
                if (!stream.tlsSettings.serverName.isEmpty())
                    query.addQueryItem("tlsServerName", stream.tlsSettings.serverName);
            }
            else if (stream.security == "xtls")
            {
                if (stream.xtlsSettings.allowInsecure)
                    query.addQueryItem("allowInsecure", "true");
                if (!stream.xtlsSettings.serverName.isEmpty())
                    query.addQueryItem("tlsServerName", stream.xtlsSettings.serverName);
            }
            url.setPath("/");
            url.setScheme("vmess");
            url.setPassword(server.users.first().id + "-" + QSTRN(server.users.first().alterId));
            url.setHost(server.address);
            url.setPort(server.port);
            url.setUserName(protocol);
            url.setQuery(query);
            return url.toString();
        }
    } // namespace serialization::vmess_new
} // namespace Qv2ray::core::connection
