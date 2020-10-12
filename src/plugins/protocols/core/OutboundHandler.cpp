#include "OutboundHandler.hpp"

#include "3rdparty/QJsonStruct/QJsonIO.hpp"

#include <QUrl>

using namespace Qv2rayPlugin;

const Qv2rayPlugin::OutboundInfoObject BuiltinSerializer::GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const
{
    OutboundInfoObject obj;
    obj[INFO_PROTOCOL] = protocol;
    if (protocol == "http")
    {
        const auto http = HttpServerObject::fromJson(outbound["servers"].toArray().first());
        obj[INFO_SERVER] = http.address;
        obj[INFO_PORT] = http.port;
    }
    else if (protocol == "socks")
    {
        const auto socks = SocksServerObject::fromJson(outbound["servers"].toArray().first());
        obj[INFO_SERVER] = socks.address;
        obj[INFO_PORT] = socks.port;
    }
    else if (protocol == "vmess")
    {
        const auto vmess = VMessServerObject::fromJson(outbound["vnext"].toArray().first());
        obj[INFO_SERVER] = vmess.address;
        obj[INFO_PORT] = vmess.port;
    }
    else if (protocol == "vless")
    {
        const auto vless = VLESSServerObject::fromJson(outbound["vnext"].toArray().first());
        obj[INFO_SERVER] = vless.address;
        obj[INFO_PORT] = vless.port;
    }
    else if (protocol == "shadowsocks")
    {
        const auto ss = ShadowSocksServerObject::fromJson(outbound["servers"].toArray().first());
        obj[INFO_SERVER] = ss.address;
        obj[INFO_PORT] = ss.port;
    }
    return obj;
}

const void BuiltinSerializer::SetOutboundInfo(const QString &protocol, const Qv2rayPlugin::OutboundInfoObject &info, QJsonObject &outbound) const
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
                                                   const QJsonObject &) const
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
