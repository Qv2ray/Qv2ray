#include "core/connection/Generation.hpp"
namespace Qv2ray::core::connection::generation::outbounds
{

    OUTBOUNDSETTING GenerateFreedomOUT(const QString &domainStrategy, const QString &redirect, int userLevel)
    {
        OUTBOUNDSETTING root;
        JADD(domainStrategy, redirect, userLevel)
        RROOT
    }
    OUTBOUNDSETTING GenerateBlackHoleOUT(bool useHTTP)
    {
        OUTBOUNDSETTING root;
        QJsonObject resp;
        resp.insert("type", useHTTP ? "http" : "none");
        root.insert("response", resp);
        RROOT
    }

    OUTBOUNDSETTING GenerateShadowSocksOUT(const QList<ShadowSocksServerObject> &servers)
    {
        OUTBOUNDSETTING root;
        QJsonArray x;

        for (auto server : servers)
        {
            x.append(GenerateShadowSocksServerOUT(server.email, server.address, server.port, server.method, server.password, server.ota,
                                                  server.level));
        }

        root.insert("servers", x);
        RROOT
    }

    OUTBOUNDSETTING GenerateShadowSocksServerOUT(const QString &email, const QString &address, int port, const QString &method,
                                                 const QString &password, bool ota, int level)
    {
        OUTBOUNDSETTING root;
        JADD(email, address, port, method, password, level, ota)
        RROOT
    }

    OUTBOUNDSETTING GenerateHTTPSOCKSOut(const QString &address, int port, bool useAuth, const QString &username, const QString &password)
    {
        OUTBOUNDSETTING root;
        QJsonArray servers;
        {
            QJsonObject oneServer;
            oneServer["address"] = address;
            oneServer["port"] = port;

            if (useAuth)
            {
                QJsonArray users;
                QJsonObject oneUser;
                oneUser["user"] = username;
                oneUser["pass"] = password;
                users.push_back(oneUser);
                oneServer["users"] = users;
            }

            servers.push_back(oneServer);
        }
        JADD(servers)
        RROOT
    }

    OUTBOUND GenerateOutboundEntry(const QString &protocol, const OUTBOUNDSETTING &settings, const QJsonObject &streamSettings,
                                   const QJsonObject &mux, const QString &sendThrough, const QString &tag)
    {
        OUTBOUND root;
        JADD(sendThrough, protocol, settings, tag, streamSettings, mux)
        RROOT
    }
} // namespace Qv2ray::core::connection::generation::outbounds
