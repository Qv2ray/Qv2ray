#include "OutboundHandler.hpp"

#include "QJsonIO.hpp"

#include <QUrl>
#include <QUrlQuery>

using namespace Qv2rayPlugin;

std::optional<PluginIOBoundData> BuiltinSerializer::GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const
{
    PluginIOBoundData obj;
    obj[IOBOUND::PROTOCOL] = protocol;
    if (protocol == "http")
    {
        HttpServerObject http;
        http.loadJson(outbound["servers"].toArray().first());
        obj[IOBOUND::ADDRESS] = *http.address;
        obj[IOBOUND::PORT] = *http.port;
        return obj;
    }
    else if (protocol == "socks")
    {
        SocksServerObject socks;
        socks.loadJson(outbound["servers"].toArray().first());
        obj[IOBOUND::ADDRESS] = *socks.address;
        obj[IOBOUND::PORT] = *socks.port;
        return obj;
    }
    else if (protocol == "vmess")
    {
        VMessServerObject vmess;
        vmess.loadJson(outbound["vnext"].toArray().first());
        obj[IOBOUND::ADDRESS] = *vmess.address;
        obj[IOBOUND::PORT] = *vmess.port;
        return obj;
    }
    else if (protocol == "vless")
    {
        VLESSServerObject vless;
        vless.loadJson(outbound["vnext"].toArray().first());
        obj[IOBOUND::ADDRESS] = *vless.address;
        obj[IOBOUND::PORT] = *vless.port;
        return obj;
    }
    else if (protocol == "shadowsocks")
    {
        ShadowSocksServerObject ss;
        ss.loadJson(outbound["servers"].toArray().first());
        obj[IOBOUND::ADDRESS] = *ss.address;
        obj[IOBOUND::PORT] = *ss.port;
        return obj;
    }
    return std::nullopt;
}

bool BuiltinSerializer::SetOutboundInfo(const QString &protocol, QJsonObject &outbound, const PluginIOBoundData &info) const
{
    if ((QStringList{ "http", "socks", "shadowsocks" }).contains(protocol))
    {
        QJsonIO::SetValue(outbound, info[IOBOUND::ADDRESS].toString(), "servers", 0, "address");
        QJsonIO::SetValue(outbound, info[IOBOUND::PORT].toInt(), "servers", 0, "port");
        return true;
    }

    if ((QStringList{ "vless", "vmess" }).contains(protocol))
    {
        QJsonIO::SetValue(outbound, info[IOBOUND::ADDRESS].toString(), "vnext", 0, "address");
        QJsonIO::SetValue(outbound, info[IOBOUND::PORT].toInt(), "vnext", 0, "port");
        return true;
    }

    return false;
}

