#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::connection
{
    namespace Generation
    {
        // Important config generation algorithms.
        const QStringList vLogLevels = { "none", "debug", "info", "warning", "error" };
        ROUTING GenerateRoutes(bool enableProxy, bool bypassCN, const QString &defaultOutboundTag);
        ROUTERULE GenerateSingleRouteRule(const QString &str, bool isDomain, const QString &outboundTag, const QString &type = "field");
        ROUTERULE GenerateSingleRouteRule(const QStringList &list, bool isDomain, const QString &outboundTag, const QString &type = "field");
        QJsonObject GenerateDNS(bool withLocalhost, const QStringList &dnsServers);
        QJsonObject GenerateAPIEntry(const QString &tag, bool withHandler = true, bool withLogger = true, bool withStats = true);
        //
        // Outbound Protocols
        OUTBOUNDSETTING GenerateFreedomOUT(const QString &domainStrategy, const QString &redirect, int userLevel);
        OUTBOUNDSETTING GenerateBlackHoleOUT(bool useHTTP);
        OUTBOUNDSETTING GenerateShadowSocksOUT(const QList<ShadowSocksServerObject> &servers);
        OUTBOUNDSETTING GenerateShadowSocksServerOUT(const QString &email, const QString &address, int port, const QString &method,
                                                     const QString &password, bool ota, int level);
        OUTBOUNDSETTING GenerateHTTPSOCKSOut(const QString &address, int port, bool useAuth, const QString &username, const QString &password);
        //
        // Inbounds Protocols
        INBOUNDSETTING GenerateDokodemoIN(const QString &address, int port, const QString &network, int timeout, bool followRedirect,
                                          int userLevel);
        INBOUNDSETTING GenerateHTTPIN(const QList<AccountObject> &accounts, int timeout = 300, bool allowTransparent = true, int userLevel = 0);
        INBOUNDSETTING GenerateSocksIN(const QString &auth, const QList<AccountObject> &_accounts, bool udp = false,
                                       const QString &ip = "127.0.0.1", int userLevel = 0);
        //
        // Generate FINAL Configs
        CONFIGROOT GenerateRuntimeConfig(CONFIGROOT root);
        OUTBOUND GenerateOutboundEntry(const QString &protocol, const OUTBOUNDSETTING &settings, const QJsonObject &streamSettings,
                                       const QJsonObject &mux = QJsonObject(), const QString &sendThrough = "0.0.0.0",
                                       const QString &tag = OUTBOUND_TAG_PROXY);
        INBOUND GenerateInboundEntry(const QString &listen, int port, const QString &protocol, const INBOUNDSETTING &settings,
                                     const QString &tag, const QJsonObject &sniffing = QJsonObject(),
                                     const QJsonObject &allocate = QJsonObject());
    } // namespace Generation
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::Generation;
