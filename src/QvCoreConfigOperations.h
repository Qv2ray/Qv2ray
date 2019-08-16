#ifndef CONFIGGENERATION_H
#define CONFIGGENERATION_H

#include "QvUtils.h"
#include "QJsonObjectInsertMacros.h"

#define OUTBOUND_TAG_DIRECT "outBound_DIRECT"
#define OUTBOUND_TAG_PROXY "outBound_PROXY"

#define DROOT QJsonObject root;
#define RROOT return root;

#define JSON_ROOT_TRY_REMOVE(obj) if (root.contains(obj)) { root.remove(obj); }

namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG GENERATIONS ---------------------------------------------
        QJsonObject GenerateRoutes(bool globalProxy, bool cnProxy);
        QJsonObject GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type = "field");
        QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers);
        //
        // OutBoundProtocols
        QJsonObject GenerateFreedomOUT(QString domainStrategy, QString redirect, int userLevel);
        QJsonObject GenerateBlackHoleOUT(bool useHTTP);
        QJsonObject GenerateShadowSocksServerOUT(QString email, QString address, int port, QString method, QString password, bool ota, int level);
        QJsonObject GenerateShadowSocksOUT(QList<QJsonObject> servers);
        //
        // InBoundsProtocols
        QJsonObject GenerateDokodemoIN(QString address, int port, QString  network, int timeout, bool followRedirect, int userLevel);
        QJsonObject GenerateHTTPIN(QList<AccountObject> accounts, int timeout = 300, bool allowTransparent = true, int userLevel = 0);
        QJsonObject GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp = false, QString ip = "127.0.0.1", int userLevel = 0);
        //
        // Misc
        template<typename T>
        QJsonObject GetRootObject(T t)
        {
            auto json = StructToJSONString(t);
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json.toStdString()));
            return doc.object();
        }
        template QJsonObject GetRootObject<StreamSettingsObject>(StreamSettingsObject t);
        template QJsonObject GetRootObject<VMessServerObject>(VMessServerObject t);
        //
        // Generate FINAL Configs
        QJsonObject GenerateRuntimeConfig(QJsonObject root);
        QJsonObject GenerateOutboundEntry(QString protocol, QJsonObject settings, QJsonObject streamSettings, QJsonObject mux, QString sendThrough = "0.0.0.0", QString tag = "");

        //
        // -------------------------- BEGIN CONFIG VALIDATIONS ---------------------------------------------
        int VerifyVMessProtocolString(QString vmess);
        //
        // -------------------------- BEGIN CONFIG CONVERSIONS ---------------------------------------------
        // Save Connection Config
        bool SaveConnectionConfig(QJsonObject obj, const QString *alias);
        bool RenameConnection(QString originalName, QString newName);
        // VMess Protocol
        QJsonObject ConvertConfigFromVMessString(QString vmess);
        QJsonObject ConvertConfigFromFile(QString sourceFilePath, bool overrideInbounds);
        // Load Configs
        QMap<QString, QJsonObject> GetConnections(list<string> connections);
        // Startup Prepares
        int StartPreparation(QJsonObject fullConfig);

    }
}
using namespace Qv2ray::ConfigOperations;
#endif // CONFIGGENERATION_H
