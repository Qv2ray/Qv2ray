#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::connection
{
    namespace Generation
    {
        // Important config generation algorithms.
        const QStringList vLogLevels = { "none", "debug", "info", "warning", "error" };
        ROUTING GenerateRoutes(bool enableProxy, bool bypassCN, const QString &defaultOutboundTag);
        ROUTERULE GenerateSingleRouteRule(QString str, bool isDomain, QString outboundTag, QString type = "field");
        ROUTERULE GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type = "field");
        QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers);
        QJsonObject GenerateAPIEntry(QString tag, bool withHandler = true, bool withLogger = true, bool withStats = true);
        //
        // Outbound Protocols
        OUTBOUNDSETTING GenerateFreedomOUT(QString domainStrategy, QString redirect, int userLevel);
        OUTBOUNDSETTING GenerateBlackHoleOUT(bool useHTTP);
        OUTBOUNDSETTING GenerateShadowSocksOUT(QList<ShadowSocksServerObject> servers);
        OUTBOUNDSETTING GenerateShadowSocksServerOUT(QString email, QString address, int port, QString method, QString password, bool ota,
                                                     int level);
        OUTBOUNDSETTING GenerateHTTPSOCKSOut(QString address, int port, bool useAuth, QString username, QString password);
        //
        // Inbounds Protocols
        INBOUNDSETTING GenerateDokodemoIN(QString address, int port, QString network, int timeout, bool followRedirect, int userLevel);
        INBOUNDSETTING GenerateHTTPIN(QList<AccountObject> accounts, int timeout = 300, bool allowTransparent = true, int userLevel = 0);
        INBOUNDSETTING GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp = false, QString ip = "127.0.0.1",
                                       int userLevel = 0);
        //
        // Generate FINAL Configs
        CONFIGROOT GenerateRuntimeConfig(CONFIGROOT root);
        OUTBOUND GenerateOutboundEntry(QString protocol, OUTBOUNDSETTING settings, QJsonObject streamSettings, QJsonObject mux = QJsonObject(),
                                       QString sendThrough = "0.0.0.0", QString tag = OUTBOUND_TAG_PROXY);
        INBOUND GenerateInboundEntry(QString listen, int port, QString protocol, INBOUNDSETTING settings, QString tag,
                                     QJsonObject sniffing = QJsonObject(), QJsonObject allocate = QJsonObject());
    } // namespace Generation
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::Generation;
