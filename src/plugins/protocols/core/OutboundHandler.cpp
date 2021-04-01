#include "OutboundHandler.hpp"

#include "3rdparty/QJsonStruct/QJsonIO.hpp"

#include <QUrl>
#include <QUrlQuery>

using namespace Qv2rayPlugin;

const OutboundInfoObject BuiltinSerializer::GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const
{
    OutboundInfoObject obj;
    obj[INFO_PROTOCOL] = protocol;
    if (protocol == "http")
    {
        HttpServerObject http;
        http.loadJson(outbound["servers"].toArray().first());
        obj[INFO_SERVER] = http.address;
        obj[INFO_PORT] = http.port;
    }
    else if (protocol == "socks")
    {
        SocksServerObject socks;
        socks.loadJson(outbound["servers"].toArray().first());
        obj[INFO_SERVER] = socks.address;
        obj[INFO_PORT] = socks.port;
    }
    else if (protocol == "vmess")
    {
        VMessServerObject vmess;
        vmess.loadJson(outbound["vnext"].toArray().first());
        obj[INFO_SERVER] = vmess.address;
        obj[INFO_PORT] = vmess.port;
    }
    else if (protocol == "vless")
    {
        VLESSServerObject vless;
        vless.loadJson(outbound["vnext"].toArray().first());
        obj[INFO_SERVER] = vless.address;
        obj[INFO_PORT] = vless.port;
    }
    else if (protocol == "shadowsocks")
    {
        ShadowSocksServerObject ss;
        ss.loadJson(outbound["servers"].toArray().first());
        obj[INFO_SERVER] = ss.address;
        obj[INFO_PORT] = ss.port;
    }
    return obj;
}

void BuiltinSerializer::SetOutboundInfo(const QString &protocol, const OutboundInfoObject &info, QJsonObject &outbound) const
{
    if ((QStringList{ "http", "socks", "shadowsocks" }).contains(protocol))
    {
        QJsonIO::SetValue(outbound, info[INFO_SERVER].toString(), "servers", 0, "address");
        QJsonIO::SetValue(outbound, info[INFO_PORT].toInt(), "servers", 0, "port");
    }
    else if ((QStringList{ "vless", "vmess" }).contains(protocol))
    {
        QJsonIO::SetValue(outbound, info[INFO_SERVER].toString(), "vnext", 0, "address");
        QJsonIO::SetValue(outbound, info[INFO_PORT].toInt(), "vnext", 0, "port");
    }
}