std::optional<QString> BuiltinSerializer::Serialize(const PluginOutboundDescriptor &info) const
{
    const auto protocol = info.Protocol;
    const auto oubound = info.Outbound;
    const auto stream = info.StreamSettings;
    const auto name = info.ConnectionName;
    if (protocol == "http" || protocol == "socks")
    {
        QUrl url;
        url.setScheme(protocol);
        url.setHost(QJsonIO::GetValue(oubound, { "servers", 0, "address" }).toString());
        url.setPort(QJsonIO::GetValue(oubound, { "servers", 0, "port" }).toInt());
        if (QJsonIO::GetValue(oubound, { "servers", 0 }).toObject().contains("users"))
        {
            url.setUserName(QJsonIO::GetValue(oubound, { "servers", 0, "users", 0, "user" }).toString());
            url.setPassword(QJsonIO::GetValue(oubound, { "servers", 0, "users", 0, "pass" }).toString());
        }
        return url.toString();
    }
    if (protocol == "vless")
    {
        QUrl url;
        url.setFragment(QUrl::toPercentEncoding(name));
        url.setScheme(protocol);
        url.setHost(QJsonIO::GetValue(oubound, { "vnext", 0, "address" }).toString());
        url.setPort(QJsonIO::GetValue(oubound, { "vnext", 0, "port" }).toInt());
        url.setUserName(QJsonIO::GetValue(oubound, { "vnext", 0, "users", 0, "id" }).toString());

        // -------- COMMON INFORMATION --------
        QUrlQuery query;
        const auto encryption = QJsonIO::GetValue(oubound, { "vnext", 0, "users", 0, "encryption" }).toString("none");
        if (encryption != "none")
            query.addQueryItem("encryption", encryption);

        const auto network = QJsonIO::GetValue(stream, "network").toString("tcp");
        if (network != "tcp")
            query.addQueryItem("type", network);

        const auto security = QJsonIO::GetValue(stream, "security").toString("none");
        if (security != "none")
            query.addQueryItem("security", security);

        // -------- TRANSPORT RELATED --------
        if (network == "kcp")
        {
            const auto seed = QJsonIO::GetValue(stream, { "kcpSettings", "seed" }).toString();
            if (!seed.isEmpty())
                query.addQueryItem("seed", QUrl::toPercentEncoding(seed));

            const auto headerType = QJsonIO::GetValue(stream, { "kcpSettings", "header", "type" }).toString("none");
            if (headerType != "none")
                query.addQueryItem("headerType", headerType);
        }
        else if (network == "http")
        {
            const auto path = QJsonIO::GetValue(stream, { "httpSettings", "path" }).toString("/");
            query.addQueryItem("path", QUrl::toPercentEncoding(path));

            const auto hosts = QJsonIO::GetValue(stream, { "httpSettings", "host" }).toArray();
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
            const auto path = QJsonIO::GetValue(stream, { "wsSettings", "path" }).toString("/");
            query.addQueryItem("path", QUrl::toPercentEncoding(path));

            const auto host = QJsonIO::GetValue(stream, { "wsSettings", "headers", "Host" }).toString();
            query.addQueryItem("host", host);
        }
        else if (network == "quic")
        {
            const auto quicSecurity = QJsonIO::GetValue(stream, { "quicSettings", "security" }).toString("none");
            if (quicSecurity != "none")
            {
                query.addQueryItem("quicSecurity", quicSecurity);

                const auto key = QJsonIO::GetValue(stream, { "quicSettings", "key" }).toString();
                query.addQueryItem("key", QUrl::toPercentEncoding(key));

                const auto headerType = QJsonIO::GetValue(stream, { "quicSettings", "header", "type" }).toString("none");
                if (headerType != "none")
                    query.addQueryItem("headerType", headerType);
            }
        }
        else if (network == "grpc")
        {
            const auto serviceName = QJsonIO::GetValue(stream, { "grpcSettings", "serviceName" }).toString("GunService");
            if (serviceName != "GunService")
                query.addQueryItem("serviceName", QUrl::toPercentEncoding(serviceName));

            const auto multiMode = QJsonIO::GetValue(stream, { "grpcSettings", "multiMode" }).toBool(false);
            if (multiMode)
                query.addQueryItem("mode", "multi");
        }
        // -------- TLS RELATED --------
        const auto tlsKey = security == "xtls" ? "xtlsSettings" : "tlsSettings";

        const auto sni = QJsonIO::GetValue(stream, { tlsKey, "serverName" }).toString();
        if (!sni.isEmpty())
            query.addQueryItem("sni", sni);

        // ALPN
        const auto alpnArray = QJsonIO::GetValue(stream, { tlsKey, "alpn" }).toArray();
        QStringList alpnList;
        for (const auto v : alpnArray)
        {
            const auto alpn = v.toString();
            if (!alpn.isEmpty())
                alpnList << alpn;
        }
        query.addQueryItem("alpn", QUrl::toPercentEncoding(alpnList.join(",")));

        // -------- XTLS Flow --------
        if (security == "xtls")
        {
            const auto flow = QJsonIO::GetValue(oubound, "vnext", 0, "users", 0, "flow").toString();
            query.addQueryItem("flow", flow);
        }

        // ======== END OF QUERY ========
        url.setQuery(query);
        return url.toString(QUrl::FullyEncoded);
    }

    return std::nullopt;
}

std::optional<PluginOutboundDescriptor> BuiltinSerializer::Deserialize(const QString &link) const
{
    if (link.startsWith("http://") || link.startsWith("socks://"))
    {
        const QUrl url = link;
        QJsonObject root;
        QJsonIO::SetValue(root, url.host(), "servers", 0, "address");
        QJsonIO::SetValue(root, url.port(), "servers", 0, "port");
        QJsonIO::SetValue(root, url.userName(), "servers", 0, "users", 0, "user");
        QJsonIO::SetValue(root, url.password(), "servers", 0, "users", 0, "pass");
        return PluginOutboundDescriptor{ url.fragment(), url.scheme(), root, {} };
    }

    return std::nullopt;
}
