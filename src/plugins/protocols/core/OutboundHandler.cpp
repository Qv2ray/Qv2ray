#include "OutboundHandler.hpp"

#include "3rdparty/QJsonStruct/QJsonIO.hpp"

#include <QUrl>

const Qv2rayPlugin::OutboundInfoObject BuiltinSerializer::GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const
{
    Qv2rayPlugin::OutboundInfoObject obj;
    obj[Qv2rayPlugin::INFO_PROTOCOL] = protocol;
    if (protocol == "http")
    {
        const auto http = HttpServerObject::fromJson(outbound["servers"].toArray().first());
        obj[Qv2rayPlugin::INFO_SERVER] = http.address;
        obj[Qv2rayPlugin::INFO_PORT] = http.port;
    }
    else if (protocol == "socks")
    {
        const auto socks = SocksServerObject::fromJson(outbound["servers"].toArray().first());
        obj[Qv2rayPlugin::INFO_SERVER] = socks.address;
        obj[Qv2rayPlugin::INFO_PORT] = socks.port;
    }
    else if (protocol == "vmess")
    {
        const auto vmess = VMessServerObject::fromJson(outbound["vnext"].toArray().first());
        obj[Qv2rayPlugin::INFO_SERVER] = vmess.address;
        obj[Qv2rayPlugin::INFO_PORT] = vmess.port;
    }
    else if (protocol == "vless")
    {
        const auto vless = VLESSServerObject::fromJson(outbound["vnext"].toArray().first());
        obj[Qv2rayPlugin::INFO_SERVER] = vless.address;
        obj[Qv2rayPlugin::INFO_PORT] = vless.port;
    }
    else if (protocol == "shadowsocks")
    {
        const auto ss = ShadowSocksServerObject::fromJson(outbound["servers"].toArray().first());
        obj[Qv2rayPlugin::INFO_SERVER] = ss.address;
        obj[Qv2rayPlugin::INFO_PORT] = ss.port;
    }
    return obj;
}

const QString BuiltinSerializer::SerializeOutbound(const QString &protocol, const QString &alias, const QString &group, const QJsonObject &obj) const
{
    Q_UNUSED(group)
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
    return "(Unsupported)";
}

const QPair<QString, QJsonObject> BuiltinSerializer::DeserializeOutbound(const QString &link, QString *alias, QString *errorMessage) const
{
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
