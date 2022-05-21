#include "OutboundHandler.hpp"

#include "QvPlugin/Utils/QJsonIO.hpp"
#include "V2RayModels.hpp"

#include <QJsonDocument>
#include <QUrl>
#include <QUrlQuery>

static QString SafeBase64Decode(QString string)
{
    QByteArray ba = string.replace(QChar('-'), QChar('+')).replace(QChar('_'), QChar('/')).toUtf8();
    return QByteArray::fromBase64(ba, QByteArray::Base64Option::OmitTrailingEquals);
}

using namespace Qv2rayPlugin;
using namespace Qv2ray::Models;

QString SerializeVMess(const QString &name, const IOConnectionSettings &connection);
QString SerializeSS(const QString &name, const IOConnectionSettings &connection);
QString SerializeTrojan(const QString &name, const IOConnectionSettings &connection);

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeVMess(const QString &link);
std::optional<std::pair<QString, IOConnectionSettings>> DeserializeSS(const QString &link);
std::optional<std::pair<QString, IOConnectionSettings>> DeserializeOldVMess(const QString &link);
std::optional<std::pair<QString, IOConnectionSettings>> DeserializeTrojan(const QString &link);

std::optional<QString> BuiltinSerializer::Serialize(const QString &name, const IOConnectionSettings &outbound) const
{
    const auto protocol = outbound.protocol;
    const auto out = outbound.protocolSettings;
    const auto stream = outbound.streamSettings;

    if (protocol == u"http"_qs || protocol == u"socks"_qs)
    {
        QUrl url;
        url.setScheme(protocol);
        url.setHost(outbound.address);
        url.setPort(outbound.port.from);
        if (out.contains(u"user"_qs))
            url.setUserName(out[u"user"_qs].toString());
        if (out.contains(u"pass"_qs))
            url.setUserName(out[u"pass"_qs].toString());
        return url.toString();
    }

    if (protocol == u"vmess"_qs)
        return SerializeVMess(name, outbound);

    if (protocol == u"shadowsocks"_qs)
        return SerializeSS(name, outbound);

    if (protocol == u"trojan"_qs)
        return SerializeTrojan(name, outbound);

    return std::nullopt;
}

std::optional<std::pair<QString, IOConnectionSettings>> BuiltinSerializer::Deserialize(const QString &link) const
{
    if (link.startsWith(u"http://"_qs) || link.startsWith(u"socks://"_qs))
    {
        const QUrl url{ link };
        IOConnectionSettings out;
        out.protocol = url.scheme();
        out.address = url.host();
        out.port = url.port();
        out.protocolSettings["user"] = url.userName();
        out.protocolSettings["pass"] = url.password();
        return std::make_pair(url.fragment(), out);
    }

    if (link.startsWith(u"ss://"_qs))
        return DeserializeSS(link);

    if (link.startsWith(u"vmess://"_qs))
        return link.contains('@') ? DeserializeVMess(link) : DeserializeOldVMess(link);

    if (link.startsWith(u"trojan://"_qs))
        return DeserializeTrojan(link);

    return std::nullopt;
}

std::optional<PluginIOBoundData> BuiltinSerializer::GetOutboundInfo(const IOConnectionSettings &) const
{
    return std::nullopt;
}

bool BuiltinSerializer::SetOutboundInfo(IOConnectionSettings &, const PluginIOBoundData &) const
{
    return true;
}

