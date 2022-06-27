#include "ProfileGenerator.hpp"

#include "QvPlugin/Utils/QJsonIO.hpp"
#include "V2RayCorePluginTemplate.hpp"
#include "V2RayModels.hpp"
#include "toml.hpp"

#include <QJsonDocument>
#include <QRandomGenerator>
#include <iostream>

constexpr auto DEFAULT_API_TAG = "qv2ray-api";
constexpr auto DEFAULT_API_IN_TAG = "qv2ray-api-in";

namespace
{
    QString ToHost(const QString &addr, int port)
    {
        if (addr.contains(QChar::fromLatin1(':')))
        {
            // ipv6
            return u"["_qs + addr + u"]:"_qs + QString::number(port);
        }
        else
        {
            return addr + u":"_qs + QString::number(port);
        }
    }

    class QByteArrayAppender
        : public std::ostream
        , public std::streambuf
    {
      private:
        QByteArray &m_byteArray;

      public:
        explicit QByteArrayAppender(QByteArray &byteArray) : std::ostream(this), std::streambuf(), m_byteArray(byteArray)
        {
        }

        int overflow(int c) override
        {
            m_byteArray.append(static_cast<char>(c));
            return 0;
        }
    };

    std::string GetRandomString()
    {
        const std::string possibleCharacters{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" };
        const int randomStringLength = 12; // assuming you want random strings of 12 characters
        std::string randomString;
        for (int i = 0; i < randomStringLength; ++i)
        {
            auto index = QRandomGenerator::global()->generate() % possibleCharacters.length();
            auto nextChar = possibleCharacters.at(index);
            randomString.push_back(nextChar);
        }
        return randomString;
    }

    void AddToTable(toml::table &tbl, const char *name, toml::array &a)
    {
        if (!a.empty())
            tbl.emplace<toml::array>(name, a);
    }

    void AddToTable(toml::table &tbl, const char *name, toml::table &a)
    {
        if (!a.empty())
            tbl.emplace<toml::table>(name, a);
    }
} // namespace

inline void OutboundMarkSettingFilter(QJsonObject &root, int mark)
{
    for (auto i = 0; i < root[u"outbounds"_qs].toArray().count(); i++)
        QJsonIO::SetValue(root, mark, u"outbounds"_qs, i, u"streamSettings"_qs, u"sockopt"_qs, u"mark"_qs);
}

V2RayRustProfileGenerator::V2RayRustProfileGenerator(const ProfileContent &profile) : profile(profile)
{
}

QByteArray V2RayRustProfileGenerator::GenerateConfiguration(const ProfileContent &p, QMap<QString, QString> &tagProtocolMap)
{
    V2RayRustProfileGenerator generator{ p };
    auto ret = generator.Generate();
    generator.SwapTagProtocolMap(tagProtocolMap);
    return ret;
}

QByteArray V2RayRustProfileGenerator::Generate()
{
    // QJsonObject rootconf;
    toml::table root_table;
    // JsonStructHelper::MergeJson(rootconf, profile.extraOptions);

    for (const auto &in : profile.inbounds)
        ProcessInboundConfig(in);

    for (const auto &out : profile.outbounds)
        if (out.objectType == OutboundObject::ORIGINAL)
            ProcessOutboundConfig(out);
        else if (out.objectType == OutboundObject::BALANCER)
            ProcessBalancerConfig(out);

    for (const auto &rule : profile.routing.rules)
        ProcessRoutingRule(rule);

    // Override log level
    const auto settings = V2RayCorePluginClass::PluginInstance->settings;
#define AddToRootTable(name) AddToTable(root_table, #name, name)
    AddToRootTable(trojan);
    AddToTable(root_table, "ss", shadowsocks);
    AddToRootTable(vmess);
    AddToRootTable(tls);
    AddToTable(root_table, "ws", websocket);
    AddToRootTable(h2);
    AddToRootTable(grpc);
    AddToRootTable(direct);
    AddToRootTable(blackhole);
    AddToRootTable(outbounds);
    AddToRootTable(inbounds);
    AddToRootTable(dokodemo);
    AddToRootTable(domain_routing_rules);
    AddToRootTable(ip_routing_rules);
    AddToRootTable(geosite_rules);
    AddToRootTable(geoip_rules);

    if (settings.APIEnabled)
    {
        root_table.emplace<bool>("enable_api_server", true);
        root_table.emplace<std::string>("api_server_addr", (u"127.0.0.1:"_qs + QString::number(*settings.APIPort)).toStdString());
    }
    QByteArray byteArray;
    QByteArrayAppender appender{ byteArray };
    appender << root_table;
    V2RayCorePluginClass::Log(u"v2ray-rust root_table: "_qs + QString::fromUtf8(byteArray));
    return byteArray;
}

void V2RayRustProfileGenerator::ProcessRoutingRule(const RuleObject &r)
{
    const auto out = findOutbound(r.outboundTag);
    if (out.objectType == OutboundObject::ORIGINAL)
    {
        const auto &settings = V2RayCorePluginClass::PluginInstance->settings;
        toml::array geoip_str;
        toml::array cidr;
        for (auto &ip : r.targetIPs)
        {
            if (ip.startsWith(u"geoip:"))
            {
                geoip_str.emplace_back<std::string>(ip.mid(6).toStdString());
            }
            else if (ip.startsWith(u"ext:") && ip.size() > 4)
            {
                auto last = ip.lastIndexOf(QChar(u':'));
                toml::table tbl;
                toml::array array;
                tbl.emplace<std::string>("file_path", QDir(settings.AssetsPath).filePath(ip.mid(4, last - 4)).toStdString());
                tbl.emplace<std::string>("tag", r.outboundTag.toStdString());
                array.emplace_back<std::string>(ip.toStdString());
                tbl.emplace<toml::array>("rules", std::move(array));
                geoip_rules.emplace_back<toml::table>(std::move(tbl));
            }
            else if (ip.startsWith(u"ext-ip:") && ip.size() > 7)
            {
                auto last = ip.lastIndexOf(QChar(u':'));
                toml::table tbl;
                toml::array array;
                tbl.emplace<std::string>("file_path", QDir(settings.AssetsPath).filePath(ip.mid(7, last - 7)).toStdString());
                tbl.emplace<std::string>("tag", r.outboundTag.toStdString());
                array.emplace_back<std::string>(ip.toStdString());
                tbl.emplace<toml::array>("rules", std::move(array));
                geoip_rules.emplace_back<toml::table>(std::move(tbl));
            }
            else
            {
                cidr.emplace_back<std::string>(ip.toStdString());
            }
        }
        if (!geoip_str.empty())
        {
            toml::table tbl;
            tbl.emplace<std::string>("tag", r.outboundTag.toStdString());
            tbl.emplace<toml::array>("rules", std::move(geoip_str));
            geoip_rules.emplace_back<toml::table>(std::move(tbl));
        }
        if (!cidr.empty())
        {
            toml::table tbl;
            tbl.emplace<std::string>("tag", r.outboundTag.toStdString());
            tbl.emplace<toml::array>("cidr_rules", std::move(cidr));
            ip_routing_rules.emplace_back<toml::table>(std::move(tbl));
        }
        toml::array full_rules;
        toml::array domain_rules;
        toml::array regex_rules;
        toml::array substr_rules;
        toml::array geosite_str;
        for (auto &domain : r.targetDomains)
        {
            if (domain.startsWith(u"geosite:"))
            {
                geosite_str.emplace_back<std::string>(domain.mid(8).toStdString());
            }
            else if (domain.startsWith(u"ext:") && domain.size() > 4)
            {
                auto last = domain.lastIndexOf(QChar(u':'));
                toml::table tbl;
                toml::array array;
                tbl.emplace<std::string>("file_path", QDir(settings.AssetsPath).filePath(domain.mid(4, last - 4)).toStdString());
                tbl.emplace<std::string>("tag", r.outboundTag.toStdString());
                array.emplace_back<std::string>(domain.toStdString());
                tbl.emplace<toml::array>("rules", std::move(array));
                geosite_rules.emplace_back<toml::table>(std::move(tbl));
            }
            else if (domain.startsWith(u"full:") && domain.size() > 5)
            {
                full_rules.emplace_back<std::string>(domain.mid(5).toStdString());
            }
            else if (domain.startsWith(u"domain:") && domain.size() > 7)
            {
                domain_rules.emplace_back<std::string>(domain.mid(7).toStdString());
            }
            else if (domain.startsWith(u"regexp:") && domain.size() > 7)
            {
                regex_rules.emplace_back<std::string>(domain.mid(7).toStdString());
            }
            else
            {
                substr_rules.emplace_back<std::string>(domain.toStdString());
            }
        }
        if (!geosite_str.empty())
        {
            toml::table tbl;
            tbl.emplace<std::string>("tag", r.outboundTag.toStdString());
            tbl.emplace<toml::array>("rules", std::move(geosite_str));
            geosite_rules.emplace_back<toml::table>(std::move(tbl));
        }
        if (!full_rules.empty() || !substr_rules.empty() || !regex_rules.empty() || !domain_rules.empty())
        {
            toml::table tbl;
            tbl.emplace<std::string>("tag", r.outboundTag.toStdString());
            tbl.emplace<toml::array>("full_rules", std::move(full_rules));
            tbl.emplace<toml::array>("domain_rules", std::move(domain_rules));
            tbl.emplace<toml::array>("regex_rules", std::move(regex_rules));
            tbl.emplace<toml::array>("substr_rules", std::move(substr_rules));
            domain_routing_rules.emplace_back<toml::table>(std::move(tbl));
        }
    }
}

void V2RayRustProfileGenerator::ProcessInboundConfig(const InboundObject &in)
{

    // Special Case: HTTP, SOCKS: with flattened users[]
    if (in.inboundSettings.protocol == u"http"_qs || in.inboundSettings.protocol == u"socks"_qs)
    {
        toml::table table;
        table.emplace<std::string>("tag", in.name.toStdString());
        table.emplace<std::string>("addr", ToHost(in.inboundSettings.address, in.inboundSettings.port.from).toStdString());
        if (in.inboundSettings.protocolSettings.contains(u"udp"_qs) && in.inboundSettings.protocolSettings[u"udp"_qs].toBool())
        {
            table.emplace<bool>("enable_udp", true);
        }
        inbounds.emplace_back<toml::table>(table);
    }

    if (in.inboundSettings.protocol == u"dokodemo-door"_qs)
    {
        if (in.inboundSettings.streamSettings.contains(u"sockopt"_qs))
        {
            if (in.inboundSettings.streamSettings[u"sockopt"_qs].toObject().contains(u"tproxy"_qs))
            {
                toml::table table;
                table.emplace<std::string>("tag", in.name.toStdString());
                table.emplace<std::string>("addr", ToHost(in.inboundSettings.address, in.inboundSettings.port.from).toStdString());
                table.emplace<bool>("tproxy", true);
                dokodemo.emplace_back<toml::table>(table);
            }
        }
    }
}

void V2RayRustProfileGenerator::ProcessOutboundConfig(const OutboundObject &out)
{
    assert(out.objectType == OutboundObject::ORIGINAL);
    toml::table outbound_tbl;
    outbound_tbl.emplace<std::string>("tag", out.name.toStdString());

    QJsonObject root;
    root[u"streamSettings"_qs] = GenerateStreamSettings(out.outboundSettings.streamSettings);
    Qv2ray::Models::StreamSettingsObject streamSettingsObject;
    streamSettingsObject.loadJson(out.outboundSettings.streamSettings);

    std::string protocol_random_tag;
    std::string tls_random_tag;
    std::string websocket_random_tag;
    std::string h2_random_tag;
    std::string grpc_random_tag;

    if (out.outboundSettings.protocol == u"freedom"_qs)
    {
        InsertTagProtocolMap(out.name, u"freedom"_qs);
        protocol_random_tag = GetRandomString();
        toml::table tbl;
        // random tag
        tbl.emplace<std::string>("tag", protocol_random_tag);
        direct.emplace_back<toml::table>(std::move(tbl));
    }

    if (out.outboundSettings.protocol == u"blackhole"_qs)
    {
        protocol_random_tag = GetRandomString();
        toml::table tbl;
        // random tag
        tbl.emplace<std::string>("tag", protocol_random_tag);
        blackhole.emplace_back<toml::table>(std::move(tbl));
    }

    if (out.outboundSettings.protocol == u"trojan"_qs)
    {
        InsertTagProtocolMap(out.name, u"trojan"_qs);
        protocol_random_tag = GetRandomString();
        Qv2ray::Models::TrojanClientObject trojanClientObject;
        trojanClientObject.loadJson(out.outboundSettings.protocolSettings);
        toml::table tbl;
        tbl.emplace<std::string>("tag", protocol_random_tag);
        tbl.emplace<std::string>("addr", ToHost(out.outboundSettings.address, out.outboundSettings.port.from).toStdString());
        tbl.emplace<std::string>("password", trojanClientObject.password->toStdString());
        trojan.push_back(std::move(tbl));
    }

    if (out.outboundSettings.protocol == u"vmess"_qs)
    {
        InsertTagProtocolMap(out.name, u"vmess"_qs);
        protocol_random_tag = GetRandomString();
        Qv2ray::Models::VMessClientObject serv;
        serv.loadJson(out.outboundSettings.protocolSettings);
        toml::table tbl;
        tbl.emplace<std::string>("tag", protocol_random_tag);
        tbl.emplace<std::string>("addr", ToHost(out.outboundSettings.address, out.outboundSettings.port.from).toStdString());
        tbl.emplace<std::string>("method", serv.security->toStdString());
        tbl.emplace<std::string>("uuid", serv.id->toStdString());
        vmess.push_back(std::move(tbl));
    }

    if (out.outboundSettings.protocol == u"shadowsocks"_qs)
    {
        InsertTagProtocolMap(out.name, u"shadowsocks"_qs);
        protocol_random_tag = GetRandomString();
        Qv2ray::Models::ShadowSocksClientObject ss;
        ss.loadJson(out.outboundSettings.protocolSettings);
        toml::table tbl;
        tbl.emplace<std::string>("tag", protocol_random_tag);
        tbl.emplace<std::string>("addr", ToHost(out.outboundSettings.address, out.outboundSettings.port.from).toStdString());
        tbl.emplace<std::string>("method", ss.method->toStdString());
        tbl.emplace<std::string>("password", ss.password->toStdString());
        shadowsocks.push_back(std::move(tbl));
    }

    if (streamSettingsObject.security == u"tls"_qs)
    {
        toml::table tbl;
        // random tag
        tls_random_tag = GetRandomString();
        tbl.emplace<std::string>("tag", tls_random_tag);
        if (streamSettingsObject.tlsSettings->serverName->isEmpty())
        {
            tbl.emplace<std::string>("sni", out.outboundSettings.address.toStdString());
        }
        else
        {
            tbl.emplace<std::string>("sni", streamSettingsObject.tlsSettings->serverName->toStdString());
        }
        tls.emplace_back<toml::table>(std::move(tbl));
    }

    if (streamSettingsObject.network == u"h2"_qs)
    {
        toml::table tbl;
        h2_random_tag = GetRandomString();
        tbl.emplace<std::string>("tag", h2_random_tag);
        if (!streamSettingsObject.httpSettings->host->isEmpty())
        {
            toml::array t;
            for (auto &h : *streamSettingsObject.httpSettings->host)
            {
                t.emplace_back<std::string>(h.toStdString());
            }
            tbl.emplace<toml::array>("hosts", t);
        }
        tbl.emplace<std::string>("path", streamSettingsObject.httpSettings->path->toStdString());
        if (!streamSettingsObject.httpSettings->method->isEmpty())
        {
            tbl.emplace<std::string>("method", streamSettingsObject.httpSettings->method->toStdString());
        }
        h2.emplace_back<toml::table>(std::move(tbl));
    }

    if (streamSettingsObject.network == u"grpc"_qs)
    {
        toml::table tbl;
        grpc_random_tag = GetRandomString();
        tbl.emplace<std::string>("tag", grpc_random_tag);
        tbl.emplace<std::string>("host", ToHost(out.outboundSettings.address, out.outboundSettings.port.from).toStdString());
        tbl.emplace<std::string>("service_name", streamSettingsObject.grpcSettings->serviceName->toStdString());
        grpc.emplace_back<toml::table>(std::move(tbl));
    }

    if (streamSettingsObject.network == u"ws"_qs)
    {
        toml::table tbl;
        // random tag
        websocket_random_tag = GetRandomString();
        tbl.emplace<std::string>("tag", websocket_random_tag);
        tbl.emplace<std::string>(
            "uri", (u"ws://"_qs + ToHost(out.outboundSettings.address, out.outboundSettings.port.from) + streamSettingsObject.wsSettings->path).toStdString());
        if (!streamSettingsObject.wsSettings->headers->isEmpty())
        {
            toml::table headers;
            for (auto it = streamSettingsObject.wsSettings->headers->keyValueBegin(); it != streamSettingsObject.wsSettings->headers->keyValueEnd(); ++it)
            {
                headers.emplace<std::string>(it->first.toStdString(), it->second.toStdString());
            }
            tbl.emplace<toml::table>("headers", std::move(headers));
        }
        auto max_early_data = *(streamSettingsObject.wsSettings->maxEarlyData);
        if (max_early_data > 0)
        {
            tbl.emplace<int64_t>("max_early_data", max_early_data);
            if (streamSettingsObject.wsSettings->earlyDataHeaderName->isEmpty())
            {
                tbl.emplace<std::string>("early_data_header_name", "Sec-WebSocket-Protocol");
            }
            else
            {
                tbl.emplace<std::string>("early_data_header_name", streamSettingsObject.wsSettings->earlyDataHeaderName->toStdString());
            }
        }
        websocket.emplace_back<toml::table>(std::move(tbl));
    }
    if (!protocol_random_tag.empty())
    {
        toml::array chain;
        if (!tls_random_tag.empty())
        {
            chain.emplace_back<std::string>(std::move(tls_random_tag));
        }
        if (!h2_random_tag.empty())
        {
            chain.emplace_back<std::string>(std::move(h2_random_tag));
        }
        if (!grpc_random_tag.empty())
        {
            chain.emplace_back<std::string>(std::move(grpc_random_tag));
        }
        if (!websocket_random_tag.empty())
        {
            chain.emplace_back<std::string>(std::move(websocket_random_tag));
        }
        if (!protocol_random_tag.empty())
        {
            chain.emplace_back<std::string>(std::move(protocol_random_tag));
        }
        outbound_tbl.emplace<toml::array>("chain", std::move(chain));
        outbounds.emplace_back<toml::table>(std::move(outbound_tbl));
    }
}

void V2RayRustProfileGenerator::ProcessBalancerConfig(const OutboundObject &out)
{
    // not support
}

QJsonObject V2RayRustProfileGenerator::GenerateStreamSettings(const IOStreamSettings &stream)
{
    return stream;
}

void V2RayRustProfileGenerator::SwapTagProtocolMap(QMap<QString, QString> &inmap)
{
    std::swap(tagProtocolMap, inmap);
}

void V2RayRustProfileGenerator::InsertTagProtocolMap(const QString &tag, const QString &protocol)
{
    if (tag.isEmpty())
    {
        V2RayCorePluginClass::Log(u"Ignored outbound with empty tag."_qs);
    }
    else
    {
        tagProtocolMap.insert(tag, protocol);
    }
}
