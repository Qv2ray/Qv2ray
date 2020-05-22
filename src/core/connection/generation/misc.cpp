#include "core/connection/Generation.hpp"

namespace Qv2ray::core::connection::generation::misc
{

    QJsonObject GenerateDNS(bool withLocalhost, const QStringList &dnsServers)
    {
        QJsonObject root;
        QJsonArray servers(QJsonArray::fromStringList(dnsServers));

        if (withLocalhost)
        {
            // https://github.com/Qv2ray/Qv2ray/issues/64
            // The fix patch didn't touch this line below.
            //
            // Should we APPEND localhost or PUSH_FRONT localhost?
            servers.append("localhost");
        }

        root.insert("servers", servers);
        RROOT
    }

    QJsonObject GenerateAPIEntry(const QString &tag, bool withHandler, bool withLogger, bool withStats)
    {
        QJsonObject root;
        QJsonArray services;

        if (withHandler)
            services << "HandlerService";

        if (withLogger)
            services << "LoggerService";

        if (withStats)
            services << "StatsService";

        JADD(services, tag)
        RROOT
    }
} // namespace Qv2ray::core::connection::generation::misc
