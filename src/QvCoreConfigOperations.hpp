#ifndef CONFIGGENERATION_H
#define CONFIGGENERATION_H

#include "QvUtils.hpp"
#include "QJsonObjectInsertMacros.h"

#define OUTBOUND_TAG_DIRECT "outBound_DIRECT"
#define OUTBOUND_TAG_PROXY "outBound_PROXY"
#define API_TAG_DEFAULT "_QV2RAY_API_"
#define API_TAG_INBOUND "_QV2RAY_API_INBOUND_"

#define JSON_ROOT_TRY_REMOVE(obj) if (root.contains(obj)) { root.remove(obj); }

namespace Qv2ray
{
    // -------------------------- BEGIN GENERAL FUNCTIONS ----------------------------------------------
    namespace ConfigOperations
    {
        QMap<QString, QJsonObject> GetConnections(list<string> connections);
        int StartPreparation(QJsonObject fullConfig);
        int FindIndexByTag(QJsonArray list, QString *tag);

        //
        // -------------------------- BEGIN CONFIG CONVERSIONS --------------------------
        namespace Conversion
        {
            //int VerifyVMessProtocolString(QString vmess);
            QString DecodeSubscriptionString(QByteArray arr);
            //
            // Save Connection Config
            bool SaveConnectionConfig(QJsonObject obj, QString *alias, bool canOverrideExisting);
            bool RemoveConnection(const QString &alias);
            bool RenameConnection(QString originalName, QString newName);
            // VMess URI Protocol
            QJsonObject ConvertConfigFromVMessString(QString vmess, QString *alias, QString *errMessage);
            QJsonObject ConvertConfigFromFile(QString sourceFilePath, bool overrideInbounds);
        }

        //
        // -------------------------- BEGIN CONFIG GENERATIONS ---------------------------------------------
        namespace Generation
        {
            QJsonObject GenerateRoutes(bool enableProxy, bool cnProxy);
            QJsonObject GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type = "field");
            QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers);
            QJsonObject GenerateAPIEntry(QString tag, bool withHandler = true, bool withLogger = true, bool withStats = true);
            //
            // Outbound Protocols
            QJsonObject GenerateFreedomOUT(QString domainStrategy, QString redirect, int userLevel);
            QJsonObject GenerateBlackHoleOUT(bool useHTTP);
            QJsonObject GenerateShadowSocksServerOUT(QString email, QString address, int port, QString method, QString password, bool ota, int level);
            QJsonObject GenerateShadowSocksOUT(QList<QJsonObject> servers);
            //
            // Inbounds Protocols
            QJsonObject GenerateDokodemoIN(QString address, int port, QString  network, int timeout, bool followRedirect, int userLevel);
            QJsonObject GenerateHTTPIN(QList<AccountObject> accounts, int timeout = 300, bool allowTransparent = true, int userLevel = 0);
            QJsonObject GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp = false, QString ip = "127.0.0.1", int userLevel = 0);
            //
            // Generate FINAL Configs
            QJsonObject GenerateRuntimeConfig(QJsonObject root);
            QJsonObject GenerateOutboundEntry(QString protocol, QJsonObject settings, QJsonObject streamSettings, QJsonObject mux = QJsonObject(), QString sendThrough = "0.0.0.0", QString tag = "");
            QJsonObject GenerateInboundEntry(QString listen, int port, QString protocol, QJsonObject settings, QString tag, QJsonObject sniffing = QJsonObject(), QJsonObject allocate = QJsonObject());
        }
    }
}

using namespace Qv2ray::ConfigOperations;
using namespace Qv2ray::ConfigOperations::Conversion;
using namespace Qv2ray::ConfigOperations::Generation;

#endif // CONFIGGENERATION_H
