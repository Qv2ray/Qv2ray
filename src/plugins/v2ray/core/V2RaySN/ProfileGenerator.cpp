#include "ProfileGenerator.hpp"

#include "QvPlugin/Utils/QJsonIO.hpp"
#include "V2RayCorePluginTemplate.hpp"
#include "V2RayModels.hpp"

#include <QJsonDocument>

constexpr auto DEFAULT_API_TAG = "qv2ray-api";
constexpr auto DEFAULT_API_IN_TAG = "qv2ray-api-in";

inline void OutboundMarkSettingFilter(QJsonObject &root, int mark)
{
    for (auto i = 0; i < root[u"outbounds"_qs].toArray().count(); i++)
        QJsonIO::SetValue(root, mark, u"outbounds"_qs, i, u"streamSettings"_qs, u"sockopt"_qs, u"mark"_qs);
}

V2RaySNProfileGenerator::V2RaySNProfileGenerator(const ProfileContent &profile) : profile(profile){};

QByteArray V2RaySNProfileGenerator::GenerateConfiguration(const ProfileContent &p)
{
    return V2RaySNProfileGenerator(p).Generate();
}

QByteArray V2RaySNProfileGenerator::Generate()
{
    QJsonObject rootconf;
    JsonStructHelper::MergeJson(rootconf, profile.extraOptions);

    for (const auto &in : profile.inbounds)
        ProcessInboundConfig(in);

    for (const auto &out : profile.outbounds)
        if (out.objectType == OutboundObject::ORIGINAL)
            ProcessOutboundConfig(out);
        else if (out.objectType == OutboundObject::BALANCER)
            ProcessBalancerConfig(out);

    for (const auto &rule : profile.routing.rules)
        ProcessRoutingRule(rule);

    QJsonObject routing;
    if (const auto ds = profile.routing.extraOptions[u"domainStrategy"_qs].toString(); !ds.isEmpty())
        routing[u"domainStrategy"_qs] = ds;

    if (const auto dm = profile.routing.extraOptions[u"domainMatcher"_qs].toString(); !dm.isEmpty())
        routing[u"domainMatcher"_qs] = dm;

    // Override log level
    const auto settings = V2RayCorePluginClass::PluginInstance->settings;
    rootconf[u"log"_qs] = QJsonObject{ { u"loglevel"_qs, [&]()
                                         {
                                             switch (settings.LogLevel)
                                             {
                                                 case V2RayCorePluginSettings::None: return u"none"_qs;
                                                 case V2RayCorePluginSettings::Error: return u"error"_qs;
                                                 case V2RayCorePluginSettings::Warning: return u"warning"_qs;
                                                 case V2RayCorePluginSettings::Info: return u"info"_qs;
                                                 case V2RayCorePluginSettings::Debug: return u"debug"_qs;
                                                 default: return u"warning"_qs;
                                             }
                                         }() } };

    // Browser Forwarder
    {
        if (!rootconf.contains(u"browserForwarder"_qs) || rootconf.value(u"browserForwarder"_qs).toObject().isEmpty())
            if (!settings.BrowserForwarderSettings.listenAddr->isEmpty())
                rootconf[u"browserForwarder"_qs] = settings.BrowserForwarderSettings.toJson();
    }

    // Observatory
    {
        if (!rootconf.contains(u"observatory"_qs) || rootconf.value(u"observatory"_qs).toObject().isEmpty())
            rootconf[u"observatory"_qs] = settings.ObservatorySettings.toJson();

        if (rootconf[u"observatory"_qs][u"subjectSelector"_qs].toArray().isEmpty())
            rootconf.remove(u"observatory"_qs);
    }

    if (settings.APIEnabled)
    {
        //
        // Stats
        rootconf.insert(u"stats"_qs, QJsonObject());

        //
        // Policy
        rootconf[u"policy"_qs] = QJsonObject{
            { u"system"_qs,
              QJsonObject{
                  { u"statsInboundUplink"_qs, true },
                  { u"statsInboundDownlink"_qs, true },
                  { u"statsOutboundUplink"_qs, true },
                  { u"statsOutboundDownlink"_qs, true },
              } },
        };

        //
        // Inbound
        inbounds.push_front(QJsonObject{
            { u"tag"_qs, QString::fromUtf8(DEFAULT_API_IN_TAG) },
            { u"listen"_qs, u"127.0.0.1"_qs },
            { u"port"_qs, *settings.APIPort },
            { u"protocol"_qs, u"dokodemo-door"_qs },
            { u"settings"_qs, QJsonObject{ { u"address"_qs, u"127.0.0.1"_qs } } },
        });
        //
        // Rule
        rules.push_front(QJsonObject{
            { u"type"_qs, u"field"_qs },
            { u"outboundTag"_qs, QString::fromUtf8(DEFAULT_API_TAG) },
            { u"inboundTag"_qs, QJsonArray{ QString::fromUtf8(DEFAULT_API_IN_TAG) } },
        });

        //
        // API
        rootconf[u"api"_qs] = QJsonObject{
            { u"tag"_qs, QString::fromUtf8(DEFAULT_API_TAG) },
            { u"services"_qs,
              QJsonArray{
                  u"ReflectionService"_qs,
                  u"HandlerService"_qs,
                  u"LoggerService"_qs,
                  u"StatsService"_qs,
              } },
        };
    }

    if (!rules.isEmpty())
        routing[u"rules"_qs] = rules;

    if (!balancers.isEmpty())
        routing[u"balancers"_qs] = balancers;

    rootconf[u"routing"_qs] = routing;
    rootconf[u"inbounds"_qs] = inbounds;
    rootconf[u"outbounds"_qs] = outbounds;

    if (!profile.routing.dns.isEmpty())
        rootconf[u"dns"_qs] = profile.routing.dns;

    if (!profile.routing.fakedns[u"pools"_qs].toArray().isEmpty())
        rootconf[u"fakedns"_qs] = profile.routing.fakedns[u"pools"_qs];

    OutboundMarkSettingFilter(rootconf, settings.OutboundMark);

    return QJsonDocument(rootconf).toJson(QJsonDocument::Indented);
}

