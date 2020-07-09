#include "common/QvHelpers.hpp"
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

    void RemoveEmptyMuxFilter(CONFIGROOT &root)
    {
        for (auto i = 0; i < root["outbounds"].toArray().count(); i++)
        {
            if (!QJsonIO::GetValue(root, "outbounds", i, "mux", "enabled").toBool(false))
            {
                QJsonIO::SetValue(root, QJsonIO::Undefined, "outbounds", i, "mux");
            }
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
        const QJsonObject dnsRoutingRuleObj{ { "outboundTag", "dns-out" },
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
        for (auto i = 0; i < root["outbounds"].toArray().count(); i++)
        {
            bool isEmptySeed = QJsonIO::GetValue(root, "outbounds", i, "streamSettings", "kcpSettings", "seed").toString().isEmpty();
            if (isEmptySeed)
                QJsonIO::SetValue(root, QJsonIO::Undefined, "outbounds", i, "streamSettings", "kcpSettings", "seed");
        }
    }

    void FillupTagsFilter(CONFIGROOT &root, const QString &subKey)
    {
        for (auto i = 0; i < root[subKey].toArray().count(); i++)
        {
            if (QJsonIO::GetValue(root, subKey, i, "tag").toString().isEmpty())
            {
                LOG(MODULE_SETTINGS, "Adding a tag to an inbound.")
                const auto tag = GenerateRandomString(8);
                QJsonIO::SetValue(root, tag, subKey, i, "tag");
            }
        }
    }

} // namespace Qv2ray::core::connection::generation::filters
