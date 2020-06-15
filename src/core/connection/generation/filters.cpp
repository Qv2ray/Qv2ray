#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::filters
{
    void OutboundMarkSettingFilter(const int mark, CONFIGROOT &root)
    {
        for (auto i = 0; i < root["outbounds"].toArray().count(); i++)
        {
            QJsonIO::SetValue(root, mark, "outbounds", i, "streamSettings", "sockopt", "mark");
        }
    }

    void DNSInterceptFilter(CONFIGROOT &root, const bool have_ipv6)
    {
        // Static DNS Objects
        static const QJsonObject dnsOutboundObj{ { "protocol", "dns" }, { "tag", "dns-out" } };
        QJsonArray dnsRouteInTag;
        if (have_ipv6)
        {
            dnsRouteInTag = QJsonArray{ "tproxy_IN", "tproxy_IN_V6" };
        }
        else
        {
            dnsRouteInTag = QJsonArray{ "tproxy_IN" };
        }
        static const QJsonObject dnsRoutingRuleObj{ { "outboundTag", "dns-out" },
                                                    { "port", "53" },
                                                    { "type", "field" },
                                                    { "inboundTag", dnsRouteInTag } };
        // DNS Outbound
        QJsonIO::SetValue(root, dnsOutboundObj, "outbounds", root["outbounds"].toArray().count());
        // DNS Route
        auto _rules = QJsonIO::GetValue(root, "routing", "rules").toArray();
        _rules.insert(0, dnsRoutingRuleObj);
        QJsonIO::SetValue(root, _rules, "routing", "rules");
    }

    void BypassBTFilter(CONFIGROOT &root)
    {
        static const QJsonObject bypassBTRuleObj{ { "protocol", QJsonArray{ "bittorrent" } },
                                                  { "outboundTag", OUTBOUND_TAG_DIRECT },
                                                  { "type", "field" } };
        auto _rules = QJsonIO::GetValue(root, "routing", "rules").toArray();
        _rules.insert(0, bypassBTRuleObj);
        QJsonIO::SetValue(root, _rules, "routing", "rules");
    }

    void mKCPSeedFilter(CONFIGROOT &root)
    {
        const auto outboundCount = root["outbounds"].toArray().count();
        for (auto i = 0; i < outboundCount; i++)
        {
            bool isKCP = QJsonIO::GetValue(root, "outbounds", i, "streamSettings", "network").toString() == "kcp";
            if (isKCP)
            {
                bool isEmptySeed = QJsonIO::GetValue(root, "outbounds", i, "streamSettings", "kcpSettings", "seed").toString().isEmpty();
                if (isEmptySeed)
                    QJsonIO::SetValue(root, QJsonIO::Undefined, "outbounds", i, "streamSettings", "kcpSettings", "seed");
            }
        }
    }

} // namespace Qv2ray::core::connection::generation::filters
