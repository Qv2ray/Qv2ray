#include "RouteHandler.hpp"

#include "base/models/QvComplexConfigModels.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "RouteHandler"

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

    bool RouteHandler::ExpandChainedOutbounds(CONFIGROOT &root) const
    {
        // Proxy Chain Expansion
        const auto outbounds = root["outbounds"].toArray();
        const auto inbounds = root["inbounds"].toArray();
        const auto rules = root["routing"].toObject()["rules"].toArray();

        QJsonArray newOutbounds, newInbounds, newRules;

        QMap<QString, QJsonObject> outboundCache;
        // First pass - Resolve Indexes (tags), build cache
        for (const auto &singleOutboundVal : outbounds)
        {
            const auto outbound = singleOutboundVal.toObject();
            const auto meta = OutboundObjectMeta::loadFromOutbound(OUTBOUND(outbound));
            if (meta.metaType != METAOUTBOUND_CHAIN)
                outboundCache[outbound["tag"].toString()] = outbound;
        }

        // Second pass - Build Chains
        for (const auto &singleOutboundVal : outbounds)
        {
            const auto outbound = singleOutboundVal.toObject();
            const auto meta = OutboundObjectMeta::loadFromOutbound(OUTBOUND(outbound));
            if (meta.metaType != METAOUTBOUND_CHAIN)
            {
                newOutbounds << outbound;
                continue;
            }

            if (meta.outboundTags.isEmpty())
            {
                LOG("Trying to expand an empty chain.");
                continue;
            }

            int nextInboundPort = meta.chainPortAllocation;
            const auto firstOutboundTag = meta.getDisplayName();
            const auto lastOutboundTag = meta.outboundTags.first();

            OutboundInfoObject lastOutbound;

            const auto outbountTagCount = meta.outboundTags.count();

            for (auto i = outbountTagCount - 1; i >= 0; i--)
            {
                const auto chainOutboundTag = meta.outboundTags[i];

                const auto isFirstOutbound = i == outbountTagCount - 1;
                const auto isLastOutbound = i == 0;
                const auto newOutboundTag = [&]() {
                    if (isFirstOutbound)
                        return firstOutboundTag;
                    else if (isLastOutbound)
                        return lastOutboundTag;
                    else
                        return (firstOutboundTag + "_" + chainOutboundTag + "_" + QSTRN(nextInboundPort));
                }();

                if (!outboundCache.contains(chainOutboundTag))
                {
                    LOG("Cannot build outbound chain: Missing tag: " + firstOutboundTag);
                    return false;
                }
                auto newOutbound = outboundCache[chainOutboundTag];

                // Create Inbound
                if (!isFirstOutbound)
                {
                    const auto inboundTag = firstOutboundTag + ":" + QSTRN(nextInboundPort) + "->" + newOutboundTag;
                    const auto inboundSettings = GenerateDokodemoIN(lastOutbound[INFO_SERVER].toString(), lastOutbound[INFO_PORT].toInt(), "tcp,udp");
                    const auto newInbound = GenerateInboundEntry(inboundTag, "dokodemo-door", "127.0.0.1", nextInboundPort, inboundSettings);
                    nextInboundPort++;
                    newInbounds << newInbound;
                    //
                    QJsonObject ruleObject;
                    ruleObject["type"] = "field";
                    ruleObject["inboundTag"] = QJsonArray{ inboundTag };
                    ruleObject["outboundTag"] = newOutboundTag;
                    newRules.prepend(ruleObject);
                }

                if (!isLastOutbound)
                {
                    // Begin process outbound.
                    const auto outboundProtocol = newOutbound["protocol"].toString();
                    auto outboundSettings = newOutbound["settings"].toObject();
                    // Get Outbound Info for next Inbound
                    bool getOutboundInfoStatus = false;
                    lastOutbound = PluginHost->GetOutboundInfo(outboundProtocol, outboundSettings, getOutboundInfoStatus);
                    if (!getOutboundInfoStatus)
                    {
                        LOG("Cannot get outbound info for: " + chainOutboundTag);
                        return false;
                    }

                    // Update allocated port as outbound server/port
                    OutboundInfoObject newOutboundInfo;
                    newOutboundInfo[INFO_SERVER] = "127.0.0.1";
                    newOutboundInfo[INFO_PORT] = nextInboundPort;
                    // For those kernels deducing SNI from the server name.
                    if (!lastOutbound.contains(INFO_SNI) || lastOutbound[INFO_SNI].toString().trimmed().isEmpty())
                        newOutboundInfo[INFO_SNI] = lastOutbound[INFO_SERVER];
                    //
                    PluginHost->SetOutboundInfo(outboundProtocol, newOutboundInfo, outboundSettings);
                    newOutbound.insert("settings", outboundSettings);

                    // Create new outbound
                    newOutbound.insert("tag", newOutboundTag);
                }
                newOutbounds << newOutbound;
            }
        }

        //
        // Finalize
        {
            QJsonArray _newInbounds = inbounds, _newRules = rules;
            for (const auto &in : newInbounds)
                _newInbounds << in;
            for (const auto &rule : newRules)
                _newRules.prepend(rule);

            root["outbounds"] = newOutbounds;
            root["inbounds"] = _newInbounds;
            QJsonIO::SetValue(root, _newRules, "routing", "rules");
        }
        return true;
    }

    OUTBOUNDS RouteHandler::ExpandExternalConnection(const OUTBOUNDS &outbounds) const
    {
        OUTBOUNDS result;
        for (const auto &out : outbounds)
        {
            auto outObject = out.toObject();
            const auto meta = OutboundObjectMeta::loadFromOutbound(OUTBOUND(outObject));
            if (meta.metaType == METAOUTBOUND_EXTERNAL)
            {
                outObject = ConnectionManager->GetConnectionRoot(meta.connectionId)["outbounds"].toArray().first().toObject();
                outObject["tag"] = meta.getDisplayName();
            }
            result << outObject;
        }
        return result;
    }

    CONFIGROOT RouteHandler::GenerateFinalConfig(const ConnectionGroupPair &p, bool api) const
    {
        return GenerateFinalConfig(ConnectionManager->GetConnectionRoot(p.connectionId), ConnectionManager->GetGroupRoutingId(p.groupId), api);
    }

    //
    // BEGIN RUNTIME CONFIG GENERATION
    // We need copy construct here
    CONFIGROOT RouteHandler::GenerateFinalConfig(CONFIGROOT root, const GroupRoutingId &routingId, bool hasAPI) const
    {
        const auto &config = configs.contains(routingId) ? configs[routingId] : GlobalConfig.defaultRouteConfig;
        //
        const auto &connConf = config.overrideConnectionConfig ? config.connectionConfig : GlobalConfig.defaultRouteConfig.connectionConfig;
        const auto &dnsConf = config.overrideDNS ? config.dnsConfig : GlobalConfig.defaultRouteConfig.dnsConfig;
        const auto &routeConf = config.overrideRoute ? config.routeConfig : GlobalConfig.defaultRouteConfig.routeConfig;
        const auto &fpConf = config.overrideForwardProxyConfig ? config.forwardProxyConfig : GlobalConfig.defaultRouteConfig.forwardProxyConfig;
        //
        //
        // Note: The part below always makes the whole functionality in
        // trouble...... BE EXTREME CAREFUL when changing these code
        // below...
        //
        // Check if is complex BEFORE adding anything.
        bool isComplex = IsComplexConfig(root);

        if (isComplex)
        {
            // For some config files that has routing entries already.
            // We DO NOT add extra routings.
            //
            // HOWEVER, we need to verify the QV2RAY_RULE_ENABLED entry.
            // And what's more, process (by removing unused items) from a
            // rule object.
            ROUTING routing(root["routing"].toObject());
            QJsonArray newRules;
            LOG("Processing an existing routing table.");

            for (const auto &_rule : routing["rules"].toArray())
            {
                auto rule = _rule.toObject();

                // For backward compatibility
                if (rule.contains("QV2RAY_RULE_USE_BALANCER"))
                {
                    // We use balancer, or the normal outbound
                    rule.remove(rule["QV2RAY_RULE_USE_BALANCER"].toBool(false) ? "outboundTag" : "balancerTag");
                }
                else
                {
                    LOG("We found a rule without QV2RAY_RULE_USE_BALANCER, so didn't process it.");
                }

                // If this entry has been disabled.
                if (rule.contains("QV2RAY_RULE_ENABLED") && rule["QV2RAY_RULE_ENABLED"].toBool() == false)
                {
                    LOG("Discarded a rule as it's been set DISABLED");
                }
                else
                {
                    newRules.append(rule);
                }
            }

            routing["rules"] = newRules;
            root["routing"] = routing;
            root["outbounds"] = ExpandExternalConnection(OUTBOUNDS(root["outbounds"].toArray()));
            const auto result = ExpandChainedOutbounds(root);
            if (!result)
                ;
        }
        else
        {
            LOG("Processing a simple connection config");
            if (root["outbounds"].toArray().count() != 1)
            {
                // There are no ROUTING but 2 or more outbounds.... This is rare, but possible.
                LOG("WARN: This message usually indicates the config file has logic errors:");
                LOG("WARN: --> The config file has NO routing section, however more than 1 outbounds are detected.");
            }
            //
            auto tag = QJsonIO::GetValue(root, "outbounds", 0, "tag").toString();
            if (tag.isEmpty())
            {
                LOG("Applying workaround when an outbound tag is empty");
                tag = GenerateRandomString(15);
                QJsonIO::SetValue(root, tag, "outbounds", 0, "tag");
            }
            root["routing"] = GenerateRoutes(connConf.enableProxy, connConf.bypassCN, connConf.bypassLAN, tag, routeConf);

            //
            // Forward proxy
            if (fpConf.enableForwardProxy)
            {
                if (QJsonIO::GetValue(root, "outbounds", 0, QV2RAY_USE_FPROXY_KEY).toBool(false))
                {
                    if (fpConf.type.isEmpty())
                    {
                        DEBUG("WARNING: Empty forward proxy type.");
                    }
                    else if (fpConf.type.toLower() != "http" && fpConf.type.toLower() != "socks")
                    {
                        DEBUG("WARNING: Unsupported forward proxy type: " + fpConf.type);
                    }
                    else
                    {
                        const static QJsonObject proxySettings{ { "tag", OUTBOUND_TAG_FORWARD_PROXY } };
                        LOG("Applying forward proxy to current connection.");
                        QJsonIO::SetValue(root, proxySettings, "outbounds", 0, "proxySettings");
                        const auto forwardProxySettings = GenerateHTTPSOCKSOut(fpConf.serverAddress, //
                                                                               fpConf.port,          //
                                                                               fpConf.useAuth,       //
                                                                               fpConf.username,      //
                                                                               fpConf.password);
                        const auto forwardProxyOutbound = GenerateOutboundEntry(OUTBOUND_TAG_FORWARD_PROXY, //
                                                                                fpConf.type.toLower(),      //
                                                                                forwardProxySettings, {});
                        auto outboundArray = root["outbounds"].toArray();
                        outboundArray.push_back(forwardProxyOutbound);
                        root["outbounds"] = outboundArray;
                    }
                }
                else
                {
                    // Remove proxySettings from first outbound
                    QJsonIO::SetValue(root, QJsonIO::Undefined, "outbounds", 0, "proxySettings");
                }
            }

            //
            // Process FREEDOM and BLACKHOLE outbound
            {
                OUTBOUNDS outbounds(root["outbounds"].toArray());
                const auto freeDS = (connConf.v2rayFreedomDNS) ? "UseIP" : "AsIs";
                outbounds.append(GenerateOutboundEntry(OUTBOUND_TAG_DIRECT, "freedom", GenerateFreedomOUT(freeDS, ":0"), {}));
                outbounds.append(GenerateOutboundEntry(OUTBOUND_TAG_BLACKHOLE, "blackhole", GenerateBlackHoleOUT(false), {}));
                root["outbounds"] = outbounds;
            }

            //
            // Connection Filters
            {
                if (GlobalConfig.defaultRouteConfig.connectionConfig.dnsIntercept)
                {
                    const auto hasTProxy = GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.inboundConfig.tProxySettings.hasUDP;
                    const auto hasIPv6 = hasTProxy && (!GlobalConfig.inboundConfig.tProxySettings.tProxyV6IP.isEmpty());
                    const auto hasSocksUDP = GlobalConfig.inboundConfig.useSocks && GlobalConfig.inboundConfig.socksSettings.enableUDP;
                    DNSInterceptFilter(root, hasTProxy, hasIPv6, hasSocksUDP);
                }

                if (GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.outboundConfig.mark > 0)
                    OutboundMarkSettingFilter(root, GlobalConfig.outboundConfig.mark);

                // Process bypass bitTorrent
                if (connConf.bypassBT)
                    BypassBTFilter(root);

                // Process mKCP seed
                mKCPSeedFilter(root);

                // Remove empty Mux object from settings
                RemoveEmptyMuxFilter(root);
            }
        }

        //
        // Process Log
        QJsonIO::SetValue(root, V2RayLogLevel[GlobalConfig.logLevel], "log", "loglevel");

        //
        // Process DNS
        const auto hasDNS = root.contains("dns") && !root.value("dns").toObject().isEmpty();
        if (!hasDNS)
        {
            root.insert("dns", GenerateDNS(connConf.fakeDNS, dnsConf));
            LOG("Added global DNS config");
        }

        //
        // If inbounds list is empty, we append our global configured inbounds to the config.
        // The setting applies to BOTH complex config AND simple config.
        // Just to ensure there's AT LEAST 1 possible inbound is being configured.
        if (!root.contains("inbounds") || root.value("inbounds").toArray().empty())
        {
            root["inbounds"] = GenerateDefaultInbounds();
            DEBUG("Added global inbound config");
        }

        //
        // API 0 speed issue occured when no tag is configured.
        FillupTagsFilter(root, "inbounds");
        FillupTagsFilter(root, "outbounds");

        //
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
            const static QJsonObject APIRouteRoot{ { "type", "field" },                //
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
            static const QJsonObject fakeDocodemoDoor{ { "address", "127.0.0.1" } };
            const auto apiInboundsRoot = GenerateInboundEntry(API_TAG_INBOUND, "dokodemo-door",    //
                                                              "127.0.0.1",                         //
                                                              GlobalConfig.kernelConfig.statsPort, //
                                                              INBOUNDSETTING(fakeDocodemoDoor));
            inbounds.push_front(apiInboundsRoot);
            root["inbounds"] = inbounds;
            //
            // API
            root["api"] = GenerateAPIEntry(API_TAG_DEFAULT);
        }

        return root;
    }
} // namespace Qv2ray::core::handler
