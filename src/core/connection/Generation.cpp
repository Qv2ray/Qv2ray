#include "Generation.hpp"

#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"

namespace Qv2ray::core::connection
{
    namespace Generation
    {
        // -------------------------- BEGIN CONFIG GENERATIONS
        ROUTING GenerateRoutes(bool enableProxy, bool bypassCN, const QString &defaultOutboundTag)
        {
            auto &routeConfig = GlobalConfig.connectionConfig.routeConfig;
            ROUTING root;
            root.insert("domainStrategy", routeConfig.domainStrategy);
            //
            // For Rules list
            ROUTERULELIST rulesList;
            if (!enableProxy)
            {
                // This is added to disable all proxies, as a alternative
                // influence of #64
                rulesList.append(GenerateSingleRouteRule("regexp:.*", true, OUTBOUND_TAG_DIRECT));
            }

            // Private IPs should always NOT TO PROXY!
            rulesList.append(GenerateSingleRouteRule("geoip:private", false, OUTBOUND_TAG_DIRECT));
            //
            // To the route list.
            if (!routeConfig.domains.block.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.domains.block, true, OUTBOUND_TAG_BLACKHOLE));
            }
            if (!routeConfig.domains.proxy.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.domains.proxy, true, defaultOutboundTag));
            }
            if (!routeConfig.domains.direct.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.domains.direct, true, OUTBOUND_TAG_DIRECT));
            }

            // IP list
            if (!routeConfig.ips.block.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.ips.block, false, OUTBOUND_TAG_BLACKHOLE));
            }
            if (!routeConfig.ips.proxy.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.ips.proxy, false, defaultOutboundTag));
            }
            if (!routeConfig.ips.direct.isEmpty())
            {
                rulesList.append(GenerateSingleRouteRule(routeConfig.ips.direct, false, OUTBOUND_TAG_DIRECT));
            }
            //
            // Check if CN needs proxy, or direct.
            if (bypassCN)
            {
                // No proxy agains CN addresses.
                rulesList.append(GenerateSingleRouteRule("geoip:cn", false, OUTBOUND_TAG_DIRECT));
                rulesList.append(GenerateSingleRouteRule("geosite:cn", true, OUTBOUND_TAG_DIRECT));
            }
            //
            //
            // As a bug fix of #64, this default rule has been disabled.
            // rulesList.append(GenerateSingleRouteRule(QStringList({"regexp:.*"}),
            // true, globalProxy ? OUTBOUND_TAG_PROXY :  OUTBOUND_TAG_DIRECT));
            root.insert("rules", rulesList);
            RROOT
        }

        ROUTERULE GenerateSingleRouteRule(const QString &str, bool isDomain, const QString &outboundTag, const QString &type)
        {
            return GenerateSingleRouteRule(QStringList{ str }, isDomain, outboundTag, type);
        }

        ROUTERULE GenerateSingleRouteRule(const QStringList &rules, bool isDomain, const QString &outboundTag, const QString &type)
        {
            ROUTERULE root;
            auto list = rules;
            list.removeAll("");
            root.insert(isDomain ? "domain" : "ip", QJsonArray::fromStringList(rules));
            JADD(outboundTag, type)
            RROOT
        }

        OUTBOUNDSETTING GenerateFreedomOUT(const QString &domainStrategy, const QString &redirect, int userLevel)
        {
            OUTBOUNDSETTING root;
            JADD(domainStrategy, redirect, userLevel)
            RROOT
        }
        OUTBOUNDSETTING GenerateBlackHoleOUT(bool useHTTP)
        {
            OUTBOUNDSETTING root;
            QJsonObject resp;
            resp.insert("type", useHTTP ? "http" : "none");
            root.insert("response", resp);
            RROOT
        }

        OUTBOUNDSETTING GenerateShadowSocksOUT(const QList<ShadowSocksServerObject> &servers)
        {
            OUTBOUNDSETTING root;
            QJsonArray x;

            for (auto server : servers)
            {
                x.append(GenerateShadowSocksServerOUT(server.email, server.address, server.port, server.method, server.password, server.ota,
                                                      server.level));
            }

            root.insert("servers", x);
            RROOT
        }

        OUTBOUNDSETTING GenerateShadowSocksServerOUT(const QString &email, const QString &address, int port, const QString &method,
                                                     const QString &password, bool ota, int level)
        {
            OUTBOUNDSETTING root;
            JADD(email, address, port, method, password, level, ota)
            RROOT
        }

        QJsonObject GenerateDNS(bool withLocalhost, const QStringList &dnsServers)
        {
            QJsonObject root;
            QJsonArray servers(QJsonArray::fromStringList(dnsServers));

            if (withLocalhost)
            {
                // https://github.com/Qv2ray/Qv2ray/issues/64
                // The fix patch didn't touch this line below.
                //
                // Should we APPEND localhost or PUSH_FRONT localhost?
                servers.append("localhost");
            }

            root.insert("servers", servers);
            RROOT
        }

        INBOUNDSETTING GenerateDokodemoIN(const QString &address, int port, const QString &network, int timeout, bool followRedirect,
                                          int userLevel)
        {
            INBOUNDSETTING root;
            JADD(address, port, network, timeout, followRedirect, userLevel)
            RROOT
        }

        INBOUNDSETTING GenerateHTTPIN(const QList<AccountObject> &_accounts, int timeout, bool allowTransparent, int userLevel)
        {
            INBOUNDSETTING root;
            QJsonArray accounts;

            for (auto account : _accounts)
            {
                if (account.user.isEmpty() && account.pass.isEmpty())
                {
                    continue;
                }

                accounts.append(GetRootObject(account));
            }

            if (!accounts.isEmpty())
            {
                JADD(accounts)
            }

            JADD(timeout, allowTransparent, userLevel)
            RROOT
        }

        OUTBOUNDSETTING GenerateHTTPSOCKSOut(const QString &address, int port, bool useAuth, const QString &username, const QString &password)
        {
            OUTBOUNDSETTING root;
            QJsonArray servers;
            {
                QJsonObject oneServer;
                oneServer["address"] = address;
                oneServer["port"] = port;

                if (useAuth)
                {
                    QJsonArray users;
                    QJsonObject oneUser;
                    oneUser["user"] = username;
                    oneUser["pass"] = password;
                    users.push_back(oneUser);
                    oneServer["users"] = users;
                }

                servers.push_back(oneServer);
            }
            JADD(servers)
            RROOT
        }

        INBOUNDSETTING GenerateSocksIN(const QString &auth, const QList<AccountObject> &_accounts, bool udp, const QString &ip, int userLevel)
        {
            INBOUNDSETTING root;
            QJsonArray accounts;

            foreach (auto acc, _accounts)
            {
                if (acc.user.isEmpty() && acc.pass.isEmpty())
                {
                    continue;
                }

                accounts.append(GetRootObject(acc));
            }

            if (!accounts.isEmpty())
            {
                JADD(accounts)
            }

            if (udp)
            {
                JADD(auth, udp, ip, userLevel)
            }
            else
            {
                JADD(auth, userLevel)
            }

            RROOT
        }

        OUTBOUND GenerateOutboundEntry(const QString &protocol, const OUTBOUNDSETTING &settings, const QJsonObject &streamSettings,
                                       const QJsonObject &mux, const QString &sendThrough, const QString &tag)
        {
            OUTBOUND root;
            JADD(sendThrough, protocol, settings, tag, streamSettings, mux)
            RROOT
        }

        INBOUND GenerateInboundEntry(const QString &listen, int port, const QString &protocol, const INBOUNDSETTING &settings,
                                     const QString &tag, const QJsonObject &sniffing, const QJsonObject &allocate)
        {
            INBOUND root;
            DEBUG(MODULE_CONNECTION, "Allocation is not used here, Not Implemented")
            Q_UNUSED(allocate)
            JADD(listen, port, protocol, settings, tag, sniffing)
            RROOT
        }

        QJsonObject GenerateAPIEntry(const QString &tag, bool withHandler, bool withLogger, bool withStats)
        {
            QJsonObject root;
            QJsonArray services;

            if (withHandler)
                services << "HandlerService";

            if (withLogger)
                services << "LoggerService";

            if (withStats)
                services << "StatsService";

            JADD(services, tag)
            RROOT
        }

        // -------------------------- END CONFIG GENERATIONS
        //
        // BEGIN RUNTIME CONFIG GENERATION
        // We need copy construct here
        CONFIGROOT GenerateRuntimeConfig(CONFIGROOT root)
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
            logObject.insert("loglevel", vLogLevels[GlobalConfig.logLevel]);
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
                INBOUNDS inboundsList;
                QJsonObject sniffingObject{ { "enabled", false } };
                // HTTP Inbound
                if (GlobalConfig.inboundConfig.useHTTP)
                {
                    INBOUND httpInBoundObject;
                    httpInBoundObject.insert("listen", GlobalConfig.inboundConfig.listenip);
                    httpInBoundObject.insert("port", GlobalConfig.inboundConfig.http_port);
                    httpInBoundObject.insert("protocol", "http");
                    httpInBoundObject.insert("tag", "http_IN");
                    httpInBoundObject.insert("sniffing", sniffingObject);

                    if (GlobalConfig.inboundConfig.http_useAuth)
                    {
                        auto httpInSettings = GenerateHTTPIN(QList<AccountObject>() << GlobalConfig.inboundConfig.httpAccount);
                        httpInBoundObject.insert("settings", httpInSettings);
                    }

                    inboundsList.append(httpInBoundObject);
                }

                // SOCKS Inbound
                if (GlobalConfig.inboundConfig.useSocks)
                {
                    INBOUND socksInBoundObject;
                    socksInBoundObject.insert("listen", GlobalConfig.inboundConfig.listenip);
                    socksInBoundObject.insert("port", GlobalConfig.inboundConfig.socks_port);
                    socksInBoundObject.insert("protocol", "socks");
                    socksInBoundObject.insert("tag", "socks_IN");
                    socksInBoundObject.insert("sniffing", sniffingObject);
                    auto socksInSettings = GenerateSocksIN(GlobalConfig.inboundConfig.socks_useAuth ? "password" : "noauth",
                                                           QList<AccountObject>() << GlobalConfig.inboundConfig.socksAccount,
                                                           GlobalConfig.inboundConfig.socksUDP, GlobalConfig.inboundConfig.socksLocalIP);
                    socksInBoundObject.insert("settings", socksInSettings);
                    inboundsList.append(socksInBoundObject);
                }

                // TPROXY
                if (GlobalConfig.inboundConfig.useTPROXY)
                {
                    INBOUND tproxyInBoundObject;
                    tproxyInBoundObject.insert("listen", GlobalConfig.inboundConfig.tproxy_ip);
                    tproxyInBoundObject.insert("port", GlobalConfig.inboundConfig.tproxy_port);
                    tproxyInBoundObject.insert("protocol", "dokodemo-door");
                    tproxyInBoundObject.insert("tag", "tproxy_IN");

                    QList<QString> networks;
                    if (GlobalConfig.inboundConfig.tproxy_use_tcp)
                        networks << "tcp";
                    if (GlobalConfig.inboundConfig.tproxy_use_udp)
                        networks << "udp";
                    const auto tproxy_network = networks.join(",");

                    auto tproxyInSettings = GenerateDokodemoIN("", 0, tproxy_network, 0, true, 0);
                    tproxyInBoundObject.insert("settings", tproxyInSettings);

                    QJsonObject tproxy_sniff{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };
                    tproxyInBoundObject.insert("sniffing", tproxy_sniff);
                    //                    tproxyInBoundObject.insert("sniffing", sniffingObject);

                    QJsonObject tproxy_streamSettings{ { "sockopt", QJsonObject{ { "tproxy", GlobalConfig.inboundConfig.tproxy_mode } } } };
                    tproxyInBoundObject.insert("streamSettings", tproxy_streamSettings);

                    inboundsList.append(tproxyInBoundObject);
                }

                root["inbounds"] = inboundsList;
                DEBUG(MODULE_CONNECTION, "Added global config inbounds to the config")
            }

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
                outbounds.append(GenerateOutboundEntry("freedom", GenerateFreedomOUT("AsIs", ":0", 0), {}, {}, "0.0.0.0", OUTBOUND_TAG_DIRECT));
                outbounds.append(GenerateOutboundEntry("blackhole", GenerateBlackHoleOUT(false), {}, {}, "0.0.0.0", OUTBOUND_TAG_BLACKHOLE));
                //
                root["outbounds"] = outbounds;
                // mark outbound if necessary
                if (GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.outboundConfig.mark > 0)
                {
                    OutboundMarkSettingFilter(GlobalConfig.outboundConfig.mark, root);
                }
                if (GlobalConfig.inboundConfig.useTPROXY && GlobalConfig.inboundConfig.dnsIntercept)
                {
                    DNSInterceptFilter(root);
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
                    GenerateInboundEntry("127.0.0.1", GlobalConfig.apiConfig.statsPort, "dokodemo-door", fakeDocodemoDoor, API_TAG_INBOUND);
                inbounds.push_front(apiInboundsRoot);
                root["inbounds"] = inbounds;
                //
                // API
                //
                root["api"] = GenerateAPIEntry(API_TAG_DEFAULT);
            }

            return root;
        }

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

    } // namespace Generation
} // namespace Qv2ray::core::connection
