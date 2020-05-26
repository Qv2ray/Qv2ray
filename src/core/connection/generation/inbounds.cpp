#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::inbounds
{

    INBOUNDSETTING GenerateDokodemoIN(const QString &address, int port, const QString &network, int timeout, bool followRedirect, int userLevel)
    {
        INBOUNDSETTING root;
        JADD(address, port, network, timeout, followRedirect, userLevel)
        return root;
    }

    INBOUNDSETTING GenerateHTTPIN(const QList<AccountObject> &_accounts, int timeout, bool allowTransparent, int userLevel)
    {
        INBOUNDSETTING root;
        QJsonArray accounts;

        for (const auto &account : _accounts)
        {
            if (account.user.isEmpty() && account.pass.isEmpty())
                continue;
            accounts.append(account.toJson());
        }

        if (!accounts.isEmpty())
            JADD(accounts)

        JADD(timeout, allowTransparent, userLevel)
        return root;
    }

    INBOUNDSETTING GenerateSocksIN(const QString &auth, const QList<AccountObject> &_accounts, bool udp, const QString &ip, int userLevel)
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
            JADD(auth, udp, ip, userLevel)
        }
        else
        {
            JADD(auth, userLevel)
        }
        return root;
    }

    INBOUND GenerateInboundEntry(const QString &listen, int port, const QString &protocol, const INBOUNDSETTING &settings, const QString &tag,
                                 const QJsonObject &sniffing, const QJsonObject &allocate)
    {
        INBOUND root;
        DEBUG(MODULE_CONNECTION, "Allocation is not used here, Not Implemented")
        Q_UNUSED(allocate)
        JADD(listen, port, protocol, settings, tag, sniffing)
        return root;
    }
} // namespace Qv2ray::core::connection::generation::inbounds
