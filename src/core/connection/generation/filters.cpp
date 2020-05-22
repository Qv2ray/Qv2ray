#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::filters
{
    void OutboundMarkSettingFilter(const int mark, CONFIGROOT &root)
    {
        QJsonObject sockoptObj{ { "mark", mark } };
        QJsonObject streamSettingsObj{ { "sockopt", sockoptObj } };
        OUTBOUNDS outbounds(root["outbounds"].toArray());
        for (auto i = 0; i < outbounds.count(); i++)
        {
            auto _outbound = outbounds[i].toObject();
            if (_outbound.contains("streamSettings"))
            {
                auto _streamSetting = _outbound["streamSettings"].toObject();
                if (_streamSetting.contains("sockopt"))
                {
                    auto _sockopt = _streamSetting["sockopt"].toObject();
                    _sockopt.insert("mark", mark);
                    _streamSetting["sockopt"] = _sockopt;
                }
                else
                {
                    _streamSetting.insert("sockopt", sockoptObj);
                }
                _outbound["streamSettings"] = _streamSetting;
            }
            else
            {
                _outbound.insert("streamSettings", streamSettingsObj);
            }
            outbounds[i] = _outbound;
        }
        root["outbounds"] = outbounds;
    }

    void DNSInterceptFilter(CONFIGROOT &root)
    {
        // dns outBound
        QJsonObject dnsOutboundObj{ { "protocol", "dns" }, { "tag", "dns-out" } };
        OUTBOUNDS outbounds(root["outbounds"].toArray());
        outbounds.append(dnsOutboundObj);
        root["outbounds"] = outbounds;

        // dns route
        QJsonObject dnsRoutingRuleObj{ { "outboundTag", "dns-out" }, { "port", "53" }, { "type", "field" } };
        ROUTING routing(root["routing"].toObject());
        QJsonArray _rules(routing["rules"].toArray());
        _rules.insert(0, dnsRoutingRuleObj);
        routing["rules"] = _rules;
        root["routing"] = routing;
    }

    void bypassBTFilter(CONFIGROOT &root)
    {
        QJsonObject bypassBTRuleObj{ { "protocol", QJsonArray::fromStringList(QStringList{ "bittorrent" }) },
                                     { "outboundTag", OUTBOUND_TAG_DIRECT },
                                     { "type", "field" } };
        ROUTING routing(root["routing"].toObject());
        QJsonArray _rules(routing["rules"].toArray());
        _rules.insert(0, bypassBTRuleObj);
        routing["rules"] = _rules;
        root["routing"] = routing;
    }
} // namespace Qv2ray::core::connection::generation::filters