void V2RaySNProfileGenerator::ProcessRoutingRule(const RuleObject &r)
{
    QJsonObject rule;
    rule[u"type"_qs] = u"field"_qs;
    if (!r.targetDomains.isEmpty())
        rule[u"domains"_qs] = QJsonArray::fromStringList(r.targetDomains);

    if (!r.targetIPs.isEmpty())
        rule[u"ip"_qs] = QJsonArray::fromStringList(r.targetIPs);

    if (r.targetPort.from != 0 && r.targetPort.to != 0)
        rule[u"port"_qs] = (QString) r.targetPort;

    if (r.sourcePort.from != 0 && r.sourcePort.to != 0)
        rule[u"sourcePort"_qs] = (QString) r.sourcePort;

    if (!r.networks.isEmpty())
        rule[u"network"_qs] = r.networks.join(u',');

    if (!r.sourceAddresses.isEmpty())
        rule[u"source"_qs] = QJsonArray::fromStringList(r.sourceAddresses);

    if (!r.inboundTags.isEmpty())
        rule[u"inboundTag"_qs] = QJsonArray::fromStringList(r.inboundTags);

    if (!r.protocols.isEmpty())
        rule[u"protocol"_qs] = QJsonArray::fromStringList(r.protocols);

    rule[u"user"_qs] = r.extraSettings[u"user"_qs];

    const auto out = findOutbound(r.outboundTag);
    rule[out.objectType == OutboundObject::ORIGINAL ? u"outboundTag"_qs : u"balancerTag"_qs] = r.outboundTag;

    rules << rule;
}

void V2RaySNProfileGenerator::ProcessInboundConfig(const InboundObject &in)
{
    QJsonObject root;
    root[u"tag"_qs] = in.name;
    root[u"listen"_qs] = in.inboundSettings.address;
    root[u"port"_qs] = in.inboundSettings.port.from;
    root[u"streamSettings"_qs] = GenerateStreamSettings(in.inboundSettings.streamSettings);
    root[u"protocol"_qs] = in.inboundSettings.protocol;
    root[u"settings"_qs] = in.inboundSettings.protocolSettings;

    if (in.inboundSettings.muxSettings.enabled)
        root[u"mux"_qs] = in.inboundSettings.muxSettings.toJson();

    // Special Case: HTTP, SOCKS: with flattened users[]
    if (in.inboundSettings.protocol == u"http"_qs || in.inboundSettings.protocol == u"socks"_qs)
    {
        QJsonObject settings = root[u"settings"_qs].toObject();
        settings.remove(u"user"_qs);
        settings.remove(u"pass"_qs);
        root[u"settings"_qs] = settings;
    }

    if (in.inboundSettings.protocol == u"dokodemo-door"_qs)
    {
        QJsonObject settings = root[u"settings"_qs].toObject();
        settings.insert(u"allowTransparent"_qs, true);
        root[u"settings"_qs] = settings;
    }

    JsonStructHelper::MergeJson(root, in.options);
    inbounds << root;
}

