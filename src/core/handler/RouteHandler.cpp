#include "RouteHandler.hpp"

#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
namespace Qv2ray::core::handler
{
    RouteHandler::RouteHandler(QObject *parent) : QObject(parent)
    {
        const auto routesJson = JsonFromString(StringFromFile(QV2RAY_CONFIG_DIR + "routes.json"));
        for (const auto &routeId : routesJson.keys())
        {
            configs.insert(GroupRoutingId{ routeId }, GroupRoutingConfig::fromJson(routesJson.value(routeId).toObject()));
        }
    }

    RouteHandler::~RouteHandler()
    {
        SaveRoutes();
    }

    void RouteHandler::SaveRoutes() const
    {
        QJsonObject routingObject;
        for (const auto &key : configs.keys())
        {
            routingObject[key.toString()] = configs[key].toJson();
        }
        StringToFile(JsonToString(routingObject), QV2RAY_CONFIG_DIR + "routes.json");
    }

    bool RouteHandler::SetDNSSettings(const GroupRoutingId &id, bool overrideGlobal, const QvConfig_DNS &dns)
    {
        configs[id].overrideDNS = overrideGlobal;
        configs[id].dnsConfig = dns;
        return true;
    }
    bool RouteHandler::SetAdvancedRouteSettings(const GroupRoutingId &id, bool overrideGlobal, const QvConfig_Route &route)
    {
        configs[id].overrideRoute = overrideGlobal;
        configs[id].routeConfig = route;
        return true;
    }

