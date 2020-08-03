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

    ROUTERULE GenerateSingleRouteRule(RuleType t, const QString &str, const QString &outboundTag, const QString &type)
    {
        return GenerateSingleRouteRule(t, QStringList{ str }, outboundTag, type);
    }

    ROUTERULE GenerateSingleRouteRule(RuleType t, const QStringList &rules, const QString &outboundTag, const QString &type)
    {
        ROUTERULE root;
        auto list = rules;
        list.removeAll("");
        switch (t)
        {
            case RULE_IPS: root.insert("domain", QJsonArray::fromStringList(rules)); break;
            case RULE_DOMAINS: root.insert("ip", QJsonArray::fromStringList(rules)); break;
            default: Q_UNREACHABLE();
        }
        JADD(outboundTag, type)
        return root;
    }

} // namespace Qv2ray::core::connection::generation::routing