const QString BuiltinSerializer::SerializeOutbound(const QString &protocol, const QString &alias, const QString &, const QJsonObject &obj,
                                                   const QJsonObject &objStream) const
{
    if (protocol == "http" || protocol == "socks")
    {
        QUrl url;
        url.setScheme(protocol);
        url.setHost(QJsonIO::GetValue(obj, { "servers", 0, "address" }).toString());
        url.setPort(QJsonIO::GetValue(obj, { "servers", 0, "port" }).toInt());
        if (QJsonIO::GetValue(obj, { "servers", 0 }).toObject().contains("users"))
        {
            url.setUserName(QJsonIO::GetValue(obj, { "servers", 0, "users", 0, "user" }).toString());
            url.setPassword(QJsonIO::GetValue(obj, { "servers", 0, "users", 0, "pass" }).toString());
        }
        return url.toString();
    }
    if (protocol == "vless")
    {
        QUrl url;
        url.setFragment(QUrl::toPercentEncoding(alias));
        url.setScheme(protocol);
        url.setHost(QJsonIO::GetValue(obj, { "vnext", 0, "address" }).toString());
        url.setPort(QJsonIO::GetValue(obj, { "vnext", 0, "port" }).toInt());
        url.setUserName(QJsonIO::GetValue(obj, { "vnext", 0, "users", 0, "id" }).toString());

        // -------- COMMON INFORMATION --------
        QUrlQuery query;
        const auto encryption = QJsonIO::GetValue(obj, { "vnext", 0, "users", 0, "encryption" }).toString("none");
        if (encryption != "none")
            query.addQueryItem("encryption", encryption);

        const auto network = QJsonIO::GetValue(objStream, "network").toString("tcp");
        if (network != "tcp")
            query.addQueryItem("type", network);

        const auto security = QJsonIO::GetValue(objStream, "security").toString("none");
        if (security != "none")
            query.addQueryItem("security", security);

        // -------- TRANSPORT RELATED --------
        if (network == "kcp")
        {
            const auto seed = QJsonIO::GetValue(objStream, { "kcpSettings", "seed" }).toString();
            if (!seed.isEmpty())
                query.addQueryItem("seed", QUrl::toPercentEncoding(seed));

            const auto headerType = QJsonIO::GetValue(objStream, { "kcpSettings", "header", "type" }).toString("none");
            if (headerType != "none")
                query.addQueryItem("headerType", headerType);
        }
        else if (network == "http")
        {
            const auto path = QJsonIO::GetValue(objStream, { "httpSettings", "path" }).toString("/");
            query.addQueryItem("path", QUrl::toPercentEncoding(path));

            const auto hosts = QJsonIO::GetValue(objStream, { "httpSettings", "host" }).toArray();
            QStringList hostList;
            for (const auto item : hosts)
            {
                const auto host = item.toString();
                if (!host.isEmpty())
                    hostList << host;
            }
            query.addQueryItem("host", QUrl::toPercentEncoding(hostList.join(",")));
        }
        else if (network == "ws")
        {
            const auto path = QJsonIO::GetValue(objStream, { "wsSettings", "path" }).toString("/");
            query.addQueryItem("path", QUrl::toPercentEncoding(path));

            const auto host = QJsonIO::GetValue(objStream, { "wsSettings", "headers", "Host" }).toString();
            query.addQueryItem("host", host);
        }
        else if (network == "quic")
        {
            const auto quicSecurity = QJsonIO::GetValue(objStream, { "quicSettings", "security" }).toString("none");
            if (quicSecurity != "none")
            {
                query.addQueryItem("quicSecurity", quicSecurity);

                const auto key = QJsonIO::GetValue(objStream, { "quicSettings", "key" }).toString();
                query.addQueryItem("key", QUrl::toPercentEncoding(key));

                const auto headerType = QJsonIO::GetValue(objStream, { "quicSettings", "header", "type" }).toString("none");
                if (headerType != "none")
                    query.addQueryItem("headerType", headerType);
            }
        }

        // -------- TLS RELATED --------
        const auto tlsKey = security == "xtls" ? "xtlsSettings" : "tlsSettings";

        const auto sni = QJsonIO::GetValue(objStream, { tlsKey, "serverName" }).toString();
        if (!sni.isEmpty())
            query.addQueryItem("sni", sni);
        // TODO: ALPN Support

        // -------- XTLS Flow --------
        if (security == "xtls")
        {
            const auto flow = QJsonIO::GetValue(obj, "vnext", 0, "users", 0, "flow").toString();
            query.addQueryItem("flow", flow);
        }

        // ======== END OF QUERY ========
        url.setQuery(query);
        return url.toString(QUrl::FullyEncoded);
    }
    return "(Unsupported)";
}

const QPair<QString, QJsonObject> BuiltinSerializer::DeserializeOutbound(const QString &link, QString *alias, QString *errorMessage) const
{
    Q_UNUSED(alias);
    Q_UNUSED(errorMessage);
    if (link.startsWith("http://") || link.startsWith("socks://"))
    {
        const QUrl url = link;
        QJsonObject root;
        QJsonIO::SetValue(root, url.host(), "servers", 0, "address");
        QJsonIO::SetValue(root, url.port(), "servers", 0, "port");
        if (url.userName().isEmpty() && url.password().isEmpty())
        {
            QJsonIO::SetValue(root, url.userName(), "servers", 0, "users", 0, "user");
            QJsonIO::SetValue(root, url.password(), "servers", 0, "users", 0, "pass");
        }
        return { url.scheme(), root };
    }
    return {};
}

const QList<QString> BuiltinSerializer::SupportedLinkPrefixes() const
{
    return { "http", "socks" };
}
