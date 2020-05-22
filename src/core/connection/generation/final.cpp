#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"

namespace Qv2ray::core::connection::generation::final
{

    // -------------------------- END CONFIG GENERATIONS
    //
    // BEGIN RUNTIME CONFIG GENERATION
    // We need copy construct here
    CONFIGROOT GenerateFinalConfig(CONFIGROOT root)
    {
        // See: https://github.com/Qv2ray/Qv2ray/issues/129
        // routeCountLabel in Mainwindow makes here failed to ENOUGH-ly
        // check the routing tables
        //
        // Check if is complex BEFORE adding anything.
        bool isComplex = IsComplexConfig(root);
        //
        //
        QJsonObject logObject;
        // logObject.insert("access", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ACCESS_LOG_FILENAME);
        // logObject.insert("error", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ERROR_LOG_FILENAME);
        logObject.insert("loglevel", V2rayLogLevel[GlobalConfig.logLevel]);
        root.insert("log", logObject);
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
            auto dnsList = GlobalConfig.connectionConfig.dnsList;
            auto dnsObject = GenerateDNS(GlobalConfig.connectionConfig.withLocalDNS, dnsList);
            root.insert("dns", dnsObject);
        }
        //
        //
        // If inbounds list is empty we append our global configured
        // inbounds to the config.
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
                INBOUND httpInBoundObject =                        //
                    GenerateInboundEntry(INCONF.listenip,          //
                                         INCONF.httpSettings.port, //
                                         "http",                   //
                                         INBOUNDSETTING{},         //
                                         "http_IN",                //
                                         { INCONF.httpSettings.sniffing ? sniffingOn : sniffingOff });
                if (INCONF.httpSettings.useAuth)
                {
                    httpInBoundObject.insert("settings", GenerateHTTPIN({ INCONF.httpSettings.account }));
                }

                inboundsList.append(httpInBoundObject);
            }

            // SOCKS Inbound
            if (INCONF.useSocks)
            {
                INBOUND socksInBoundObject =                                                                   //
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
                //
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
        INBOUNDS newTaggedInbounds(root["inbounds"].toArray());

        for (auto i = 0; i < newTaggedInbounds.count(); i++)
        {
            auto _inboundItem = newTaggedInbounds[i].toObject();
            if (!_inboundItem.contains("tag") || _inboundItem["tag"].toString().isEmpty())
            {
                LOG(MODULE_SETTINGS, "Adding a tag to an inbound.")
                _inboundItem["tag"] = GenerateRandomString(8);
                newTaggedInbounds[i] = _inboundItem;
            }
        }

        root["inbounds"] = newTaggedInbounds;
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

            for (auto _rule : routing["rules"].toArray())
            {
                auto _b = _rule.toObject();

                if (_b.contains("QV2RAY_RULE_USE_BALANCER"))
                {
                    if (_b["QV2RAY_RULE_USE_BALANCER"].toBool(false))
                    {
                        // We use balancer
                        _b.remove("outboundTag");
                    }
                    else
                    {
                        // We only use the normal outbound
                        _b.remove("balancerTag");
                    }
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
            auto tag = getTag(OUTBOUND(root["outbounds"].toArray().first().toObject()));
            auto routeObject = GenerateRoutes(GlobalConfig.connectionConfig.enableProxy, GlobalConfig.connectionConfig.bypassCN, tag);
            root.insert("routing", routeObject);
            //
            // Process forward proxy
#define fpConf GlobalConfig.connectionConfig.forwardProxyConfig

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
                    else
                    {
                        if (!fpConf.type.isEmpty())
                        {
                            DEBUG(MODULE_CONNECTION, "WARNING: Unsupported outbound type: " + fpConf.type)
                        }
                        else
                        {
                            DEBUG(MODULE_CONNECTION, "WARNING: Empty outbound type.")
                        }
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
            OUTBOUNDS outbounds(root["outbounds"].toArray());
            //
            const auto freeDS = (GlobalConfig.connectionConfig.v2rayFreedomDNS) ? "UseIP" : "AsIs";
            //
            outbounds.append(GenerateOutboundEntry("freedom", GenerateFreedomOUT(freeDS, ":0", 0), {}, {}, "0.0.0.0", OUTBOUND_TAG_DIRECT));
            outbounds.append(GenerateOutboundEntry("blackhole", GenerateBlackHoleOUT(false), {}, {}, "0.0.0.0", OUTBOUND_TAG_BLACKHOLE));
            //
            root["outbounds"] = outbounds;

            // intercepet dns if necessary
            if (GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.inboundConfig.tProxySettings.dnsIntercept)
            {
                DNSInterceptFilter(root);
            }

            // mark outbound if necessary
            if (GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.outboundConfig.mark > 0)
            {
                OutboundMarkSettingFilter(GlobalConfig.outboundConfig.mark, root);
            }

            if (GlobalConfig.connectionConfig.bypassBT)
            {
                bypassBTFilter(root);
            }
        }

        // Let's process some api features.
        {
            //
            // Stats
            //
            root.insert("stats", QJsonObject());
            //
            // Routes
            //
            QJsonObject routing = root["routing"].toObject();
            QJsonArray routingRules = routing["rules"].toArray();
            QJsonObject APIRouteRoot;
            APIRouteRoot["type"] = "field";
            APIRouteRoot["outboundTag"] = API_TAG_DEFAULT;
            QJsonArray inboundTag;
            inboundTag.append(API_TAG_INBOUND);
            APIRouteRoot["inboundTag"] = inboundTag;
            // Add this to root.
            routingRules.push_front(APIRouteRoot);
            routing["rules"] = routingRules;
            root["routing"] = routing;
            //
            // Policy
            //
            QJsonObject policyRoot = root.contains("policy") ? root["policy"].toObject() : QJsonObject();
            QJsonObject systemPolicy = policyRoot.contains("system") ? policyRoot["system"].toObject() : QJsonObject();
            systemPolicy["statsInboundUplink"] = true;
            systemPolicy["statsInboundDownlink"] = true;
            policyRoot["system"] = systemPolicy;
            // Add this to root.
            root["policy"] = policyRoot;
            //
            // Inbounds
            //
            INBOUNDS inbounds(root["inbounds"].toArray());
            INBOUNDSETTING fakeDocodemoDoor;
            fakeDocodemoDoor["address"] = "127.0.0.1";
            QJsonObject apiInboundsRoot =
                GenerateInboundEntry("127.0.0.1", GlobalConfig.kernelConfig.statsPort, "dokodemo-door", fakeDocodemoDoor, API_TAG_INBOUND);
            inbounds.push_front(apiInboundsRoot);
            root["inbounds"] = inbounds;
            //
            // API
            //
            root["api"] = GenerateAPIEntry(API_TAG_DEFAULT);
        }

        return root;
    }

} // namespace Qv2ray::core::connection::generation::final
