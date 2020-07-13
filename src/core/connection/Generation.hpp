#pragma once
#include "base/Qv2rayBase.hpp"

static const inline QStringList V2RayLogLevel = { "none", "debug", "info", "warning", "error" };

namespace Qv2ray::core::connection::generation
{
    namespace routing
    {
        ROUTERULE GenerateSingleRouteRule(const QString &str, bool isDomain, const QString &outboundTag, const QString &type = "field");
        ROUTERULE GenerateSingleRouteRule(const QStringList &list, bool isDomain, const QString &outboundTag, const QString &type = "field");
        QJsonObject GenerateDNS(bool withLocalhost, const QvConfig_DNS &dnsServer);
    } // namespace routing

    namespace misc
    {
        QJsonObject GenerateAPIEntry(const QString &tag, bool withHandler = true, bool withLogger = true, bool withStats = true);
    } // namespace misc

    namespace inbounds
    {
        INBOUNDSETTING GenerateDokodemoIN(const QString &address, int port, const QString &network, int timeout, bool followRedirect,
                                          int userLevel);
        INBOUNDSETTING GenerateHTTPIN(const QList<AccountObject> &accounts, int timeout = 300, bool allowTransparent = true, int userLevel = 0);
        INBOUNDSETTING GenerateSocksIN(const QString &auth, const QList<AccountObject> &_accounts, bool udp = false,
                                       const QString &ip = "127.0.0.1", int userLevel = 0);
        INBOUND GenerateInboundEntry(const QString &listen, int port, const QString &protocol, const INBOUNDSETTING &settings,
                                     const QString &tag, const QJsonObject &sniffing = QJsonObject(),
                                     const QJsonObject &allocate = QJsonObject());
    } // namespace inbounds

    namespace outbounds
    {
        OUTBOUNDSETTING GenerateFreedomOUT(const QString &domainStrategy, const QString &redirect, int userLevel);
        OUTBOUNDSETTING GenerateBlackHoleOUT(bool useHTTP);
        OUTBOUNDSETTING GenerateShadowSocksOUT(const QList<ShadowSocksServerObject> &servers);
        OUTBOUNDSETTING GenerateShadowSocksServerOUT(const QString &email, const QString &address, int port, const QString &method,
                                                     const QString &password, bool ota, int level);
        OUTBOUNDSETTING GenerateHTTPSOCKSOut(const QString &address, int port, bool useAuth, const QString &username, const QString &password);
        OUTBOUND GenerateOutboundEntry(const QString &protocol, const OUTBOUNDSETTING &settings, const QJsonObject &streamSettings,
                                       const QJsonObject &mux = QJsonObject(), const QString &sendThrough = "0.0.0.0",
                                       const QString &tag = OUTBOUND_TAG_PROXY);
    } // namespace outbounds

    namespace filters
    {
        // mark all outbound
        void OutboundMarkSettingFilter(CONFIGROOT &root, const int mark);
        void RemoveEmptyMuxFilter(CONFIGROOT &root);
        void DNSInterceptFilter(CONFIGROOT &root, const bool have_tproxy, const bool have_tproxy_v6, const bool have_socks);
        void BypassBTFilter(CONFIGROOT &root);
        void mKCPSeedFilter(CONFIGROOT &root);
        void FillupTagsFilter(CONFIGROOT &root, const QString &subKey);
    } // namespace filters

} // namespace Qv2ray::core::connection::generation

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::generation;
using namespace Qv2ray::core::connection::generation::filters;
using namespace Qv2ray::core::connection::generation::inbounds;
using namespace Qv2ray::core::connection::generation::outbounds;
using namespace Qv2ray::core::connection::generation::routing;
using namespace Qv2ray::core::connection::generation::misc;
