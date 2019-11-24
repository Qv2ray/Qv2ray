#include "QvCoreConfigOperations.hpp"

namespace Qv2ray
{
    namespace ConfigOperations
    {
        namespace Generation
        {
            // Important config generation algorithms.
            static const QStringList vLogLevels = {"none", "debug", "info", "warning", "error"};
            // -------------------------- BEGIN CONFIG GENERATIONS ----------------------------------------------------------------------------
            QJsonObject GenerateRoutes(bool enableProxy, bool proxyCN)
            {
                DROOT
                root.insert("domainStrategy", "IPIfNonMatch");
                //
                // For Rules list
                QJsonArray rulesList;

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

            QJsonObject GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type)
            {
                DROOT
                root.insert(isDomain ? "domain" : "ip", QJsonArray::fromStringList(list));
                JADD(outboundTag, type)
                RROOT
            }

            QJsonObject GenerateFreedomOUT(QString domainStrategy, QString redirect, int userLevel)
            {
                DROOT
                JADD(domainStrategy, redirect, userLevel)
                RROOT
            }
            QJsonObject GenerateBlackHoleOUT(bool useHTTP)
            {
                DROOT
                QJsonObject resp;
                resp.insert("type", useHTTP ? "http" : "none");
                root.insert("response", resp);
                RROOT
            }

            QJsonObject GenerateShadowSocksOUT(QList<QJsonObject> servers)
            {
                DROOT
                QJsonArray x;

                foreach (auto server, servers) {
                    x.append(server);
                }

                root.insert("servers", x);
                RROOT
            }

            QJsonObject GenerateShadowSocksServerOUT(QString email, QString address, int port, QString method, QString password, bool ota, int level)
            {
                DROOT
                JADD(email, address, port, method, password, level, ota)
                RROOT
            }

            QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers)
            {
                DROOT
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

            QJsonObject GenerateDokodemoIN(QString address, int port, QString  network, int timeout, bool followRedirect, int userLevel)
            {
                DROOT
                JADD(address, port, network, timeout, followRedirect, userLevel)
                RROOT
            }

            QJsonObject GenerateHTTPIN(QList<AccountObject> _accounts, int timeout, bool allowTransparent, int userLevel)
            {
                DROOT
                QJsonArray accounts;

                foreach (auto account, _accounts) {
                    accounts.append(GetRootObject(account));
                }

                JADD(timeout, accounts, allowTransparent, userLevel)
                RROOT
            }

            QJsonObject GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp, QString ip, int userLevel)
            {
                DROOT
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

            QJsonObject GenerateOutboundEntry(QString protocol, QJsonObject settings, QJsonObject streamSettings, QJsonObject mux, QString sendThrough, QString tag)
            {
                DROOT
                JADD(sendThrough, protocol, settings, tag, streamSettings, mux)
                RROOT
            }

            QJsonObject GenerateInboundEntry(QString listen, int port, QString protocol, QJsonObject settings, QString tag, QJsonObject sniffing, QJsonObject allocate)
            {
                DROOT
                LOG(MODULE_CONNECTION, "allocation is not used here.")
                Q_UNUSED(allocate)
                JADD(listen, port, protocol, settings, tag, sniffing)
                RROOT
            }

            QJsonObject GenerateAPIEntry(QString tag, bool withHandler, bool withLogger, bool withStats)
            {
                DROOT
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

            QJsonObject GenerateRuntimeConfig(QJsonObject root)
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

                foreach (auto str, gConf.dnsList) {
                    dnsList.append(QString::fromStdString(str));
                }

                auto dnsObject = GenerateDNS(gConf.withLocalDNS, dnsList);
                root.insert("dns", dnsObject);
                //
                //
                //
                QJsonArray inboundsList;

                // HTTP InBound
                if (gConf.inboundConfig.http_port != 0) {
                    QJsonObject httpInBoundObject;
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
                if (gConf.inboundConfig.socks_port != 0) {
                    QJsonObject socksInBoundObject;
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
                bool cRouting = root.contains("routing");
                bool cRule = cRouting && root["routing"].toObject().contains("rules");
                bool cRules = cRule && root["routing"].toObject()["rules"].toArray().count() > 0;

                if (!cRules) {
                    LOG(MODULE_CONNECTION, "Current connection has NO ROUTING section, we insert default values.")

                    if (root["outbounds"].toArray().count() != 1) {
                        // There are no ROUTING but 2 or more outbounds.... This is rare, but possible.
                        LOG(MODULE_CONNECTION, "WARN: This message usually indicates the config file has some logic errors:")
                        LOG(MODULE_CONNECTION, "WARN: --> The config file has NO routing section, however more than 1 outbounds are detected.")
                    }

                    auto routeObject = GenerateRoutes(gConf.enableProxy, gConf.bypassCN);
                    root.insert("routing", routeObject);
                    QJsonArray outbounds = root["outbounds"].toArray();
                    outbounds.append(GenerateOutboundEntry("freedom", GenerateFreedomOUT("AsIs", ":0", 0), QJsonObject(), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_DIRECT));
                    root["outbounds"] = outbounds;
                } else {
                    // For some config files that has routing entries already.
                    // We don't add extra routings.
                    //
                    // HOWEVER, we need to verify the QV2RAY_RULE_ENABLED entry.
                    // And what's more, process (by removing unused items) from a rule object.
                    QJsonObject routing = root["routing"].toObject();
                    QJsonArray rules;
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
                }

                // Let's process some api features.
                if (gConf.enableStats) {
                    // Stats
                    {
                        root.insert("stats", QJsonObject());
                    }
                    // Routes
                    {
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
                    }
                    // Policy
                    {
                        QJsonObject policyRoot = root.contains("policy") ? root["policy"].toObject() : QJsonObject();
                        QJsonObject systemPolicy = policyRoot.contains("system") ? policyRoot["system"].toObject() : QJsonObject();
                        systemPolicy["statsInboundUplink"] = true;
                        systemPolicy["statsInboundDownlink"] = true;
                        policyRoot["system"] = systemPolicy;
                        // Add this to root.
                        root["policy"] = policyRoot;
                    }
                    // Inbounds
                    {
                        QJsonArray inbounds = root["inbounds"].toArray();
                        QJsonObject fakeDocodemoDoor;
                        fakeDocodemoDoor["address"] = "127.0.0.1";
                        QJsonObject apiInboundsRoot = GenerateInboundEntry("127.0.0.1", gConf.statsPort, "dokodemo-door", fakeDocodemoDoor, API_TAG_INBOUND);
                        inbounds.push_front(apiInboundsRoot);
                        root["inbounds"] = inbounds;
                    }
                    // API
                    {
                        root["api"] = GenerateAPIEntry(API_TAG_DEFAULT);
                    }
                }

                return root;
            }
        }
    }
}