void V2RaySNProfileGenerator::ProcessOutboundConfig(const OutboundObject &out)
{
    assert(out.objectType == OutboundObject::ORIGINAL);

    QJsonObject root;
    root[u"tag"_qs] = out.name;
    root[u"protocol"_qs] = out.outboundSettings.protocol;
    root[u"settings"_qs] = out.outboundSettings.protocolSettings;
    root[u"streamSettings"_qs] = GenerateStreamSettings(out.outboundSettings.streamSettings);

    if (out.outboundSettings.muxSettings.enabled)
        root[u"mux"_qs] = out.outboundSettings.muxSettings.toJson();

    if (out.outboundSettings.protocol == u"trojan"_qs)
    {
        Qv2ray::Models::TrojanClientObject trojan;
        trojan.loadJson(out.outboundSettings.protocolSettings);

        QJsonObject singleServer{
            { u"password"_qs, *trojan.password },
            { u"address"_qs, out.outboundSettings.address },
            { u"port"_qs, out.outboundSettings.port.from },
        };

        root[u"settings"_qs] = QJsonObject{ { u"servers"_qs, QJsonArray{ singleServer } } };
    }

    if (out.outboundSettings.protocol == u"http"_qs || out.outboundSettings.protocol == u"socks"_qs)
    {
        Qv2ray::Models::HTTPSOCKSObject serv;
        serv.loadJson(out.outboundSettings.protocolSettings);

        QJsonObject singleServer{
            { u"address"_qs, out.outboundSettings.address },
            { u"port"_qs, out.outboundSettings.port.from },
        };

        if (!serv.user->isEmpty() || !serv.pass->isEmpty())
        {
            QJsonObject userobject{ { u"user"_qs, *serv.user }, { u"pass"_qs, *serv.pass } };
            singleServer[u"users"_qs] = QJsonArray{ userobject };
        }

        root[u"settings"_qs] = QJsonObject{ { u"servers"_qs, QJsonArray{ singleServer } } };
    }

    if (out.outboundSettings.protocol == u"vmess"_qs)
    {
        Qv2ray::Models::VMessClientObject serv;
        serv.loadJson(out.outboundSettings.protocolSettings);

        QJsonObject singleServer{
            { u"address"_qs, out.outboundSettings.address },
            { u"port"_qs, out.outboundSettings.port.from },
            { u"users"_qs, QJsonArray{ QJsonObject{
                               { u"id"_qs, *serv.id },
                               { u"security"_qs, *serv.security },
                               { u"experiments"_qs, *serv.experiments },
                           } } },
        };

        root[u"settings"_qs] = QJsonObject{ { u"vnext"_qs, QJsonArray{ singleServer } } };
    }

    if (out.outboundSettings.protocol == u"shadowsocks"_qs)
    {
        Qv2ray::Models::ShadowSocksClientObject ss;
        ss.loadJson(out.outboundSettings.protocolSettings);

        QJsonObject singleServer{
            { u"method"_qs, *ss.method },
            { u"password"_qs, *ss.password },
            { u"address"_qs, out.outboundSettings.address },
            { u"port"_qs, out.outboundSettings.port.from },
        };

        root[u"settings"_qs] = QJsonObject{ { u"servers"_qs, QJsonArray{ singleServer } } };
    }

    JsonStructHelper::MergeJson(root, out.options);
    outbounds << root;
}

void V2RaySNProfileGenerator::ProcessBalancerConfig(const OutboundObject &out)
{
    assert(out.objectType == OutboundObject::BALANCER);
    QJsonObject root;
    root[u"tag"_qs] = out.name;
    root[u"selector"_qs] = out.balancerSettings.selectorSettings;
    root[u"strategy"_qs] = QJsonObject{ { u"type"_qs, out.balancerSettings.selectorType } };
    balancers << root;
}

QJsonObject V2RaySNProfileGenerator::GenerateStreamSettings(const IOStreamSettings &stream)
{
    return stream;
}
