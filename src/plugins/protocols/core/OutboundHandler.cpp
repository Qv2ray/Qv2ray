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

QString SerializeVLESS(const QString &name, const IOConnectionSettings &connection);
QString SerializeVMess(const QString &name, const IOConnectionSettings &connection);
QString SerializeSS(const QString &name, const IOConnectionSettings &connection);
QString SerializeTrojan(const QString &name, const IOConnectionSettings &connection);

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeVLESS(const QString &link);
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

    if (protocol == u"vless"_qs)
        return SerializeVLESS(name, outbound);

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

    if (link.startsWith(u"vless://"_qs))
        return DeserializeVLESS(link);

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

QString SerializeVLESS(const QString &name, const IOConnectionSettings &conn)
{
    const auto out = conn.protocolSettings;
    const auto stream = conn.streamSettings;
    QUrl url;
    url.setFragment(QUrl::toPercentEncoding(name));
    url.setScheme(u"vless"_qs);
    url.setHost(conn.address);
    url.setPort(conn.port.from);
    url.setUserName(out[u"id"_qs].toString());

    // -------- COMMON INFORMATION --------
    QUrlQuery query;
    const auto enc = out[u"encryption"_qs].toString(u"none"_qs);
    if (enc != u"none"_qs)
        query.addQueryItem(u"encryption"_qs, enc);

    const auto network = QJsonIO::GetValue(stream, "network").toString(u"tcp"_qs);
    if (network != u"tcp"_qs)
        query.addQueryItem(u"type"_qs, network);

    const auto security = QJsonIO::GetValue(stream, "security").toString(u"none"_qs);
    if (security != u"none"_qs)
        query.addQueryItem(u"security"_qs, security);

    // -------- TRANSPORT RELATED --------
    if (network == u"kcp"_qs)
    {
        const auto seed = QJsonIO::GetValue(stream, { "kcpSettings", "seed" }).toString();
        if (!seed.isEmpty())
            query.addQueryItem(u"seed"_qs, QUrl::toPercentEncoding(seed));

        const auto headerType = QJsonIO::GetValue(stream, { "kcpSettings", "header", "type" }).toString(u"none"_qs);
        if (headerType != u"none"_qs)
            query.addQueryItem(u"headerType"_qs, headerType);
    }
    else if (network == u"http"_qs)
    {
        const auto path = QJsonIO::GetValue(stream, { "HTTPConfig", "path" }).toString(u"/"_qs);
        query.addQueryItem(u"path"_qs, QUrl::toPercentEncoding(path));

        const auto hosts = QJsonIO::GetValue(stream, { "HTTPConfig", "host" }).toArray();
        QStringList hostList;
        for (const auto item : hosts)
        {
            const auto host = item.toString();
            if (!host.isEmpty())
                hostList << host;
        }
        query.addQueryItem(u"host"_qs, QUrl::toPercentEncoding(hostList.join(u","_qs)));
    }
    else if (network == u"ws"_qs)
    {
        const auto path = QJsonIO::GetValue(stream, { "wsSettings", "path" }).toString(u"/"_qs);
        query.addQueryItem(u"path"_qs, QUrl::toPercentEncoding(path));

        const auto host = QJsonIO::GetValue(stream, { "wsSettings", "headers", "Host" }).toString();
        query.addQueryItem(u"host"_qs, host);
    }
    else if (network == u"quic"_qs)
    {
        const auto quicSecurity = QJsonIO::GetValue(stream, { "quicSettings", "security" }).toString(u"none"_qs);
        if (quicSecurity != u"none"_qs)
        {
            query.addQueryItem(u"quicSecurity"_qs, quicSecurity);

            const auto key = QJsonIO::GetValue(stream, { "quicSettings", "key" }).toString();
            query.addQueryItem(u"key"_qs, QUrl::toPercentEncoding(key));

            const auto headerType = QJsonIO::GetValue(stream, { "quicSettings", "header", "type" }).toString(u"none"_qs);
            if (headerType != u"none"_qs)
                query.addQueryItem(u"headerType"_qs, headerType);
        }
    }
    else if (network == u"grpc"_qs)
    {
        const auto serviceName = QJsonIO::GetValue(stream, { "grpcSettings", "serviceName" }).toString(u"GunService"_qs);
        if (serviceName != u"GunService"_qs)
            query.addQueryItem(u"serviceName"_qs, QUrl::toPercentEncoding(serviceName));
    }
    // -------- TLS RELATED --------
    const auto tlsKey = security == u"xtls"_qs ? "xtlsSettings" : "tlsSettings";

    const auto sni = QJsonIO::GetValue(stream, { tlsKey, "serverName" }).toString();
    if (!sni.isEmpty())
        query.addQueryItem(u"sni"_qs, sni);

    // ALPN
    const auto alpnArray = QJsonIO::GetValue(stream, { tlsKey, u"alpn"_qs }).toArray();
    QStringList alpnList;
    for (const auto v : alpnArray)
    {
        const auto alpn = v.toString();
        if (!alpn.isEmpty())
            alpnList << alpn;
    }
    query.addQueryItem(u"alpn"_qs, QUrl::toPercentEncoding(alpnList.join(u","_qs)));

    // -------- XTLS Flow --------
    if (security == u"xtls"_qs)
    {
        const auto flow = out[u"flow"_qs].toString();
        query.addQueryItem(u"flow"_qs, flow);
    }

    // ======== END OF QUERY ========
    url.setQuery(query);
    return url.toString(QUrl::FullyEncoded);
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
    const auto plainUserInfo = server.method + ":" + server.password;
    const auto userinfo = plainUserInfo.toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    url.setUserInfo(userinfo);
    url.setScheme(u"ss"_qs);
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
    url.setUserInfo(connection.protocolSettings[u"password"_qs].toString());
    url.setFragment(name);
    return url.toString();
}

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeTrojan(const QString &link)
{
    QUrl url{ link };
    IOConnectionSettings conn;
    conn.address = url.host();
    conn.port = url.port();
    conn.protocol = u"trojan"_qs;
    conn.protocolSettings.insert(u"password"_qs, url.userInfo());
    return std::make_pair(QUrlQuery{ url.query() }.queryItemValue(u"password"_qs), conn);
}

