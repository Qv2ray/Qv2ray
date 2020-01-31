#include "Generation.hpp"
#include "core/CoreUtils.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::connection
{
    namespace Generation
    {
        // -------------------------- BEGIN CONFIG GENERATIONS ----------------------------------------------------------------------------
        ROUTING GenerateRoutes(bool enableProxy, bool proxyCN)
        {
            ROUTING root;
            root.insert("domainStrategy", "IPIfNonMatch");
            //
            // For Rules list
            ROUTERULELIST rulesList;

            if (!enableProxy) {
                // This is added to disable all proxies, as a alternative influence of #64
                rulesList.append(GenerateSingleRouteRule(QStringList() << "regexp:.*", true, OUTBOUND_TAG_DIRECT));
            }

            // Private IPs should always NOT TO PROXY!
            rulesList.append(GenerateSingleRouteRule(QStringList() << "geoip:private", false, OUTBOUND_TAG_DIRECT));
            //
            // Check if CN needs proxy, or direct.
            rulesList.append(GenerateSingleRouteRule(QStringList() << "geoip:cn", false, proxyCN ? OUTBOUND_TAG_DIRECT : OUTBOUND_TAG_PROXY));
            rulesList.append(GenerateSingleRouteRule(QStringList() << "geosite:cn", true, proxyCN ? OUTBOUND_TAG_DIRECT : OUTBOUND_TAG_PROXY));
            //
            // As a bug fix of #64, this default rule has been disabled.
            //rulesList.append(GenerateSingleRouteRule(QStringList({"regexp:.*"}), true, globalProxy ? OUTBOUND_TAG_PROXY :  OUTBOUND_TAG_DIRECT));
            root.insert("rules", rulesList);
            RROOT
        }

        ROUTERULE GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type)
        {
            ROUTERULE root;
            root.insert(isDomain ? "domain" : "ip", QJsonArray::fromStringList(list));
            JADD(outboundTag, type)
            RROOT
        }

        OUTBOUNDSETTING GenerateFreedomOUT(QString domainStrategy, QString redirect, int userLevel)
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

        OUTBOUNDSETTING GenerateShadowSocksOUT(QList<ShadowSocksServerObject> servers)
        {
            OUTBOUNDSETTING root;
            QJsonArray x;

            foreach (auto server, servers) {
                x.append(GenerateShadowSocksServerOUT(server.email, server.address, server.port, server.method, server.password, server.ota, server.level));
            }

            root.insert("servers", x);
            RROOT
        }

        OUTBOUNDSETTING GenerateShadowSocksServerOUT(QString email, QString address, int port, QString method, QString password, bool ota, int level)
        {
            OUTBOUNDSETTING root;
            JADD(email, address, port, method, password, level, ota)
            RROOT
        }

        QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers)
        {
            QJsonObject root;
            QJsonArray servers(QJsonArray::fromStringList(dnsServers));

            if (withLocalhost) {
                // https://github.com/lhy0403/Qv2ray/issues/64
                // The fix patch didn't touch this line below.
                //
                // Should we APPEND localhost or PUSH_FRONT localhost?
                servers.append("localhost");
            }

            root.insert("servers", servers);
            RROOT
        }

        INBOUNDSETTING GenerateDokodemoIN(QString address, int port, QString  network, int timeout, bool followRedirect, int userLevel)
        {
            INBOUNDSETTING root;
            JADD(address, port, network, timeout, followRedirect, userLevel)
            RROOT
        }

        INBOUNDSETTING GenerateHTTPIN(QList<AccountObject> _accounts, int timeout, bool allowTransparent, int userLevel)
        {
            INBOUNDSETTING root;
            QJsonArray accounts;

            foreach (auto account, _accounts) {
                if (account.user.isEmpty() && account.pass.isEmpty()) {
                    continue;
                }

                accounts.append(GetRootObject(account));
            }

            if (!accounts.isEmpty()) {
                JADD(accounts)
            }

            JADD(timeout, allowTransparent, userLevel)
            RROOT
        }

        OUTBOUNDSETTING GenerateHTTPSOCKSOut(QString address, int port, bool useAuth, QString username, QString password)
        {
            OUTBOUNDSETTING root;
            QJsonArray servers;
            {
                QJsonObject oneServer;
                oneServer["address"] = address;
                oneServer["port"] = port;

                if (useAuth) {
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

        INBOUNDSETTING GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp, QString ip, int userLevel)
        {
            INBOUNDSETTING root;
            QJsonArray accounts;

            foreach (auto acc, _accounts) {
                if (acc.user.isEmpty() && acc.pass.isEmpty()) {
                    continue;
                }

                accounts.append(GetRootObject(acc));
            }

            if (!accounts.isEmpty()) {
                JADD(accounts)
            }

            if (udp) {
                JADD(auth, udp, ip, userLevel)
            } else {
                JADD(auth, userLevel)
            }

            RROOT
        }

        OUTBOUND GenerateOutboundEntry(QString protocol, OUTBOUNDSETTING settings, QJsonObject streamSettings, QJsonObject mux, QString sendThrough, QString tag)
        {
            OUTBOUND root;
            JADD(sendThrough, protocol, settings, tag, streamSettings, mux)
            RROOT
        }

        INBOUND GenerateInboundEntry(QString listen, int port, QString protocol, INBOUNDSETTING settings, QString tag, QJsonObject sniffing, QJsonObject allocate)
        {
            INBOUND root;
            LOG(MODULE_CONNECTION, "allocation is not used here.")
            Q_UNUSED(allocate)
            JADD(listen, port, protocol, settings, tag, sniffing)
            RROOT
        }

        QJsonObject GenerateAPIEntry(QString tag, bool withHandler, bool withLogger, bool withStats)
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

        // -------------------------- END CONFIG GENERATIONS ------------------------------------------------------------------------------
        // BEGIN RUNTIME CONFIG GENERATION

        CONFIGROOT GenerateRuntimeConfig(CONFIGROOT root)
        {
            QJsonObject logObject;
            //
            //logObject.insert("access", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ACCESS_LOG_FILENAME);
            //logObject.insert("error", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ERROR_LOG_FILENAME);
            //
            logObject.insert("loglevel", vLogLevels[GlobalConfig.logLevel]);
            root.insert("log", logObject);
            //
            auto dnsList = GlobalConfig.connectionConfig.dnsList;
            auto dnsObject = GenerateDNS(GlobalConfig.connectionConfig.withLocalDNS, dnsList);
            root.insert("dns", dnsObject);
            //
            //

            // If inbounds list is empty we append our global configured inbounds to the config.
            if (!root.contains("inbounds") || root["inbounds"].toArray().empty()) {
                INBOUNDS inboundsList;

                // HTTP Inbound
                if (GlobalConfig.inboundConfig.useHTTP) {
                    INBOUND httpInBoundObject;
                    httpInBoundObject.insert("listen", GlobalConfig.inboundConfig.listenip);
                    httpInBoundObject.insert("port", GlobalConfig.inboundConfig.http_port);
                    httpInBoundObject.insert("protocol", "http");
                    httpInBoundObject.insert("tag", "http_IN");

                    if (GlobalConfig.inboundConfig.http_useAuth) {
                        auto httpInSettings =  GenerateHTTPIN(QList<AccountObject>() << GlobalConfig.inboundConfig.httpAccount);
                        httpInBoundObject.insert("settings", httpInSettings);
                    }

                    inboundsList.append(httpInBoundObject);
                }

                // SOCKS Inbound
                if (GlobalConfig.inboundConfig.useSocks) {
                    INBOUND socksInBoundObject;
                    socksInBoundObject.insert("listen", GlobalConfig.inboundConfig.listenip);
                    socksInBoundObject.insert("port", GlobalConfig.inboundConfig.socks_port);
                    socksInBoundObject.insert("protocol", "socks");
                    socksInBoundObject.insert("tag", "socks_IN");
                    auto socksInSettings = GenerateSocksIN(GlobalConfig.inboundConfig.socks_useAuth ? "password" : "noauth",
                                                           QList<AccountObject>() << GlobalConfig.inboundConfig.socksAccount,
                                                           GlobalConfig.inboundConfig.socksUDP,
                                                           GlobalConfig.inboundConfig.socksLocalIP);
                    socksInBoundObject.insert("settings", socksInSettings);
                    inboundsList.append(socksInBoundObject);
                }

                root["inbounds"] = inboundsList;
                DEBUG(MODULE_CONFIG, "Added global config inbounds to the config")
            }

            // Process every inbounds to make sure a tag is configured, fixed API 0 speed
            // issue when no tag is configured.
            INBOUNDS newTaggedInbounds = INBOUNDS(root["inbounds"].toArray());

            for (auto i = 0; i < newTaggedInbounds.count(); i++) {
                auto _inboundItem = newTaggedInbounds[i].toObject();

                if (!_inboundItem.contains("tag") || _inboundItem["tag"].toString().isEmpty()) {
                    LOG(MODULE_CONFIG, "Adding a tag to an inbound.")
                    _inboundItem["tag"] = GenerateRandomString(8);
                    newTaggedInbounds[i] = _inboundItem;
                }
            }

            root["inbounds"] = newTaggedInbounds;
            //
            //
            // Note: The part below always makes the whole functionality in trouble......
            // BE EXTREME CAREFUL when changing these code below...
            // See: https://github.com/lhy0403/Qv2ray/issues/129
            // routeCountLabel in Mainwindow makes here failed to ENOUGH-ly check the routing tables
            bool isComplex = CheckIsComplexConfig(root);

            if (isComplex) {
                // For some config files that has routing entries already.
                // We DO NOT add extra routings.
                //
                // HOWEVER, we need to verify the QV2RAY_RULE_ENABLED entry.
                // And what's more, process (by removing unused items) from a rule object.
                ROUTING routing = ROUTING(root["routing"].toObject());
                ROUTERULELIST rules;
                LOG(MODULE_CONNECTION, "Processing an existing routing table.")

                for (auto _rule : routing["rules"].toArray()) {
                    auto _b = _rule.toObject();

                    if (_b.contains("QV2RAY_RULE_USE_BALANCER")) {
                        if (_b["QV2RAY_RULE_USE_BALANCER"].toBool()) {
                            // We use balancer
                            _b.remove("outboundTag");
                        } else {
                            // We only use the normal outbound
                            _b.remove("balancerTag");
                        }
                    } else {
                        LOG(MODULE_CONFIG, "We found a rule without QV2RAY_RULE_USE_BALANCER, so don't process it.")
                    }

                    // If this entry has been disabled.
                    if (_b.contains("QV2RAY_RULE_ENABLED") && _b["QV2RAY_RULE_ENABLED"].toBool() == true) {
                        rules.append(_b);
                    } else {
                        LOG(MODULE_CONFIG, "Discarded a rule as it's been set DISABLED")
                    }
                }

                routing["rules"] = rules;
                root["routing"] = routing;
            } else {
                //
                LOG(MODULE_CONNECTION, "Inserting default values to simple config")

                if (root["outbounds"].toArray().count() != 1) {
                    // There are no ROUTING but 2 or more outbounds.... This is rare, but possible.
                    LOG(MODULE_CONNECTION, "WARN: This message usually indicates the config file has logic errors:")
                    LOG(MODULE_CONNECTION, "WARN: --> The config file has NO routing section, however more than 1 outbounds are detected.")
                }

                auto routeObject = GenerateRoutes(GlobalConfig.connectionConfig.enableProxy, GlobalConfig.connectionConfig.bypassCN);
                root.insert("routing", routeObject);
                //
                // Process forward proxy
#define fpConf GlobalConfig.connectionConfig.forwardProxyConfig

                if (fpConf.enableForwardProxy) {
                    auto outboundArray = root["outbounds"].toArray();
                    auto firstOutbound = outboundArray.first().toObject();

                    if (firstOutbound[QV2RAY_USE_FPROXY_KEY].toBool(false)) {
                        LOG(MODULE_CONNECTION, "Applying forward proxy to current connection.")
                        auto proxy = PROXYSETTING();
                        proxy["tag"] = OUTBOUND_TAG_FORWARD_PROXY;
                        firstOutbound["proxySettings"] = proxy;
                        // FP Outbound.
                        OUTBOUNDSETTING fpOutbound;

                        if (fpConf.type.toLower() == "http" || fpConf.type.toLower() == "socks") {
                            fpOutbound = GenerateHTTPSOCKSOut(fpConf.serverAddress, fpConf.port, fpConf.useAuth, fpConf.username, fpConf.password);
                            outboundArray.push_back(GenerateOutboundEntry(fpConf.type.toLower(), fpOutbound, QJsonObject(), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_FORWARD_PROXY));
                        } else {
                            LOG(MODULE_CONNECTION, "WARNING: Unsupported outbound type: " + fpConf.type)
                        }
                    } else {
                        // Remove proxySettings from firstOutbound
                        firstOutbound.remove("proxySettings");
                    }

                    outboundArray.replace(0, firstOutbound);
                    root["outbounds"] = outboundArray;
                }

#undef fpConf
                OUTBOUNDS outbounds = OUTBOUNDS(root["outbounds"].toArray());
                outbounds.append(GenerateOutboundEntry("freedom", GenerateFreedomOUT("AsIs", ":0", 0), QJsonObject(), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_DIRECT));
                root["outbounds"] = outbounds;
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
                INBOUNDS inbounds = INBOUNDS(root["inbounds"].toArray());
                INBOUNDSETTING fakeDocodemoDoor;
                fakeDocodemoDoor["address"] = "127.0.0.1";
                QJsonObject apiInboundsRoot = GenerateInboundEntry("127.0.0.1", GlobalConfig.apiConfig.statsPort, "dokodemo-door", fakeDocodemoDoor, API_TAG_INBOUND);
                inbounds.push_front(apiInboundsRoot);
                root["inbounds"] = inbounds;
                //
                // API
                //
                root["api"] = GenerateAPIEntry(API_TAG_DEFAULT);
            }
            return root;
        }
    }
}
