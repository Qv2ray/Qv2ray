#include "QvCoreConfigOperations.h"

namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG GENERATIONS ----------------------------------------------------------------------------
        QJsonObject GenerateRoutes(bool globalProxy, bool cnProxy)
        {
            DROOT
            root.insert("domainStrategy", "IPIfNonMatch");
            //
            // For Rules list
            QJsonArray rulesList;
            //
            // Private IPs should always NOT TO PROXY!
            rulesList.append(GenerateSingleRouteRule(QStringList({"geoip:private"}), false, OUTBOUND_TAG_DIRECT));
            //
            // Check if CN needs proxy, or direct.
            rulesList.append(GenerateSingleRouteRule(QStringList({"geoip:cn"}), false, cnProxy ? OUTBOUND_TAG_PROXY : OUTBOUND_TAG_DIRECT));
            rulesList.append(GenerateSingleRouteRule(QStringList({"geosite:cn"}), true, cnProxy ? OUTBOUND_TAG_PROXY :  OUTBOUND_TAG_DIRECT));
            //
            // Check global proxy, or direct.
            rulesList.append(GenerateSingleRouteRule(QStringList({"regexp:.*"}), true, globalProxy ? OUTBOUND_TAG_PROXY :  OUTBOUND_TAG_DIRECT));
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

            JADD(auth, accounts, udp, ip, userLevel)
            RROOT
        }

        QJsonObject GenerateOutboundEntry(QString protocol, QJsonObject settings, QJsonObject streamSettings, QJsonObject mux, QString sendThrough, QString tag)
        {
            DROOT
            JADD(sendThrough, protocol, settings, tag, streamSettings, mux)
            RROOT
        }

        // -------------------------- END CONFIG GENERATIONS ------------------------------------------------------------------------------
        // BEGIN RUNTIME CONFIG GENERATION

        QJsonObject GenerateRuntimeConfig(QJsonObject root)
        {
            auto gConf = GetGlobalConfig();
            QJsonObject logObject;
            //logObject.insert("access", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ACCESS_LOG_FILENAME);
            //logObject.insert("error", QV2RAY_CONFIG_PATH + QV2RAY_VCORE_LOG_DIRNAME + QV2RAY_VCORE_ERROR_LOG_FILENAME);
            QString logLevel_s;

            switch (gConf.logLevel) {
                case 0:
                    logLevel_s  = "none";
                    break;

                case 1:
                    logLevel_s  = "debug";
                    break;

                case 2:
                    logLevel_s  = "info";
                    break;

                case 3:
                    logLevel_s  = "warning";
                    break;

                case 4:
                    logLevel_s  = "error";
                    break;
            }

            logObject.insert("loglevel", logLevel_s);
            root.insert("log", logObject);
            //
            QStringList dnsList;

            foreach (auto str, gConf.dnsList) {
                dnsList.append(QString::fromStdString(str));
            }

            auto dnsObject = GenerateDNS(gConf.withLocalDNS, dnsList);
            root.insert("dns", dnsObject);

            //
            // This is for imported config files as there are routing entries already.
            // We don't add extra routings.
            // We don't use QV2RAY_CONFIG_TYPE_FILE checking scheme because not all connections have this part.
            if (!root.contains("routing")) {
                auto routeObject = GenerateRoutes(gConf.proxyDefault, gConf.bypassCN);
                root.insert("routing", routeObject);
            }

            //
            //
            root.insert("stats", QJsonObject());
            //
            QJsonArray inboundsList;

            // HTTP InBound
            if (gConf.inBoundSettings.http_port != 0) {
                QJsonObject httpInBoundObject;
                httpInBoundObject.insert("listen", QString::fromStdString(gConf.inBoundSettings.listenip));
                httpInBoundObject.insert("port", gConf.inBoundSettings.http_port);
                httpInBoundObject.insert("protocol", "http");
                httpInBoundObject.insert("tag", "http_IN");

                if (gConf.inBoundSettings.http_useAuth) {
                    auto httpInSettings =  GenerateHTTPIN(QList<AccountObject>() << gConf.inBoundSettings.httpAccount);
                    httpInBoundObject.insert("settings", httpInSettings);
                }

                inboundsList.append(httpInBoundObject);
            }

            // SOCKS InBound
            if (gConf.inBoundSettings.socks_port != 0) {
                QJsonObject socksInBoundObject;
                socksInBoundObject.insert("listen", QString::fromStdString(gConf.inBoundSettings.listenip));
                socksInBoundObject.insert("port", gConf.inBoundSettings.socks_port);
                socksInBoundObject.insert("protocol", "socks");
                socksInBoundObject.insert("tag", "socks_IN");
                auto socksInSettings = GenerateSocksIN(gConf.inBoundSettings.socks_useAuth ? "password" : "noauth", QList<AccountObject>() << gConf.inBoundSettings.socksAccount);
                socksInBoundObject.insert("settings", socksInSettings);
                inboundsList.append(socksInBoundObject);
            }

            if (!root.contains("inbounds") || root["inbounds"].toArray().count() == 0) {
                root.insert("inbounds", inboundsList);
            }

            // TODO: MultiOutbound Settings
            if (root.contains(QV2RAY_CONFIG_TYPE_JSON_KEY) && root[QV2RAY_CONFIG_TYPE_JSON_KEY] == QV2RAY_CONFIG_TYPE_FILE) {
                LOG(MODULE_CONFIG, "Found an imported config file, skipping adding 'freedom' outbound.")
                // Do nothing because it's an imported connection.
            } else {
                QJsonArray outbounds = root["outbounds"].toArray();
                // It's not imported so we add new stuff.
                // For DIRECT
                outbounds.append(GenerateOutboundEntry("freedom", GenerateFreedomOUT("AsIs", ":0", 0), QJsonObject(), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_DIRECT));
                QJsonObject first = outbounds.first().toObject();
                first.insert("mux", GetRootObject(gConf.mux));
                outbounds[0] = first;
                root["outbounds"] = outbounds;
            }

            return root;
        }
    }
}
