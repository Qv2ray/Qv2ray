#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::routing
{
    QJsonObject GenerateDNS(bool withLocalhost, const QvConfig_DNS &dnsServer)
    {
        QJsonObject root = dnsServer.toJson();
        QJsonArray servers;
        for (const auto &serv : dnsServer.servers)
        {
            servers << (serv.QV2RAY_DNS_IS_COMPLEX_DNS ? serv.toJson() : QJsonValue(serv.address));
        }
        if (withLocalhost)
            servers.push_front("localhost");
        root["servers"] = servers;
        JAUTOREMOVE(root, "clientIp");
        JAUTOREMOVE(root, "hosts");
        JAUTOREMOVE(root, "tag");
        return root;
    }

    ROUTERULE GenerateSingleRouteRule(const QString &str, bool isDomain, const QString &outboundTag, const QString &type)
    {
        return GenerateSingleRouteRule(QStringList{ str }, isDomain, outboundTag, type);
    }

    ROUTERULE GenerateSingleRouteRule(const QStringList &rules, bool isDomain, const QString &outboundTag, const QString &type)
    {
        ROUTERULE root;
        auto list = rules;
        list.removeAll("");
        root.insert(isDomain ? "domain" : "ip", QJsonArray::fromStringList(rules));
        JADD(outboundTag, type)
        return root;
    }

} // namespace Qv2ray::core::connection::generation::routing