const static QStringList NetworkType{ "tcp", "http", "ws", "kcp", "quic" };
const static QStringList QuicSecurityTypes{ "none", "aes-128-gcm", "chacha20-poly1305" };
const static QStringList QuicKcpHeaderTypes{ "none", "srtp", "utp", "wechat-video", "dtls", "wireguard" };
const static QStringList FalseTypes{ "false", "False", "No", "Off", "0" };

std::optional<std::pair<QString, IOConnectionSettings>> DeserializeVLESS(const QString &link)
{
    // parse url
    QUrl url(link);
    if (!url.isValid())
        return std::nullopt;

    // fetch host
    const auto hostRaw = url.host();
    if (hostRaw.isEmpty())
        return std::nullopt;

    const auto host = (hostRaw.startsWith('[') && hostRaw.endsWith(']')) ? hostRaw.mid(1, hostRaw.length() - 2) : hostRaw;

    // fetch port
    const auto port = url.port();
    if (port == -1)
        return std::nullopt;

    // fetch remarks
    const auto remarks = url.fragment();

    // fetch uuid
    const auto uuid = url.userInfo();
    if (uuid.isEmpty())
        return std::nullopt;

    // initialize QJsonObject with basic info
    QJsonObject outbound;
    QJsonObject stream;

    outbound[u"id"_qs] = uuid;

    // parse query
    QUrlQuery query(url.query());

    // handle type
    const auto hasType = query.hasQueryItem(u"type"_qs);
    const auto type = hasType ? query.queryItemValue(u"type"_qs) : u"tcp"_qs;
    if (type != u"tcp"_qs)
        QJsonIO::SetValue(stream, type, u"network"_qs);

    // handle encryption
    const auto hasEncryption = query.hasQueryItem(u"encryption"_qs);
    const auto encryption = hasEncryption ? query.queryItemValue(u"encryption"_qs) : u"none"_qs;
    outbound[u"encryption"_qs] = encryption;

    // type-wise settings
    if (type == u"kcp"_qs)
    {
        const auto hasSeed = query.hasQueryItem(u"seed"_qs);
        if (hasSeed)
            QJsonIO::SetValue(stream, query.queryItemValue(u"seed"_qs), { u"kcpSettings"_qs, u"seed"_qs });

        const auto hasHeaderType = query.hasQueryItem(u"headerType"_qs);
        const auto headerType = hasHeaderType ? query.queryItemValue(u"headerType"_qs) : u"none"_qs;
        if (headerType != u"none"_qs)
            QJsonIO::SetValue(stream, headerType, { u"kcpSettings"_qs, u"header"_qs, u"type"_qs });
    }
    else if (type == u"http"_qs)
    {
        const auto hasPath = query.hasQueryItem(u"path"_qs);
        const auto path = hasPath ? QUrl::fromPercentEncoding(query.queryItemValue(u"path"_qs).toUtf8()) : u"/"_qs;
        if (path != u"/"_qs)
            QJsonIO::SetValue(stream, path, { u"httpSettings"_qs, u"path"_qs });

        const auto hasHost = query.hasQueryItem(u"host"_qs);
        if (hasHost)
        {
            const auto hosts = QJsonArray::fromStringList(query.queryItemValue(u"host"_qs).split(','));
            QJsonIO::SetValue(stream, hosts, { u"httpSettings"_qs, u"host"_qs });
        }
    }
    else if (type == u"ws"_qs)
    {
        const auto hasPath = query.hasQueryItem(u"path"_qs);
        const auto path = hasPath ? QUrl::fromPercentEncoding(query.queryItemValue(u"path"_qs).toUtf8()) : u"/"_qs;
        if (path != u"/"_qs)
            QJsonIO::SetValue(stream, path, { u"wsSettings"_qs, u"path"_qs });

        const auto hasHost = query.hasQueryItem(u"host"_qs);
        if (hasHost)
        {
            QJsonIO::SetValue(stream, query.queryItemValue(u"host"_qs), { u"wsSettings"_qs, u"headers"_qs, u"Host"_qs });
        }
    }
    else if (type == u"quic"_qs)
    {
        const auto hasQuicSecurity = query.hasQueryItem(u"quicSecurity"_qs);
        if (hasQuicSecurity)
        {
            const auto quicSecurity = query.queryItemValue(u"quicSecurity"_qs);
            QJsonIO::SetValue(stream, quicSecurity, { u"quicSettings"_qs, u"security"_qs });

            if (quicSecurity != u"none"_qs)
            {
                const auto key = query.queryItemValue(u"key"_qs);
                QJsonIO::SetValue(stream, key, { u"quicSettings"_qs, u"key"_qs });
            }

            const auto hasHeaderType = query.hasQueryItem(u"headerType"_qs);
            const auto headerType = hasHeaderType ? query.queryItemValue(u"headerType"_qs) : u"none"_qs;
            if (headerType != u"none"_qs)
                QJsonIO::SetValue(stream, headerType, { u"quicSettings"_qs, u"header"_qs, u"type"_qs });
        }
    }
    else if (type == u"grpc"_qs)
    {
        const auto hasServiceName = query.hasQueryItem(u"serviceName"_qs);
        if (hasServiceName)
        {
            const auto serviceName = QUrl::fromPercentEncoding(query.queryItemValue(u"serviceName"_qs).toUtf8());
            if (serviceName != u"GunService"_qs)
            {
                QJsonIO::SetValue(stream, serviceName, { u"grpcSettings"_qs, u"serviceName"_qs });
            }
        }
    }

    // tls-wise settings
    const auto hasSecurity = query.hasQueryItem(u"security"_qs);
    const auto security = hasSecurity ? query.queryItemValue(u"security"_qs) : u"none"_qs;
    const auto tlsKey = security == u"xtls"_qs ? u"xtlsSettings"_qs : u"tlsSettings"_qs;
    if (security != u"none"_qs)
    {
        QJsonIO::SetValue(stream, security, u"security"_qs);
    }
    // sni
    const auto hasSNI = query.hasQueryItem(u"sni"_qs);
    if (hasSNI)
    {
        const auto sni = query.queryItemValue(u"sni"_qs);
        QJsonIO::SetValue(stream, sni, { tlsKey, u"serverName"_qs });
    }
    // alpn
    const auto hasALPN = query.hasQueryItem(u"alpn"_qs);
    if (hasALPN)
    {
        const auto alpnRaw = QUrl::fromPercentEncoding(query.queryItemValue(u"alpn"_qs).toUtf8());
        const auto alpnArray = QJsonArray::fromStringList(alpnRaw.split(','));
        QJsonIO::SetValue(stream, alpnArray, { tlsKey, u"alpn"_qs });
    }

    IOConnectionSettings conn;
    conn.protocol = u"vless"_qs;
    conn.address = host;
    conn.port = port;
    conn.protocolSettings = IOProtocolSettings{ outbound };
    conn.streamSettings = IOStreamSettings{ stream };
    return std::make_pair(remarks, conn);
}

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
            const QStringList hostComponents = vmessConf[u"host"_qs].toString().replace(' ', '\0').split(';');
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

    auto uri = link.mid(5);
    auto hashPos = uri.lastIndexOf('#');
    //    DEBUG("Hash sign position: " + QSTRN(hashPos));

    if (hashPos >= 0)
    {
        // Get the name/remark
        d_name = uri.mid(uri.lastIndexOf('#') + 1);
        uri.truncate(hashPos);
    }

    auto atPos = uri.indexOf('@');
    //    DEBUG("At sign position: " + QSTRN(atPos));

    if (atPos < 0)
    {
        // Old URI scheme
        QString decoded = QByteArray::fromBase64(uri.toUtf8(), QByteArray::Base64Option::OmitTrailingEquals);
        auto colonPos = decoded.indexOf(':');

        if (colonPos < 0)
        {
            return std::nullopt;
            //            *errMessage = QObject::tr("Can't find the colon separator between method and password");
        }

        server.method = decoded.left(colonPos);
        decoded.remove(0, colonPos + 1);
        atPos = decoded.lastIndexOf('@');
        //        DEBUG("At sign position: " + QSTRN(atPos));

        if (atPos < 0)
        {
            return std::nullopt;
            //            *errMessage = QObject::tr("Can't find the at separator between password and hostname");
        }

        server.password = decoded.mid(0, atPos);
        decoded.remove(0, atPos + 1);
        colonPos = decoded.lastIndexOf(':');

        if (colonPos < 0)
        {
            return std::nullopt;
            //            *errMessage = QObject::tr("Can't find the colon separator between hostname and port");
        }

        conn.address = decoded.mid(0, colonPos);
        conn.port = decoded.mid(colonPos + 1).toInt();
    }
    else
    {
        // SIP002 URI scheme
        auto x = QUrl::fromUserInput(uri);
        conn.address = x.host();
        conn.port = x.port();
        const auto userInfo = SafeBase64Decode(x.userName());
        const auto userInfoSp = userInfo.indexOf(':');
        //
        //        DEBUG("Userinfo splitter position: " + QSTRN(userInfoSp));

        if (userInfoSp < 0)
        {
            return std::nullopt;
        }

        const auto method = userInfo.mid(0, userInfoSp);
        server.method = method;
        server.password = userInfo.mid(userInfoSp + 1);
    }

    d_name = QUrl::fromPercentEncoding(d_name.toUtf8());
    conn.protocol = u"shadowsocks"_qs;
    conn.protocolSettings = IOProtocolSettings{ server.toJson() };
    return std::make_pair(d_name, conn);
}
