#ifndef CONFIGGENERATION_H
#define CONFIGGENERATION_H

#include "QvUtils.h"
#include "QJsonObjectInsertMacros.h"

#define OUTBOUND_TAG_DIRECT "outBound_DIRECT"
#define OUTBOUND_TAG_PROXY "outBound_PROXY"
#define API_TAG_DEFAULT "_QV2RAY_API_"
#define API_TAG_INBOUND "_QV2RAY_API_INBOUND_"

#define DROOT QJsonObject root;
#define RROOT return root;

#define JSON_ROOT_TRY_REMOVE(obj) if (root.contains(obj)) { root.remove(obj); }

namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG GENERATIONS ---------------------------------------------
        QJsonObject GenerateRoutes(bool enableProxy, bool cnProxy);
        QJsonObject GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type = "field");
        QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers);
        QJsonObject GenerateAPIEntry(QString tag, bool withHandler = true, bool withLogger = true, bool withStats = true);
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
            auto json = StructToJsonString(t);
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json.toStdString()));
            return doc.object();
        }
        template QJsonObject GetRootObject<RuleObject>(RuleObject t);
        template QJsonObject GetRootObject<StreamSettingsObject>(StreamSettingsObject t);
        template QJsonObject GetRootObject<VMessServerObject>(VMessServerObject t);
        //
        // Generate FINAL Configs
        QJsonObject GenerateRuntimeConfig(QJsonObject root);
        QJsonObject GenerateOutboundEntry(QString protocol, QJsonObject settings, QJsonObject streamSettings, QJsonObject mux = QJsonObject(), QString sendThrough = "0.0.0.0", QString tag = "");
        QJsonObject GenerateInboundEntry(QString listen, int port, QString protocol, QJsonObject settings, QString tag, QJsonObject sniffing = QJsonObject(), QJsonObject allocate = QJsonObject());
        //
        // -------------------------- BEGIN CONFIG VALIDATIONS ---------------------------------------------
        int VerifyVMessProtocolString(QString vmess);
        QString GetVmessFromBase64OrPlain(QByteArray arr);
        //
        // -------------------------- BEGIN CONFIG CONVERSIONS ---------------------------------------------
        // Save Connection Config
        void DeducePossibleFileName(const QString &baseDir, QString *fileName, const QString &extension);
        bool SaveConnectionConfig(QJsonObject obj, QString *alias, bool canOverrideExisting);
        bool RemoveConnection(const QString &alias);
        bool RenameConnection(QString originalName, QString newName);
        // VMess Protocol
        QJsonObject ConvertConfigFromVMessString(QString vmess);
        QJsonObject ConvertConfigFromFile(QString sourceFilePath, bool overrideInbounds);
        // Load Configs
        QMap<QString, QJsonObject> GetConnections(list<string> connections);
        // Startup Prepares
        int StartPreparation(QJsonObject fullConfig);
        int FindIndexByTag(QJsonArray list, QString *tag);

    }
}
using namespace Qv2ray::ConfigOperations;
#endif // CONFIGGENERATION_H
