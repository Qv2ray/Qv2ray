#include "core/connection/Generation.hpp"

namespace Qv2ray::core::connection::generation::misc
{
    QJsonObject GenerateAPIEntry(const QString &tag, bool withHandler, bool withLogger, bool withStats)
    {
        QJsonObject root;
        QJsonArray services;

        services << "ReflectionService";

        if (withHandler)
            services << "HandlerService";

        if (withLogger)
            services << "LoggerService";

        if (withStats)
            services << "StatsService";

        JADD(services, tag)
        return root;
    }
} // namespace Qv2ray::core::connection::generation::misc
