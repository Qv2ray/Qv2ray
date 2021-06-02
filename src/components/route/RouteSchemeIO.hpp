#pragma once
#include "base/Qv2rayBase.hpp"
namespace Qv2ray::components::route
{
    const inline RouteConfig emptyScheme;
    const inline RouteConfig noAdsScheme({ {}, { "geosite:category-ads-all" }, {} }, { {}, {}, {} }, "AsIs");

    /**
     * @brief The Qv2rayRouteScheme struct
     * @author DuckSoft <realducksoft@gmail.com>
     */
    struct Qv2rayRouteScheme : RouteConfig
    {
        Q_OBJECT

        // M: all these fields are mandatory
        QJS_FUNCTION(Qv2rayRouteScheme, F(name, author, description), B(RouteConfig))
        /**
         * @brief the name of the scheme.
         * @example "Untitled Scheme"
         */
        QJS_PROP(QString, name, REQUIRED)
        /**
         * @brief the author of the scheme.
         * @example "DuckSoft <realducksoft@gmail.com>"
         */
        QJS_PROP(QString, author, REQUIRED)
        /**
         * @brief details of this scheme.
         * @example "A scheme to bypass China mainland, while allowing bilibili to go through proxy."
         */
        QJS_PROP(QString, description, REQUIRED)
    };
} // namespace Qv2ray::components::route

using namespace Qv2ray::components::route;
