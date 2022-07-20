#include "InternalProfilePreprocessor.hpp"

#include "Qv2rayApplication.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "QvPlugin/Utils/QJsonIO.hpp"

constexpr auto DNS_INTERCEPTION_OUTBOUND_TAG = "dns-out";
constexpr auto DEFAULT_FREEDOM_OUTBOUND_TAG = "direct";
constexpr auto DEFAULT_BLACKHOLE_OUTBOUND_TAG = "blackhole";

constexpr auto DEFAULT_DokodemoDoor_IPV4_TAG = "tproxy-in-1";
constexpr auto DEFAULT_DokodemoDoor_IPV6_TAG = "tproxy-in-2";

constexpr auto DEFAULT_SOCKS_IPV4_TAG = "socks-in-1";
constexpr auto DEFAULT_SOCKS_IPV6_TAG = "socks-in-2";

constexpr auto DEFAULT_HTTP_IPV4_TAG = "http-in-1";
constexpr auto DEFAULT_HTTP_IPV6_TAG = "http-in-2";

enum RuleType
{
    RULE_DOMAIN,
    RULE_IP
};

template<RuleType t>
inline RuleObject GenerateSingleRouteRule(const QStringList &rules, const QString &outboundTag)
{
    RuleObject r;
    r.outboundTag = outboundTag;
    if constexpr (t == RULE_DOMAIN)
        r.targetDomains = rules;
    else
        r.targetIPs = rules;
    return r;
}

// -------------------------- BEGIN CONFIG GENERATIONS
void ProcessRoutes(RoutingObject &root, bool ForceDirectConnection, bool bypassCN, bool bypassLAN, const QString &outTag, const RouteMatrixConfig &routeConfig)
{
    root.extraOptions.insert(u"domainStrategy"_qs, *routeConfig.domainStrategy);
    root.extraOptions.insert(u"domainMatcher"_qs, *routeConfig.domainMatcher);
    //
    // For Rules list
    QList<RuleObject> newRulesList;
    if (ForceDirectConnection)
    {
        // This is added to disable all proxies, as a alternative influence of #64
        newRulesList << GenerateSingleRouteRule<RULE_DOMAIN>({ "regexp:.*" }, DEFAULT_FREEDOM_OUTBOUND_TAG);
        newRulesList << GenerateSingleRouteRule<RULE_IP>({ "0.0.0.0/0" }, DEFAULT_FREEDOM_OUTBOUND_TAG);
        newRulesList << GenerateSingleRouteRule<RULE_IP>({ "::/0" }, DEFAULT_FREEDOM_OUTBOUND_TAG);
    }
    else
    {
        {
            // Domain rules
            if (!routeConfig.domains->block->isEmpty())
                newRulesList << GenerateSingleRouteRule<RULE_DOMAIN>(routeConfig.domains->block, DEFAULT_BLACKHOLE_OUTBOUND_TAG);

            if (!routeConfig.domains->proxy->isEmpty())
                newRulesList << GenerateSingleRouteRule<RULE_DOMAIN>(routeConfig.domains->proxy, outTag);

            if (!routeConfig.domains->direct->isEmpty())
                newRulesList << GenerateSingleRouteRule<RULE_DOMAIN>(routeConfig.domains->direct, DEFAULT_FREEDOM_OUTBOUND_TAG);

            if (bypassCN)
                newRulesList << GenerateSingleRouteRule<RULE_DOMAIN>({ "geosite:cn" }, DEFAULT_FREEDOM_OUTBOUND_TAG);
        }
        {
            // IP rules
            if (!routeConfig.ips->block->isEmpty())
                newRulesList << GenerateSingleRouteRule<RULE_IP>(routeConfig.ips->block, DEFAULT_BLACKHOLE_OUTBOUND_TAG);

            if (!routeConfig.ips->proxy->isEmpty())
                newRulesList << GenerateSingleRouteRule<RULE_IP>(routeConfig.ips->proxy, outTag);

            if (!routeConfig.ips->direct->isEmpty())
                newRulesList << GenerateSingleRouteRule<RULE_IP>(routeConfig.ips->direct, DEFAULT_FREEDOM_OUTBOUND_TAG);

            if (bypassLAN)
                newRulesList << GenerateSingleRouteRule<RULE_IP>({ "geoip:private" }, DEFAULT_FREEDOM_OUTBOUND_TAG);

            if (bypassCN)
                newRulesList << GenerateSingleRouteRule<RULE_IP>({ "geoip:cn" }, DEFAULT_FREEDOM_OUTBOUND_TAG);
        }
    }

    newRulesList << root.rules;
    root.rules = newRulesList;
}