    // -------------------------- BEGIN CONFIG GENERATIONS
    ROUTING RouteHandler::GenerateRoutes(bool enableProxy, bool bypassCN, const QString &outTag, const QvConfig_Route &routeConfig) const
    {
        ROUTING root;
        root.insert("domainStrategy", routeConfig.domainStrategy);
        //
        // For Rules list
        ROUTERULELIST rulesList;

        // Private IPs should always NOT TO PROXY!
        rulesList.append(GenerateSingleRouteRule("geoip:private", false, OUTBOUND_TAG_DIRECT));
        //
        if (!enableProxy)
        {
            // This is added to disable all proxies, as a alternative influence of #64
            rulesList.append(GenerateSingleRouteRule("regexp:.*", true, OUTBOUND_TAG_DIRECT));
            rulesList.append(GenerateSingleRouteRule("0.0.0.0/0", false, OUTBOUND_TAG_DIRECT));
            rulesList.append(GenerateSingleRouteRule("::/0", false, OUTBOUND_TAG_DIRECT));
        }
        else
        {
            //
            // Blocked.
            if (!routeConfig.ips.block.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.ips.block, false, OUTBOUND_TAG_BLACKHOLE));
            }
            if (!routeConfig.domains.block.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.domains.block, true, OUTBOUND_TAG_BLACKHOLE));
            }
            //
            // Proxied
            if (!routeConfig.ips.proxy.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.ips.proxy, false, outTag));
            }
            if (!routeConfig.domains.proxy.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.domains.proxy, true, outTag));
            }
            //
            // Directed
            if (!routeConfig.ips.direct.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.ips.direct, false, OUTBOUND_TAG_DIRECT));
            }
            if (!routeConfig.domains.direct.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.domains.direct, true, OUTBOUND_TAG_DIRECT));
            }
            //
            // Check if CN needs proxy, or direct.
            if (bypassCN)
            {
                // No proxy agains CN addresses.
                rulesList.append(GenerateSingleRouteRule("geoip:cn", false, OUTBOUND_TAG_DIRECT));
                rulesList.append(GenerateSingleRouteRule("geosite:cn", true, OUTBOUND_TAG_DIRECT));
            }
        }

        root.insert("rules", rulesList);
        return root;
    }
    // -------------------------- END CONFIG GENERATIONS
    //
    // BEGIN RUNTIME CONFIG GENERATION
    // We need copy construct here
    CONFIGROOT RouteHandler::GenerateFinalConfig(const ConnectionGroupPair &p, bool api) const
    {
        return GenerateFinalConfig(ConnectionManager->GetConnectionRoot(p.connectionId), ConnectionManager->GetGroupRoutingId(p.groupId), api);
    }
    CONFIGROOT RouteHandler::GenerateFinalConfig(CONFIGROOT root, const GroupRoutingId &routingId, bool hasAPI) const
    {
        const auto &config = configs.contains(routingId) ? configs[routingId] : GlobalConfig.defaultRouteConfig;
        //
        const auto &connConf = config.overrideConnectionConfig ? config.connectionConfig : GlobalConfig.defaultRouteConfig.connectionConfig;
        const auto &dnsConf = config.overrideDNS ? config.dnsConfig : GlobalConfig.defaultRouteConfig.dnsConfig;
        const auto &routeConf = config.overrideRoute ? config.routeConfig : GlobalConfig.defaultRouteConfig.routeConfig;
        const auto &fpConf = config.overrideForwardProxyConfig ? config.forwardProxyConfig : GlobalConfig.defaultRouteConfig.forwardProxyConfig;
        //
        // See: https://github.com/Qv2ray/Qv2ray/issues/129
        // routeCountLabel in Mainwindow makes here failed to ENOUGH-ly
        // check the routing tables
        //
        // Check if is complex BEFORE adding anything.
        bool isComplex = IsComplexConfig(root);
        //
        //
        // logObject.insert("access", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ACCESS_LOG_FILENAME);
        // logObject.insert("error", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ERROR_LOG_FILENAME);
        QJsonIO::SetValue(root, V2RayLogLevel[GlobalConfig.logLevel], "log", "loglevel");
        //
        // Since Qv2ray does not support settings DNS manually for now.
        // These settings are being added for both complex config AND simple config.
        if (root.contains("dns") && !root.value("dns").toObject().isEmpty())
        {
            // We assume the users are using THEIR DNS settings.
            LOG(MODULE_CONNECTION, "Found DNS settings specified manually, skipping inserting GlobalConfig")
        }
        else
        {
            root.insert("dns", GenerateDNS(connConf.withLocalDNS, dnsConf));
        }
        //
        //
        // If inbounds list is empty we append our global configured inbounds to the config.
        // The setting applies to BOTH complex config AND simple config.
        // Just to ensure there's AT LEAST 1 possible inbound is being configured.
        if (!root.contains("inbounds") || root.value("inbounds").toArray().empty())
        {
#define INCONF GlobalConfig.inboundConfig
            INBOUNDS inboundsList;
            const QJsonObject sniffingOff{ { "enabled", false } };
            const QJsonObject sniffingOn{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };

            // HTTP Inbound
            if (GlobalConfig.inboundConfig.useHTTP)
            {
                const auto httpInBoundObject =                     //
                    GenerateInboundEntry(INCONF.listenip,          //
                                         INCONF.httpSettings.port, //
                                         "http",                   //
                                         INBOUNDSETTING{},         //
                                         "http_IN",                //
                                         { INCONF.httpSettings.sniffing ? sniffingOn : sniffingOff });
                if (INCONF.httpSettings.useAuth)
                {
                    QJsonIO::SetValue(httpInBoundObject, GenerateHTTPIN({ INCONF.httpSettings.account }), "settings");
                }

                inboundsList.append(httpInBoundObject);
            }

            // SOCKS Inbound
            if (INCONF.useSocks)
            {
                const auto socksInBoundObject =                                                                //
                    GenerateInboundEntry(INCONF.listenip,                                                      //
                                         INCONF.socksSettings.port,                                            //
                                         "socks",                                                              //
                                         GenerateSocksIN(INCONF.socksSettings.useAuth ? "password" : "noauth", //
                                                         { INCONF.socksSettings.account },                     //
                                                         INCONF.socksSettings.enableUDP,                       //
                                                         INCONF.socksSettings.localIP),                        //
                                         "socks_IN",                                                           //
                                         { INCONF.socksSettings.sniffing ? sniffingOn : sniffingOff });
                inboundsList.append(socksInBoundObject);
            }

            // TPROXY
            if (INCONF.useTPROXY)
            {
                QList<QString> networks;
                if (INCONF.tProxySettings.hasTCP)
                    networks << "tcp";
                if (INCONF.tProxySettings.hasUDP)
                    networks << "udp";
                const auto tproxy_network = networks.join(",");
                // tProxy IPv4 Settings
                {
                    LOG(MODULE_CONNECTION, "Processing tProxy IPv4 inbound")
                    INBOUND tProxyIn = GenerateInboundEntry(INCONF.tProxySettings.tProxyIP,                        //
                                                            INCONF.tProxySettings.port,                            //
                                                            "dokodemo-door",                                       //
                                                            GenerateDokodemoIN("", 0, tproxy_network, 0, true, 0), //
                                                            "tproxy_IN",                                           //
                                                            {
                                                                { "enabled", true },                            //
                                                                { "destOverride", QJsonArray{ "http", "tls" } } //
                                                            });
                    tProxyIn.insert("streamSettings", QJsonObject{ { "sockopt", QJsonObject{ { "tproxy", INCONF.tProxySettings.mode } } } });
                    inboundsList.append(tProxyIn);
                }
                if (!INCONF.tProxySettings.tProxyV6IP.isEmpty())
                {
                    LOG(MODULE_CONNECTION, "Processing tProxy IPv6 inbound")
                    INBOUND tProxyIn = GenerateInboundEntry(INCONF.tProxySettings.tProxyV6IP,                      //
                                                            INCONF.tProxySettings.port,                            //
                                                            "dokodemo-door",                                       //
                                                            GenerateDokodemoIN("", 0, tproxy_network, 0, true, 0), //
                                                            "tproxy_IN_V6",                                        //
                                                            {
                                                                { "enabled", true },                            //
                                                                { "destOverride", QJsonArray{ "http", "tls" } } //
                                                            });
                    tProxyIn.insert("streamSettings", QJsonObject{ { "sockopt", QJsonObject{ { "tproxy", INCONF.tProxySettings.mode } } } });
                    inboundsList.append(tProxyIn);
                }
            }

            root["inbounds"] = inboundsList;
            DEBUG(MODULE_CONNECTION, "Added global config inbounds to the config")
        }
#undef INCONF

        // Process every inbounds to make sure a tag is configured, fixed
        // API 0 speed issue when no tag is configured.
        FillupTagsFilter(root, "inbounds");
        //
        //
        // Note: The part below always makes the whole functionality in
        // trouble...... BE EXTREME CAREFUL when changing these code
        // below...
        if (isComplex)
        {
            // For some config files that has routing entries already.
            // We DO NOT add extra routings.
            //
            // HOWEVER, we need to verify the QV2RAY_RULE_ENABLED entry.
            // And what's more, process (by removing unused items) from a
            // rule object.
            ROUTING routing(root["routing"].toObject());
            ROUTERULELIST rules;
            LOG(MODULE_CONNECTION, "Processing an existing routing table.")

            for (const auto &_rule : routing["rules"].toArray())
            {
                auto _b = _rule.toObject();

                if (_b.contains("QV2RAY_RULE_USE_BALANCER"))
                {
                    // We use balancer, or the normal outbound
                    _b.remove(_b["QV2RAY_RULE_USE_BALANCER"].toBool(false) ? "outboundTag" : "balancerTag");
                }
                else
                {
                    LOG(MODULE_SETTINGS, "We found a rule without QV2RAY_RULE_USE_BALANCER, so didn't process it.")
                }

                // If this entry has been disabled.
                if (_b.contains("QV2RAY_RULE_ENABLED") && _b["QV2RAY_RULE_ENABLED"].toBool() == false)
                {
                    LOG(MODULE_SETTINGS, "Discarded a rule as it's been set DISABLED")
                }
                else
                {
                    rules.append(_b);
                }
            }

            routing["rules"] = rules;
            root["routing"] = routing;
        }
        else
        {
            LOG(MODULE_CONNECTION, "Inserting default values to simple config")
            if (root["outbounds"].toArray().count() != 1)
            {
                // There are no ROUTING but 2 or more outbounds.... This is rare, but possible.
                LOG(MODULE_CONNECTION, "WARN: This message usually indicates the config file has logic errors:")
                LOG(MODULE_CONNECTION, "WARN: --> The config file has NO routing section, however more than 1 outbounds are detected.")
            }
            //
            auto tag = getTag(OUTBOUND(QJsonIO::GetValue(root, "outbounds", 0).toObject()));
            if (tag.isEmpty())
            {
                LOG(MODULE_CONNECTION, "Applying workaround when an outbound tag is empty")
                tag = GenerateRandomString(15);
                QJsonIO::SetValue(root, tag, "outbounds", 0, "tag");
            }
            root["routing"] = GenerateRoutes(connConf.enableProxy, connConf.bypassCN, tag, routeConf);
            //
            // Process forward proxy
            //
            if (fpConf.enableForwardProxy)
            {
                auto outboundArray = root["outbounds"].toArray();
                auto firstOutbound = outboundArray.first().toObject();
                if (firstOutbound[QV2RAY_USE_FPROXY_KEY].toBool(false))
                {
                    LOG(MODULE_CONNECTION, "Applying forward proxy to current connection.")
                    PROXYSETTING proxy;
                    proxy["tag"] = OUTBOUND_TAG_FORWARD_PROXY;
                    firstOutbound["proxySettings"] = proxy;

                    // FP Outbound.
                    if (fpConf.type.toLower() == "http" || fpConf.type.toLower() == "socks")
                    {
                        auto fpOutbound =
                            GenerateHTTPSOCKSOut(fpConf.serverAddress, fpConf.port, fpConf.useAuth, fpConf.username, fpConf.password);
                        outboundArray.push_back(
                            GenerateOutboundEntry(fpConf.type.toLower(), fpOutbound, {}, {}, "0.0.0.0", OUTBOUND_TAG_FORWARD_PROXY));
                    }
                    else if (!fpConf.type.isEmpty())
                    {
                        DEBUG(MODULE_CONNECTION, "WARNING: Unsupported outbound type: " + fpConf.type)
                    }
                    else
                    {
                        DEBUG(MODULE_CONNECTION, "WARNING: Empty outbound type.")
                    }
                }
                else
                {
                    // Remove proxySettings from firstOutbound
                    firstOutbound.remove("proxySettings");
                }

                outboundArray.replace(0, firstOutbound);
                root["outbounds"] = outboundArray;
            }
#undef fpConf
            //
            // Process FREEDOM and BLACKHOLE outbound
            {
                OUTBOUNDS outbounds(root["outbounds"].toArray());
                const auto freeDS = (connConf.v2rayFreedomDNS) ? "UseIP" : "AsIs";
                outbounds.append(GenerateOutboundEntry("freedom", GenerateFreedomOUT(freeDS, ":0", 0), {}, {}, "0.0.0.0", OUTBOUND_TAG_DIRECT));
                outbounds.append(GenerateOutboundEntry("blackhole", GenerateBlackHoleOUT(false), {}, {}, "0.0.0.0", OUTBOUND_TAG_BLACKHOLE));
                root["outbounds"] = outbounds;
            }
            //
            // Connection Filters
            if (GlobalConfig.defaultRouteConfig.connectionConfig.dnsIntercept)
            {
                const auto hasTProxy = GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.inboundConfig.tProxySettings.hasUDP;
                const auto hasIPv6 = hasTProxy && (!GlobalConfig.inboundConfig.tProxySettings.tProxyV6IP.isEmpty());
                const bool hasSocksUDP = GlobalConfig.inboundConfig.useSocks && GlobalConfig.inboundConfig.socksSettings.enableUDP;
                DNSInterceptFilter(root, hasTProxy, hasIPv6, hasSocksUDP);
            }

            if (GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.outboundConfig.mark > 0)
            {
                OutboundMarkSettingFilter(root, GlobalConfig.outboundConfig.mark);
            }

            if (connConf.bypassBT)
            {
                BypassBTFilter(root);
            }
            // Process mKCP seed.
            mKCPSeedFilter(root);

            // Remove empty Mux object from settings.
            RemoveEmptyMuxFilter(root);
        }

        // Let's process some api features.
        if (hasAPI && GlobalConfig.kernelConfig.enableAPI)
        {
            //
            // Stats
            root.insert("stats", QJsonObject());
            //
            // Routes
            QJsonObject routing = root["routing"].toObject();
            QJsonArray routingRules = routing["rules"].toArray();
            QJsonObject APIRouteRoot{ { "type", "field" },                //
                                      { "outboundTag", API_TAG_DEFAULT }, //
                                      { "inboundTag", QJsonArray{ API_TAG_INBOUND } } };
            routingRules.push_front(APIRouteRoot);
            routing["rules"] = routingRules;
            root["routing"] = routing;
            //
            // Policy
            QJsonIO::SetValue(root, true, "policy", "system", "statsInboundUplink");
            QJsonIO::SetValue(root, true, "policy", "system", "statsInboundDownlink");
            QJsonIO::SetValue(root, true, "policy", "system", "statsOutboundUplink");
            QJsonIO::SetValue(root, true, "policy", "system", "statsOutboundDownlink");
            //
            // Inbounds
            INBOUNDS inbounds(root["inbounds"].toArray());
            QJsonObject fakeDocodemoDoor{ { "address", "127.0.0.1" } };
            const auto apiInboundsRoot = GenerateInboundEntry("127.0.0.1", GlobalConfig.kernelConfig.statsPort, "dokodemo-door",
                                                              INBOUNDSETTING(fakeDocodemoDoor), API_TAG_INBOUND);
            inbounds.push_front(apiInboundsRoot);
            root["inbounds"] = inbounds;
            //
            // API
            root["api"] = GenerateAPIEntry(API_TAG_DEFAULT);
        }

        return root;
    }
} // namespace Qv2ray::core::handler
