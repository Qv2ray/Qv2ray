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

        template<typename T>
        QJsonObject GetRootObject(T out)
        {
            auto json = StructToJSON(out);
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json.toStdString()));
            return doc.object();
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

        QJsonObject GenerateHTTPIN(int timeout, QList<AccountObject> _accounts, bool allowTransparent, bool userLevel)
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

        // This generates an "OutBoundObject"
        QJsonObject ConvertOutboundFromVMessString(QString str)
        {
            DROOT
            QStringRef vmessJsonB64(&str, 8, str.length() - 8);
            auto vmessConf = StructFromJSON<VMessProtocolConfigObject>(Base64Decode(vmessJsonB64.toString()).toStdString());
            VMessOut vConf;
            VMessOut::ServerObject serv;
            serv.port = stoi(vmessConf.port);
            serv.address = vmessConf.add;
            // User
            VMessOut::ServerObject::UserObject user;
            user.id = vmessConf.id;
            user.alterId = stoi(vmessConf.aid);
            // Server
            serv.users.push_back(user);
            // VMess root config
            vConf.vnext.push_back(serv);
            //
            // Stream Settings
            StreamSettingsObject streaming;

            // Fill hosts for HTTP
            foreach (auto host, QString::fromStdString(vmessConf.host).split(',')) {
                streaming.httpSettings.host.push_back(host.toStdString());
            }

            // hosts for ws, h2 and security for QUIC
            streaming.wsSettings.headers.insert(make_pair("Host", vmessConf.host));
            streaming.quicSettings.security = vmessConf.host;
            //
            // Fake type for tcp, kcp and QUIC
            streaming.tcpSettings.header.type = vmessConf.type;
            streaming.kcpSettings.header.type = vmessConf.type;
            streaming.quicSettings.header.type = vmessConf.type;
            //
            // Path for ws, h2, Quic
            streaming.wsSettings.path = vmessConf.path;
            streaming.httpSettings.path = vmessConf.path;
            streaming.quicSettings.key = vmessConf.path;
            streaming.security = vmessConf.tls;
            //
            // Network type
            streaming.network = vmessConf.net;
            //
            // Root
            root.insert("sendThrough", "0.0.0.0");
            root.insert("protocol", "vmess");
            root.insert("settings", GetRootObject(vConf));
            root.insert("tag", OUTBOUND_TAG_PROXY);
            root.insert("streamSettings", GetRootObject(streaming));
            root.insert("QV2RAY_ALIAS", QString::fromStdString(vmessConf.ps));
            RROOT
        }

        // -------------------------- END CONFIG GENERATIONS ------------------------------------------------------------------------------
    }
}
