#pragma once

#include "base/Qv2rayBase.hpp"

#define OUTBOUND_TAG_DIRECT "outBound_DIRECT"
#define OUTBOUND_TAG_PROXY "outBound_PROXY"
#define OUTBOUND_TAG_FORWARD_PROXY "_QV2RAY_FORWARD_PROXY_"

#define API_TAG_DEFAULT "_QV2RAY_API_"
#define API_TAG_INBOUND "_QV2RAY_API_INBOUND_"

#define QV2RAY_USE_FPROXY_KEY "_QV2RAY_USE_GLOBAL_FORWARD_PROXY_"

#define JSON_ROOT_TRY_REMOVE(obj) if (root.contains(obj)) { root.remove(obj); }

namespace Qv2ray::core::connection
{
    // -------------------------- BEGIN GENERAL FUNCTIONS ----------------------------------------------

    QMap<QString, CONFIGROOT> GetRegularConnections(QStringList connections);
    QMap<QString, CONFIGROOT> GetSubscriptionConnection(QString subscription);
    QMap<QString, QMap<QString, CONFIGROOT>> GetSubscriptionConnections(QStringList subscriptions);
    bool CheckIsComplexConfig(CONFIGROOT root);

    //
    // -------------------------- BEGIN CONFIG CONVERSIONS --------------------------
    inline namespace Convertion
    {
        //int VerifyVMessProtocolString(QString vmess);
        QString DecodeSubscriptionString(QByteArray arr);
        //
        // Save Connection Config
        bool SaveConnectionConfig(CONFIGROOT obj, QString *alias, bool canOverrideExisting);
        bool SaveSubscriptionConfig(CONFIGROOT obj, const QString &subscription, QString *name);
        bool RemoveConnection(const QString &alias);
        bool RemoveSubscriptionConnection(const QString &subsName, const QString &name);
        bool RenameConnection(const QString &originalName, const QString &newName);
        bool RenameSubscription(const QString &originalName, const QString &newName);

        // VMess URI Protocol
        CONFIGROOT ConvertConfigFromVMessString(const QString &vmess, QString *alias, QString *errMessage);
        CONFIGROOT ConvertConfigFromFile(QString sourceFilePath, bool keepInbounds);
        QString ConvertConfigToVMessString(const StreamSettingsObject &transfer, const VMessServerObject &serverConfig, const QString &alias);
    }

    //
    // -------------------------- BEGIN CONFIG GENERATIONS ---------------------------------------------
    inline namespace Generation
    {
        ROUTING GenerateRoutes(bool enableProxy, bool cnProxy);
        ROUTERULE GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type = "field");
        QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers);
        QJsonObject GenerateAPIEntry(QString tag, bool withHandler = true, bool withLogger = true, bool withStats = true);
        //
        // Outbound Protocols
        OUTBOUNDSETTING GenerateFreedomOUT(QString domainStrategy, QString redirect, int userLevel);
        OUTBOUNDSETTING GenerateBlackHoleOUT(bool useHTTP);
        OUTBOUNDSETTING GenerateShadowSocksServerOUT(QString email, QString address, int port, QString method, QString password, bool ota, int level);
        OUTBOUNDSETTING GenerateShadowSocksOUT(QList<QJsonObject> servers);
        OUTBOUNDSETTING GenerateHTTPSOCKSOut(QString address, int port, bool useAuth, QString username, QString password);
        //
        // Inbounds Protocols
        INBOUNDSETTING GenerateDokodemoIN(QString address, int port, QString  network, int timeout, bool followRedirect, int userLevel);
        INBOUNDSETTING GenerateHTTPIN(QList<AccountObject> accounts, int timeout = 300, bool allowTransparent = true, int userLevel = 0);
        INBOUNDSETTING GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp = false, QString ip = "127.0.0.1", int userLevel = 0);
        //
        // Generate FINAL Configs
        CONFIGROOT GenerateRuntimeConfig(CONFIGROOT root);
        OUTBOUND GenerateOutboundEntry(QString protocol, OUTBOUNDSETTING settings, QJsonObject streamSettings, QJsonObject mux = QJsonObject(), QString sendThrough = "0.0.0.0", QString tag = "");
        INBOUND GenerateInboundEntry(QString listen, int port, QString protocol, INBOUNDSETTING settings, QString tag, QJsonObject sniffing = QJsonObject(), QJsonObject allocate = QJsonObject());
    }
}


using namespace Qv2ray::core::connection;