QString SerializeVMess(const QString &alias, const IOConnectionSettings &connection)
{
    Qv2ray::Models::VMessClientObject server;
    server.loadJson(connection.protocolSettings);

    Qv2ray::Models::StreamSettingsObject stream;
    stream.loadJson(connection.streamSettings);

    QUrl url;
    QUrlQuery query;
    url.setFragment(alias, QUrl::StrictMode);

    if (stream.network == u"tcp"_qs)
    {
        if (!stream.tcpSettings->header->type->isEmpty() && stream.tcpSettings->header->type != u"none"_qs)
            query.addQueryItem(u"type"_qs, stream.tcpSettings->header->type);
    }
    else if (stream.network == u"http"_qs)
    {
        if (!stream.httpSettings->host->isEmpty())
            query.addQueryItem(u"host"_qs, stream.httpSettings->host->first());
        query.addQueryItem(u"path"_qs, stream.httpSettings->path->isEmpty() ? u"/"_qs : *stream.httpSettings->path);
    }
    else if (stream.network == u"ws"_qs)
    {
        if (stream.wsSettings->headers->contains(u"Host"_qs) && !stream.wsSettings->headers->value(u"Host"_qs).isEmpty())
            query.addQueryItem(u"host"_qs, stream.wsSettings->headers->value(u"Host"_qs));
        if (!stream.wsSettings->path->isEmpty() && stream.wsSettings->path != u"/"_qs)
            query.addQueryItem(u"path"_qs, stream.wsSettings->path);
    }
    else if (stream.network == u"kcp"_qs)
    {
        if (!stream.kcpSettings->seed->isEmpty())
            query.addQueryItem(u"seed"_qs, stream.kcpSettings->seed);
        if (!stream.kcpSettings->header->type->isEmpty() && stream.kcpSettings->header->type != u"none"_qs)
            query.addQueryItem(u"type"_qs, stream.kcpSettings->header->type);
    }
    else if (stream.network == u"quic"_qs)
    {
        if (!stream.quicSettings->security->isEmpty() && stream.quicSettings->security != u"none"_qs)
            query.addQueryItem(u"security"_qs, stream.quicSettings->security);
        if (!stream.quicSettings->key->isEmpty())
            query.addQueryItem(u"key"_qs, stream.quicSettings->key);
        if (!stream.quicSettings->header->type->isEmpty() && stream.quicSettings->header->type != u"none"_qs)
            query.addQueryItem(u"headers"_qs, stream.quicSettings->header->type);
    }
    else
    {
        return {};
    }
    bool hasTLS = stream.security == u"tls"_qs;
    auto protocol = *stream.network;
    if (hasTLS)
    {
        // if (stream.tlsSettings.allowInsecure)
        //    query.addQueryItem("allowInsecure", "true");
        if (!stream.tlsSettings->serverName->isEmpty())
            query.addQueryItem(u"tlsServerName"_qs, stream.tlsSettings->serverName);
        protocol += u"+tls"_qs;
    }
    url.setPath(u"/"_qs);
    url.setScheme(u"vmess"_qs);
    url.setPassword(server.id + "-0");
    url.setHost(connection.address);
    url.setPort(connection.port.from);
    url.setUserName(protocol);
    url.setQuery(query);
    return url.toString();
}

QString SerializeSS(const QString &name, const IOConnectionSettings &connection)
{
    Qv2ray::Models::ShadowSocksClientObject server;
    server.loadJson(connection.protocolSettings);
    QUrl url;
    url.setScheme(u"ss"_qs);
    url.setUserName(server.method);
    url.setPassword(server.password);
    url.setHost(connection.address);
    url.setPort(connection.port.from);
    url.setFragment(name);
    return url.toString(QUrl::ComponentFormattingOption::FullyEncoded);
}

QString SerializeTrojan(const QString &name, const IOConnectionSettings &connection)
{
    QUrl url;
    url.setScheme(u"trojan"_qs);
    url.setHost(connection.address);
    url.setPort(connection.port.from);
    url.setUserName(connection.protocolSettings[u"password"_qs].toString().toUtf8(), QUrl::DecodedMode);

    Qv2ray::Models::StreamSettingsObject stream;
    stream.loadJson(connection.streamSettings);
    if (stream.security == u"tls"_qs && !stream.tlsSettings->serverName->isEmpty())
    {
        QUrlQuery q{ url.query() };
        q.addQueryItem(u"sni"_qs, stream.tlsSettings->serverName);
        url.setQuery(q);
    }

    url.setFragment(name);
    return url.toString(QUrl::ComponentFormattingOption::FullyEncoded);
}

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeTrojan(const QString &link)
{
    QUrl url{ link };
    IOConnectionSettings conn;
    conn.address = url.host();
    conn.port = url.port();
    conn.protocol = u"trojan"_qs;
    conn.protocolSettings.insert(u"password"_qs, QUrl::fromPercentEncoding(url.userInfo().toUtf8()));

    QUrlQuery q{ url.query() };
    if (q.hasQueryItem(u"sni"_qs))
    {
        Qv2ray::Models::StreamSettingsObject stream;
        stream.security = u"tls"_qs;
        stream.tlsSettings->serverName = q.queryItemValue(u"sni"_qs);
        conn.streamSettings = stream;
    }

    return std::make_pair(url.fragment(), conn);
}

