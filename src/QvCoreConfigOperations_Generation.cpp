#include "QvCoreConfigOperations.hpp"

namespace Qv2ray
{
    namespace ConfigOperations
    {
        inline namespace Generation
        {
            // Important config generation algorithms.
            static const QStringList vLogLevels = {"none", "debug", "info", "warning", "error"};
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

            OUTBOUNDSETTING GenerateShadowSocksOUT(QList<QJsonObject> servers)
            {
                OUTBOUNDSETTING root;
                QJsonArray x;

                foreach (auto server, servers) {
                    x.append(server);
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
                    accounts.append(GetRootObject(account));
                }

                JADD(timeout, accounts, allowTransparent, userLevel)
                RROOT
            }

            INBOUNDSETTING GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp, QString ip, int userLevel)
            {
                INBOUNDSETTING root;
                QJsonArray accounts;

                foreach (auto acc, _accounts) {
                    accounts.append(GetRootObject(acc));
                }

                if (udp) {
                    JADD(auth, accounts, udp, ip, userLevel)
                } else {
                    JADD(auth, accounts, userLevel)
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
                auto gConf = GetGlobalConfig();
                QJsonObject logObject;
                //
                //logObject.insert("access", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ACCESS_LOG_FILENAME);
                //logObject.insert("error", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ERROR_LOG_FILENAME);
                //
                logObject.insert("loglevel", vLogLevels[gConf.logLevel]);
                root.insert("log", logObject);
                //
                QStringList dnsList;

                foreach (auto str, gConf.connectionConfig.dnsList) {
                    dnsList.append(QString::fromStdString(str));
                }

                auto dnsObject = GenerateDNS(gConf.connectionConfig.withLocalDNS, dnsList);
                root.insert("dns", dnsObject);
                //
                //
                INBOUNDS inboundsList;

                // HTTP InBound
                if (gConf.inboundConfig.useHTTP) {
                    INBOUND httpInBoundObject;
                    httpInBoundObject.insert("listen", QString::fromStdString(gConf.inboundConfig.listenip));
                    httpInBoundObject.insert("port", gConf.inboundConfig.http_port);
                    httpInBoundObject.insert("protocol", "http");
                    httpInBoundObject.insert("tag", "http_IN");

                    if (gConf.inboundConfig.http_useAuth) {
                        auto httpInSettings =  GenerateHTTPIN(QList<AccountObject>() << gConf.inboundConfig.httpAccount);
                        httpInBoundObject.insert("settings", httpInSettings);
                    }

                    inboundsList.append(httpInBoundObject);
                }

                // SOCKS InBound
                if (gConf.inboundConfig.useSocks) {
                    INBOUND socksInBoundObject;
                    socksInBoundObject.insert("listen", QString::fromStdString(gConf.inboundConfig.listenip));
                    socksInBoundObject.insert("port", gConf.inboundConfig.socks_port);
                    socksInBoundObject.insert("protocol", "socks");
                    socksInBoundObject.insert("tag", "socks_IN");
                    auto socksInSettings = GenerateSocksIN(gConf.inboundConfig.socks_useAuth ? "password" : "noauth",
                                                           QList<AccountObject>() << gConf.inboundConfig.socksAccount,
                                                           gConf.inboundConfig.socksUDP,
                                                           QSTRING(gConf.inboundConfig.socksLocalIP));
                    socksInBoundObject.insert("settings", socksInSettings);
                    inboundsList.append(socksInBoundObject);
                }

                if (!root.contains("inbounds") || root["inbounds"].toArray().empty()) {
                    root.insert("inbounds", inboundsList);
                }

                // Note: The part below always makes the whole functionality in trouble......
                // BE EXTREME CAREFUL when changing these code below...
                // See: https://github.com/lhy0403/Qv2ray/issues/129
                // routeCountLabel in Mainwindow makes here failed to ENOUGH-ly check the routing tables
                bool isComplex = CheckIsComplexConfig(root);

                //
                if (isComplex) {  // For some config files that has routing entries already.
                    // We DO NOT add extra routings.
                    //
                    // HOWEVER, we need to verify the QV2RAY_RULE_ENABLED entry.
                    // And what's more, process (by removing unused items) from a rule object.
                    ROUTING routing = ROUTING(root["routing"].toObject());
                    ROUTERULELIST rules;
                    LOG(MODULE_CONNECTION, "Processing an existing routing table.")

                    for (auto _a : routing["rules"].toArray()) {
                        auto _b = _a.toObject();

                        if (_b.contains("QV2RAY_RULE_USE_BALANCER")) {
                            if (_b["QV2RAY_RULE_USE_BALANCER"].toBool()) {
                                // We use balancer
                                _b.remove("outboundTag");
                            } else {
                                // We only use the normal outbound
                                _b.remove("balancerTag");
                            }
                        } else {
                            LOG(MODULE_CONFIG, "We found a rule without QV2RAY_RULE_USE_BALANCER, and we didn't process it.")
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
                    LOG(MODULE_CONNECTION, "Current connection has NO ROUTING section, we insert default values.")

                    if (root["outbounds"].toArray().count() != 1) {
                        // There are no ROUTING but 2 or more outbounds.... This is rare, but possible.
                        LOG(MODULE_CONNECTION, "WARN: This message usually indicates the config file has logic errors:")
                        LOG(MODULE_CONNECTION, "WARN: --> The config file has NO routing section, however more than 1 outbounds are detected.")
                    }

                    auto routeObject = GenerateRoutes(gConf.connectionConfig.enableProxy, gConf.connectionConfig.bypassCN);
                    root.insert("routing", routeObject);
                    OUTBOUNDS outbounds = OUTBOUNDS(root["outbounds"].toArray());
                    outbounds.append(GenerateOutboundEntry("freedom", GenerateFreedomOUT("AsIs", ":0", 0), QJsonObject(), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_DIRECT));
                    root["outbounds"] = outbounds;
                }

                // Let's process some api features.
                if (gConf.connectionConfig.enableStats) {
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
                    APIRouteRoot["outboundTag"] = QV2RAY_API_TAG_DEFAULT;
                    QJsonArray inboundTag;
                    inboundTag.append(QV2RAY_API_TAG_INBOUND);
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
                    QJsonObject apiInboundsRoot = GenerateInboundEntry("127.0.0.1", gConf.connectionConfig.statsPort, "dokodemo-door", fakeDocodemoDoor, QV2RAY_API_TAG_INBOUND);
                    inbounds.push_front(apiInboundsRoot);
                    root["inbounds"] = inbounds;
                    //
                    // API
                    //
                    root["api"] = GenerateAPIEntry(QV2RAY_API_TAG_DEFAULT);
                    //
                }

                return root;
            }
        }
    }
}
