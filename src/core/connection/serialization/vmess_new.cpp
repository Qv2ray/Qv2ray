#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"

#include <QUrl>
#include <QUrlQuery>

namespace Qv2ray::core::connection
{
    namespace serialization::vmess_new
    {
        const static QStringList NetworkType{ "tcp", "http", "ws", "kcp", "quic" };
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
            const auto name = url.fragment(QUrl::FullyDecoded);

            // If previous alias is empty, just the PS is needed, else, append a "_"
            *alias = alias->trimmed().isEmpty() ? name : *alias + "_" + name;

            VMessServerObject server;
            server.users << VMessServerObject::UserObject{};
            StreamSettingsObject stream;
            QString net;
            bool tls;
            // Check streamSettings
            {
                for (const auto &_protocol : url.userName().split("+"))
                {
                    tls = tls || _protocol == "tls";
                    net = _protocol == "tls" ? net : _protocol;
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
                // L("uuid: " << uuid.toString().toStdString());
                // L("aid: " << aid);
                // L("host: " << host.toStdString());
                // L("port: " << port);
            }
            auto getQueryValue = [&](const QString &key) {
                return query.queryItemValue(key, QUrl::FullyDecoded);
            };
            //
            // Begin transport settings parser
            {
                if (net == "tcp")
                {
                    stream.tcpSettings.header.type = getQueryValue("type");
                }
                else if (net == "http")
                {
                    stream.httpSettings.host.append(getQueryValue("host"));
                    stream.httpSettings.path = getQueryValue("path");
                }
                else if (net == "ws")
                {
                    stream.wsSettings.headers["Host"] = getQueryValue("host");
                    stream.wsSettings.path = getQueryValue("path");
                }
                else if (net == "kcp")
                {
                    stream.kcpSettings.seed = getQueryValue("seed");
                    stream.kcpSettings.header.type = getQueryValue("type");
                }
                else if (net == "quic")
                {
                    stream.quicSettings.security = getQueryValue("security");
                    stream.quicSettings.key = getQueryValue("key");
                    stream.quicSettings.header.type = getQueryValue("headers");
                }
                else
                {
                    *errMessage = QObject::tr("Unknown state.");
                    return default;
                }
            }
#undef default
            if (tls)
            {
                stream.tlsSettings.allowInsecure = !FalseTypes.contains(getQueryValue("allowInsecure"));
                stream.tlsSettings.serverName = getQueryValue("tlsServerName");
            }
            CONFIGROOT root;
            OUTBOUNDSETTING vConf;
            QJsonArray vnextArray;
            vnextArray.append(server.toJson());
            vConf["vnext"] = vnextArray;
            auto outbound = GenerateOutboundEntry("vmess", vConf, stream.toJson(), {}, "0.0.0.0", OUTBOUND_TAG_PROXY);
            //
            root["outbounds"] = QJsonArray{ outbound };
            return root;
        }

        const QString Serialize(const StreamSettingsObject &stream, const VMessServerObject &server, const QString &alias)
        {
            QUrl url;
            QUrlQuery query;
            url.setFragment(alias, QUrl::StrictMode);

            if (stream.network == "tcp")
            {
                query.addQueryItem("type", stream.tcpSettings.header.type);
            }
            else if (stream.network == "http")
            {
                if (!stream.httpSettings.host.isEmpty())
                    query.addQueryItem("host", stream.httpSettings.host.first());
                query.addQueryItem("path", stream.httpSettings.path);
            }
            else if (stream.network == "ws")
            {
                query.addQueryItem("host", stream.wsSettings.headers["Host"]);
                query.addQueryItem("path", stream.wsSettings.path);
            }
            else if (stream.network == "kcp")
            {
                query.addQueryItem("seed", stream.kcpSettings.seed);
                query.addQueryItem("type", stream.kcpSettings.header.type);
            }
            else if (stream.network == "quic")
            {
                query.addQueryItem("security", stream.quicSettings.security);
                query.addQueryItem("key", stream.quicSettings.key);
                query.addQueryItem("headers", stream.quicSettings.header.type);
            }
            else
            {
                return {};
            }
            bool hasTLS = stream.security == "tls";
            auto protocol = stream.network;
            if (hasTLS)
            {
                query.addQueryItem("allowInsecure", stream.tlsSettings.allowInsecure ? "true" : "false");
                query.addQueryItem("tlsServerName", stream.tlsSettings.serverName);
                protocol += "+tls";
            }
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