ProfileContent InternalProfilePreprocessor::PreprocessProfile(const ProfileContent &_p)
{
    auto result = _p;
    if (result.defaultKernel.isNull())
        result.defaultKernel = GlobalConfig->behaviorConfig->DefaultKernelId;

    // For "complex" profiles.
    const auto needGeneration = result.inbounds.isEmpty() && result.routing.rules.isEmpty() && result.outbounds.size() == 1;

    if (!needGeneration)
        return result;

    if (result.outbounds.first().name.isEmpty())
        result.outbounds.first().name = u"Default"_qs;

    bool hasAddr1 = !GlobalConfig->inboundConfig->ListenAddress1->isEmpty();
    bool hasAddr2 = !GlobalConfig->inboundConfig->ListenAddress2->isEmpty();

#define AddInbound(PROTOCOL, _protocol, ...)                                                                                                                             \
    do                                                                                                                                                                   \
    {                                                                                                                                                                    \
        if (GlobalConfig->inboundConfig->Has##PROTOCOL)                                                                                                                  \
        {                                                                                                                                                                \
            InboundObject in;                                                                                                                                            \
            in.inboundSettings.protocol = u"" _protocol##_qs;                                                                                                            \
            GlobalConfig->inboundConfig->PROTOCOL##Config->Propagate(in);                                                                                                \
            if (hasAddr1)                                                                                                                                                \
            {                                                                                                                                                            \
                in.name = QString::fromUtf8(DEFAULT_##PROTOCOL##_IPV4_TAG);                                                                                              \
                in.inboundSettings.address = GlobalConfig->inboundConfig->ListenAddress1;                                                                                \
                __VA_ARGS__;                                                                                                                                             \
                result.inbounds << in;                                                                                                                                   \
            }                                                                                                                                                            \
            if (hasAddr2)                                                                                                                                                \
            {                                                                                                                                                            \
                in.name = QString::fromUtf8(DEFAULT_##PROTOCOL##_IPV6_TAG);                                                                                              \
                in.inboundSettings.address = GlobalConfig->inboundConfig->ListenAddress2;                                                                                \
                __VA_ARGS__;                                                                                                                                             \
                result.inbounds << in;                                                                                                                                   \
            }                                                                                                                                                            \
        }                                                                                                                                                                \
    } while (false)

    const auto dokoMode = [](auto m)
    {
        switch (m)
        {
            case Qv2ray::Models::DokodemoDoorInboundConfig::TPROXY: return u"tproxy"_qs;
            case Qv2ray::Models::DokodemoDoorInboundConfig::REDIRECT: return u"redirect"_qs;
        }
        return u"redirect"_qs;
    }(GlobalConfig->inboundConfig->DokodemoDoorConfig->WorkingMode);

    AddInbound(HTTP, "http", {});
    AddInbound(SOCKS, "socks", {});
    AddInbound(DokodemoDoor, "dokodemo-door", in.inboundSettings.streamSettings[u"sockopt"_qs] = QJsonObject{ { u"tproxy"_qs, dokoMode } });

    const auto routeMatrixConfig = RouteMatrixConfig::fromJson(result.routing.extraOptions[RouteMatrixConfig::EXTRA_OPTIONS_ID].toObject());

    ProcessRoutes(result.routing,                                        //
                  GlobalConfig->connectionConfig->ForceDirectConnection, //
                  GlobalConfig->connectionConfig->BypassCN,              //
                  GlobalConfig->connectionConfig->BypassLAN,             //
                  result.outbounds.first().name,                         //
                  routeMatrixConfig);

    if (GlobalConfig->connectionConfig->BypassBittorrent)
    {
        RuleObject r;
        r.protocols.append(u"bittorrent"_qs);
        r.outboundTag = QString::fromUtf8(DEFAULT_FREEDOM_OUTBOUND_TAG);
        result.routing.rules.prepend(r);
    }

    if (GlobalConfig->connectionConfig->DNSInterception)
    {
        QStringList dnsRuleInboundTags;
        if (GlobalConfig->inboundConfig->HasDokodemoDoor)
        {
            if (hasAddr1)
                dnsRuleInboundTags.append(QString::fromUtf8(DEFAULT_DokodemoDoor_IPV4_TAG));
            if (hasAddr2)
                dnsRuleInboundTags.append(QString::fromUtf8(DEFAULT_DokodemoDoor_IPV6_TAG));
        }

        if (GlobalConfig->inboundConfig->HasSOCKS)
        {
            if (hasAddr1)
                dnsRuleInboundTags.append(QString::fromUtf8(DEFAULT_SOCKS_IPV4_TAG));
            if (hasAddr2)
                dnsRuleInboundTags.append(QString::fromUtf8(DEFAULT_SOCKS_IPV6_TAG));
        }

        // If no UDP inbound, then DNS outbound is useless.
        if (!dnsRuleInboundTags.isEmpty())
        {
            IOConnectionSettings _dns;
            _dns.protocol = u"dns"_qs;
            auto DNSOutbound = OutboundObject(_dns);
            DNSOutbound.name = QString::fromUtf8(DNS_INTERCEPTION_OUTBOUND_TAG);

            RuleObject DNSRule;
            DNSRule.targetPort = 53;
            DNSRule.inboundTags = dnsRuleInboundTags;
            DNSRule.outboundTag = QString::fromUtf8(DNS_INTERCEPTION_OUTBOUND_TAG);

            result.outbounds << DNSOutbound;
            result.routing.rules.prepend(DNSRule);
        }
    }
    {
        // Generate Blackhole
        OutboundObject black{ IOConnectionSettings{ u"blackhole"_qs, u"0.0.0.0"_qs, 0 } };
        black.name = QString::fromUtf8(DEFAULT_BLACKHOLE_OUTBOUND_TAG);
        result.outbounds << black;

        // Generate Freedom
        OutboundObject freedom{ IOConnectionSettings{ u"freedom"_qs, u"0.0.0.0"_qs, 0 } };
        freedom.name = QString::fromUtf8(DEFAULT_FREEDOM_OUTBOUND_TAG);
        if (GlobalConfig->connectionConfig->UseDirectOutboundAsPrimary)
            result.outbounds.prepend(freedom);
        else
            result.outbounds.append(freedom);
    }

    return result;
}
