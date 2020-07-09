#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::outbounds
{

    OUTBOUNDSETTING GenerateFreedomOUT(const QString &domainStrategy, const QString &redirect, int userLevel)
    {
        OUTBOUNDSETTING root;
        JADD(domainStrategy, redirect, userLevel)
        return root;
    }
    OUTBOUNDSETTING GenerateBlackHoleOUT(bool useHTTP)
    {
        OUTBOUNDSETTING root;
        QJsonObject resp;
        resp.insert("type", useHTTP ? "http" : "none");
        root.insert("response", resp);
        return root;
    }

    OUTBOUNDSETTING GenerateShadowSocksOUT(const QList<ShadowSocksServerObject> &_servers)
    {
        OUTBOUNDSETTING root;
        QJsonArray x;

        for (const auto &server : _servers)
        {
            x.append(GenerateShadowSocksServerOUT(server.email, server.address, server.port, server.method, //
                                                  server.password, server.ota, server.level));
        }

        root.insert("servers", x);
        return root;
    }

    OUTBOUNDSETTING GenerateShadowSocksServerOUT(const QString &email, const QString &address, int port, const QString &method,
                                                 const QString &password, bool ota, int level)
    {
        OUTBOUNDSETTING root;
        JADD(email, address, port, method, password, level, ota)
        return root;
    }

    OUTBOUNDSETTING GenerateHTTPSOCKSOut(const QString &addr, int port, bool useAuth, const QString &username, const QString &password)
    {
        OUTBOUNDSETTING root;
        QJsonIO::SetValue(root, addr, "servers", 0, "address");
        QJsonIO::SetValue(root, port, "servers", 0, "port");
        if (useAuth)
        {
            QJsonIO::SetValue(root, username, "servers", 0, "users", 0, "user");
            QJsonIO::SetValue(root, password, "servers", 0, "users", 0, "pass");
        }
        return root;
    }

    OUTBOUND GenerateOutboundEntry(const QString &protocol, const OUTBOUNDSETTING &settings, const QJsonObject &streamSettings,
                                   const QJsonObject &mux, const QString &sendThrough, const QString &tag)
    {
        OUTBOUND root;
        JADD(sendThrough, protocol, settings, tag, streamSettings, mux)
        return root;
    }
} // namespace Qv2ray::core::connection::generation::outbounds
