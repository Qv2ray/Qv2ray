#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::inbounds
{

    INBOUNDSETTING GenerateDokodemoIN(const QString &address, int port, const QString &network, int timeout, bool followRedirect)
    {
        INBOUNDSETTING root;
        JADD(address, port, network, timeout, followRedirect)
        return root;
    }

    INBOUNDSETTING GenerateHTTPIN(bool enableAuth, const QList<AccountObject> &_accounts, int timeout, bool allowTransparent)
    {
        INBOUNDSETTING root;
        QJsonArray accounts;

        for (const auto &account : _accounts)
        {
            if (account.user.isEmpty() && account.pass.isEmpty())
                continue;
            accounts.append(account.toJson());
        }

        if (enableAuth && !accounts.isEmpty())
            JADD(accounts)

        JADD(timeout, allowTransparent)
        return root;
    }

    INBOUNDSETTING GenerateSocksIN(const QString &auth, const QList<AccountObject> &_accounts, bool udp, const QString &ip)
    {
        INBOUNDSETTING root;
        QJsonArray accounts;
        for (const auto &acc : _accounts)
        {
            if (acc.user.isEmpty() && acc.pass.isEmpty())
                continue;
            accounts.append(acc.toJson());
        }

        if (!accounts.isEmpty())
            JADD(accounts)

        if (udp)
        {
            JADD(auth, udp, ip)
        }
        else
        {
            JADD(auth)
        }
        return root;
    }

    INBOUND GenerateInboundEntry(const QString &tag, const QString &protocol, const QString &listen, int port, const INBOUNDSETTING &settings,
                                 const QJsonObject &sniffing, const QJsonObject &allocate)
    {
        INBOUND root;
        DEBUG(MODULE_CONNECTION, "Allocation is not used here, Not Implemented")
        Q_UNUSED(allocate)
        JADD(listen, port, protocol, settings, tag, sniffing)
        return root;
    }

    INBOUNDS GenerateDefaultInbounds()
    {
#define INCONF GlobalConfig.inboundConfig
        INBOUNDS inboundsList;
        const static QJsonObject sniffingOff{ { "enabled", false } };
        const static QJsonObject sniffingOn{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };

        // HTTP Inbound
        if (GlobalConfig.inboundConfig.useHTTP)
        {
            const auto httpInSettings = GenerateHTTPIN(INCONF.httpSettings.useAuth, { INCONF.httpSettings.account });
            const auto httpInboundObject = GenerateInboundEntry("http_IN", "http",        //
                                                                INCONF.listenip,          //
                                                                INCONF.httpSettings.port, //
                                                                httpInSettings,           //
                                                                { INCONF.httpSettings.sniffing ? sniffingOn : sniffingOff });
            inboundsList.append(httpInboundObject);
        }

        // SOCKS Inbound
        if (INCONF.useSocks)
        {
            const auto socksInSettings = GenerateSocksIN(INCONF.socksSettings.useAuth ? "password" : "noauth", //
                                                         { INCONF.socksSettings.account },                     //
                                                         INCONF.socksSettings.enableUDP,                       //
                                                         INCONF.socksSettings.localIP);
            const auto socksInboundObject = GenerateInboundEntry("socks_IN", "socks",       //
                                                                 INCONF.listenip,           //
                                                                 INCONF.socksSettings.port, //
                                                                 socksInSettings,           //
                                                                 { INCONF.socksSettings.sniffing ? sniffingOn : sniffingOff });
            inboundsList.append(socksInboundObject);
        }

        // TPROXY
        if (INCONF.useTPROXY)
        {
            QList<QString> networks;
            if (INCONF.tProxySettings.hasTCP)
                networks << "tcp";
            if (INCONF.tProxySettings.hasUDP)
                networks << "udp";
            const auto tproxy_network = networks.join(",");
            const auto tProxySettings = GenerateDokodemoIN("", 0, tproxy_network, 0, true);
            const static QJsonObject sniffingSettings = { { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };
            // tProxy IPv4 Settings
            {
                LOG(MODULE_CONNECTION, "Processing tProxy IPv4 inbound")
                auto tProxyIn = GenerateInboundEntry("tproxy_IN", "dokodemo-door",   //
                                                     INCONF.tProxySettings.tProxyIP, //
                                                     INCONF.tProxySettings.port,     //
                                                     tProxySettings,                 //
                                                     sniffingSettings);
                tProxyIn.insert("streamSettings", QJsonObject{ { "sockopt", QJsonObject{ { "tproxy", INCONF.tProxySettings.mode } } } });
                inboundsList.append(tProxyIn);
            }
            if (!INCONF.tProxySettings.tProxyV6IP.isEmpty())
            {
                LOG(MODULE_CONNECTION, "Processing tProxy IPv6 inbound")
                auto tProxyIn = GenerateInboundEntry("tproxy_IN_V6", "dokodemo-door",  //
                                                     INCONF.tProxySettings.tProxyV6IP, //
                                                     INCONF.tProxySettings.port,       //
                                                     tProxySettings,                   //
                                                     sniffingSettings);
                tProxyIn.insert("streamSettings", QJsonObject{ { "sockopt", QJsonObject{ { "tproxy", INCONF.tProxySettings.mode } } } });
                inboundsList.append(tProxyIn);
            }
        }

#undef INCONF
        return inboundsList;
    }
} // namespace Qv2ray::core::connection::generation::inbounds
