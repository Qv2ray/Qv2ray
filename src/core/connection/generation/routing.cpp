#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::routing
{

    // -------------------------- BEGIN CONFIG GENERATIONS
    ROUTING GenerateRoutes(bool enableProxy, bool bypassCN, const QString &defaultOutboundTag)
    {
        auto &routeConfig = GlobalConfig.connectionConfig.routeConfig;
        ROUTING root;
        root.insert("domainStrategy", routeConfig.domainStrategy);
        //
        // For Rules list
        ROUTERULELIST rulesList;
        if (!enableProxy)
        {
            // This is added to disable all proxies, as a alternative
            // influence of #64
            rulesList.append(GenerateSingleRouteRule("regexp:.*", true, OUTBOUND_TAG_DIRECT));
            rulesList.append(GenerateSingleRouteRule("0.0.0.0/0", false, OUTBOUND_TAG_DIRECT));
            rulesList.append(GenerateSingleRouteRule("::/0", false, OUTBOUND_TAG_DIRECT));
        }

        // Private IPs should always NOT TO PROXY!
        rulesList.append(GenerateSingleRouteRule("geoip:private", false, OUTBOUND_TAG_DIRECT));
        //
        // To the route list.
        if (!routeConfig.domains.block.isEmpty())
        {
            rulesList.append(GenerateSingleRouteRule(routeConfig.domains.block, true, OUTBOUND_TAG_BLACKHOLE));
        }
        if (!routeConfig.domains.proxy.isEmpty())
        {
            rulesList.append(GenerateSingleRouteRule(routeConfig.domains.proxy, true, defaultOutboundTag));
        }
        if (!routeConfig.domains.direct.isEmpty())
        {
            rulesList.append(GenerateSingleRouteRule(routeConfig.domains.direct, true, OUTBOUND_TAG_DIRECT));
        }

        // IP list
        if (!routeConfig.ips.block.isEmpty())
        {
            rulesList.append(GenerateSingleRouteRule(routeConfig.ips.block, false, OUTBOUND_TAG_BLACKHOLE));
        }
        if (!routeConfig.ips.proxy.isEmpty())
        {
            rulesList.append(GenerateSingleRouteRule(routeConfig.ips.proxy, false, defaultOutboundTag));
        }
        if (!routeConfig.ips.direct.isEmpty())
        {
            rulesList.append(GenerateSingleRouteRule(routeConfig.ips.direct, false, OUTBOUND_TAG_DIRECT));
        }
        //
        // Check if CN needs proxy, or direct.
        if (bypassCN)
        {
            // No proxy agains CN addresses.
            rulesList.append(GenerateSingleRouteRule("geoip:cn", false, OUTBOUND_TAG_DIRECT));
            rulesList.append(GenerateSingleRouteRule("geosite:cn", true, OUTBOUND_TAG_DIRECT));
        }
        //
        //
        // As a bug fix of #64, this default rule has been disabled.
        // rulesList.append(GenerateSingleRouteRule(QStringList({"regexp:.*"}),
        // true, globalProxy ? OUTBOUND_TAG_PROXY :  OUTBOUND_TAG_DIRECT));
        root.insert("rules", rulesList);
        RROOT
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
        RROOT
    }

} // namespace Qv2ray::core::connection::generation::routing