const static QStringList NetworkType{ "tcp", "http", "ws", "kcp", "quic" };
const static QStringList QuicSecurityTypes{ "none", "aes-128-gcm", "chacha20-poly1305" };
const static QStringList QuicKcpHeaderTypes{ "none", "srtp", "utp", "wechat-video", "dtls", "wireguard" };
const static QStringList FalseTypes{ "false", "False", "No", "Off", "0" };

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeOldVMess(const QString &link)
{
    QString vmess = link.trimmed();

    if (!vmess.startsWith(u"vmess://"_qs, Qt::CaseInsensitive))
    {
        // QObject::tr("VMess string should start with 'vmess://'");
        return std::nullopt;
    }

    const auto b64Str = vmess.mid(8, vmess.length() - 8);
    if (b64Str.isEmpty())
    {
        // QObject::tr("VMess string should be a valid base64 string");
        return std::nullopt;
    }

    auto vmessConf = QJsonDocument::fromJson(SafeBase64Decode(b64Str).toUtf8()).object();

    if (vmessConf.isEmpty())
    {
        // QObject::tr("JSON should not be empty");
        return std::nullopt;
    }

    // --------------------------------------------------------------------------------------

    // vmess v1 upgrader
    if (!vmessConf.contains(u"v"_qs))
    {
        // LOG("Detected deprecated vmess v1. Trying to upgrade...");
        if (const auto network = vmessConf[u"net"_qs].toString(); network == u"ws"_qs || network == u"h2"_qs)
        {
            const QStringList hostComponents = vmessConf[u"host"_qs].toString().remove(' ').split(';');
            if (const auto nParts = hostComponents.length(); nParts == 1)
                vmessConf[u"path"_qs] = hostComponents[0], vmessConf[u"host"_qs] = u""_qs;
            else if (nParts == 2)
                vmessConf[u"path"_qs] = hostComponents[0], vmessConf[u"host"_qs] = hostComponents[1];
            else
                vmessConf[u"path"_qs] = u"/"_qs, vmessConf[u"host"_qs] = u""_qs;
        }
    }

    QString ps, add, id, net, type, host, path, tls, scy, sni;
    int port;
    //
    // __vmess_checker__func(key, values)
    //
    //   - Key     =    Key in JSON and the variable name.
    //   - Values  =    Candidate variable list, if not match, the first one is used as default.
    //
    //   - [[val.size() <= 1]] is used when only the default value exists.
    //
    //   - It can be empty, if so,           if the key is not in the JSON, or the value is empty,  report an error.
    //   - Else if it contains one thing.    if the key is not in the JSON, or the value is empty,  use that one.
    //   - Else if it contains many things,  when the key IS in the JSON but not within the THINGS, use the first in the THINGS
    //   - Else -------------------------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  use the JSON value
    //
#define __vmess_checker__func(key, vals)                                                                                                                                 \
    do                                                                                                                                                                   \
    {                                                                                                                                                                    \
        const auto _key = u"" #key##_qs;                                                                                                                                 \
        auto val = QStringList() vals;                                                                                                                                   \
        if (vmessConf.contains(_key) && !vmessConf[_key].toVariant().toString().trimmed().isEmpty() &&                                                                   \
            (val.size() <= 1 || val.contains(vmessConf[_key].toVariant().toString())))                                                                                   \
        {                                                                                                                                                                \
            key = vmessConf[u"" #key##_qs].toVariant().toString();                                                                                                       \
        }                                                                                                                                                                \
        else if (!val.isEmpty())                                                                                                                                         \
        {                                                                                                                                                                \
            key = val.first();                                                                                                                                           \
        }                                                                                                                                                                \
        else                                                                                                                                                             \
        {                                                                                                                                                                \
            return std::nullopt;                                                                                                                                         \
        }                                                                                                                                                                \
    } while (false)

    // Strict check of VMess protocol, to check if the specified value
    // is in the correct range.
    //
    // Get Alias (AKA ps) from address and port.
    {
        __vmess_checker__func(ps, << vmessConf[u"add"_qs].toString() + u":"_qs + vmessConf[u"port"_qs].toVariant().toString()); //
        __vmess_checker__func(add, );
        __vmess_checker__func(id, );
        __vmess_checker__func(scy, << u"aes-128-gcm"_qs       //
                                   << u"chacha20-poly1305"_qs //
                                   << u"auto"_qs              //
                                   << u"none"_qs              //
                                   << u"zero"_qs);            //

        __vmess_checker__func(type, << u"none"_qs           //
                                    << u"http"_qs           //
                                    << u"srtp"_qs           //
                                    << u"utp"_qs            //
                                    << u"wechat-video"_qs); //

        __vmess_checker__func(net, << u"tcp"_qs    //
                                   << u"http"_qs   //
                                   << u"h2"_qs     //
                                   << u"ws"_qs     //
                                   << u"kcp"_qs    //
                                   << u"quic"_qs   //
                                   << u"grpc"_qs); //

        __vmess_checker__func(tls, << u"none"_qs  //
                                   << u"tls"_qs); //

        path = vmessConf.contains("path") ? vmessConf["path"].toVariant().toString() : (net == "quic" ? "" : "/");
        host = vmessConf.contains("host") ? vmessConf["host"].toVariant().toString() : (net == "quic" ? "none" : "");
    }

    // Respect connection type rather than obfs type
    if (QStringList{ "srtp", "utp", "wechat-video" }.contains(type))
    {
        if (net != "quic" && net != "kcp")
        {
            // ("Reset obfs settings from " + type + " to none");
            type = "none";
        }
    }

    port = vmessConf[u"port"_qs].toVariant().toInt();

    //
    // Apply the settings.

    IOConnectionSettings conn;
    VMessClientObject client;
    client.id = id;
    client.security = scy;
    conn.port = port;
    conn.address = add;
    conn.protocol = u"vmess"_qs;
    conn.protocolSettings = IOProtocolSettings{ client.toJson() };

    //
    //
    // Stream Settings
    StreamSettingsObject streaming;

    if (net == "tcp")
    {
        streaming.tcpSettings->header->type = type;
    }
    else if (net == "http" || net == "h2")
    {
        // Fill hosts for HTTP
        for (const auto &_host : host.split(','))
            if (!_host.isEmpty())
                streaming.httpSettings->host << _host.trimmed();

        streaming.httpSettings->path = path;
    }
    else if (net == "ws")
    {
        if (!host.isEmpty())
            (*streaming.wsSettings->headers)["Host"] = host;
        streaming.wsSettings->path = path;
    }
    else if (net == "kcp")
    {
        streaming.kcpSettings->header->type = type;
    }
    else if (net == "quic")
    {
        streaming.quicSettings->security = host;
        streaming.quicSettings->header->type = type;
        streaming.quicSettings->key = path;
    }
    else if (net == "grpc")
    {
        streaming.grpcSettings->serviceName = path;
    }

    streaming.security = tls;
    if (tls == "tls")
    {
        if (sni.isEmpty() && !host.isEmpty())
            sni = host;
        streaming.tlsSettings->serverName = sni;
    }

    // Network type
    // NOTE(DuckSoft): Damn vmess:// just don't write 'http' properly
    if (net == "h2")
        net = "http";
    streaming.network = net;
    conn.streamSettings = IOStreamSettings{ streaming.toJson() };
    return std::make_pair(ps, conn);
}

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeVMess(const QString &link)
{
    IOConnectionSettings conn;
    conn.protocol = u"vmess"_qs;
    QUrl url{ link };
    QUrlQuery query{ url };
    if (!url.isValid())
        return std::nullopt;

    // If previous alias is empty, just the PS is needed, else, append a "_"
    const auto name = url.fragment(QUrl::FullyDecoded).trimmed();

    VMessClientObject client;

    StreamSettingsObject stream;
    QString net;
    bool tls = false;
    // Check streamSettings
    {
        for (const auto &_protocol : url.userName().split('+'))
        {
            if (_protocol == u"tls"_qs)
                tls = true;
            else
                net = _protocol;
        }
        if (!NetworkType.contains(net))
        {
            return {};
        }
        // L("net: " << net.toStdString());
        // L("tls: " << tls);
        stream.network = net;
        stream.security = tls ? u"tls"_qs : u""_qs;
    }
    // Host Port UUID AlterID
    {
        const auto host = url.host();
        int port = url.port();
        QString uuid;
        {
            const auto pswd = url.password();
            const auto index = pswd.lastIndexOf('-');
            uuid = pswd.mid(0, index);
        }

        conn.address = host;
        conn.port = port;
        client.id = uuid;
        client.security = u"auto"_qs;
    }

    const static auto getQueryValue = [&query](const QString &key, const QString &defaultValue)
    {
        if (query.hasQueryItem(key))
            return query.queryItemValue(key, QUrl::FullyDecoded);
        else
            return defaultValue;
    };

    //
    // Begin transport settings parser
    {
        if (net == u"tcp"_qs)
        {
            stream.tcpSettings->header->type = getQueryValue(u"type"_qs, u"none"_qs);
        }
        else if (net == u"http"_qs)
        {
            stream.httpSettings->host->append(getQueryValue(u"host"_qs, u""_qs));
            stream.httpSettings->path = getQueryValue(u"path"_qs, u"/"_qs);
        }
        else if (net == u"ws"_qs)
        {
            stream.wsSettings->headers->insert(u"Host"_qs, getQueryValue(u"host"_qs, u""_qs));
            stream.wsSettings->path = getQueryValue(u"path"_qs, u"/"_qs);
        }
        else if (net == u"kcp"_qs)
        {
            stream.kcpSettings->seed = getQueryValue(u"seed"_qs, u""_qs);
            stream.kcpSettings->header->type = getQueryValue(u"type"_qs, u"none"_qs);
        }
        else if (net == u"quic"_qs)
        {
            stream.quicSettings->security = getQueryValue(u"security"_qs, u"none"_qs);
            stream.quicSettings->key = getQueryValue(u"key"_qs, u""_qs);
            stream.quicSettings->header->type = getQueryValue(u"type"_qs, u"none"_qs);
        }
    }

    if (tls)
    {
        // stream.tlsSettings.allowInsecure = !FalseTypes.contains(getQueryValue("allowInsecure", "false"));
        stream.tlsSettings->serverName = getQueryValue(u"tlsServerName"_qs, u""_qs);
    }

    conn.protocolSettings = IOProtocolSettings{ client.toJson() };
    conn.streamSettings = IOStreamSettings{ stream.toJson() };
    return std::make_pair(name, conn);
}

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeSS(const QString &link)
{
    IOConnectionSettings conn;
    ShadowSocksClientObject server;
    QString d_name;

    const QUrl ssUrl{ link };

    const auto userinfo = ssUrl.userInfo();

    conn.address = ssUrl.host();
    conn.port = ssUrl.port();
    conn.protocol = u"shadowsocks"_qs;

    // why fromPercentEncoding
    d_name = QUrl::fromPercentEncoding(ssUrl.fragment(QUrl::FullyDecoded).trimmed().toUtf8());

    // We now don't support the old old old format: base64(url)
    if (userinfo.contains(u':'))
    {
        // case 1: neat format
        server.method = ssUrl.userName();
        server.password = ssUrl.password();
    }
    else
    {
        // case 2: base64(userinfo) old format
        const auto realUserinfo = QByteArray::fromBase64(userinfo.toUtf8());
        const auto list = realUserinfo.split(':');

        if (Q_UNLIKELY(list.size() != 2))
            return std::nullopt;

        server.method = list[0];
        server.password = list[1];
    }

    conn.protocolSettings = IOProtocolSettings{ server.toJson() };
    return std::make_pair(d_name, conn);
}
