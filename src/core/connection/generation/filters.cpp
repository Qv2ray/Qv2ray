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

    void DNSInterceptFilter(CONFIGROOT &root)
    {
        // Static DNS Objects
        static const QJsonObject dnsOutboundObj{ { "protocol", "dns" }, { "tag", "dns-out" } };
        static const QJsonObject dnsRoutingRuleObj{ { "outboundTag", "dns-out" }, { "port", "53" }, { "type", "field" } };
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
} // namespace Qv2ray::core::connection::generation::filters
