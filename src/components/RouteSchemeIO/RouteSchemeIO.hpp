#pragma once

#include "plugins/PluginsCommon/V2RayModels.hpp"

#include <QObject>

namespace Qv2ray::components::RouteSchemeIO
{
    const static inline Qv2ray::Models::RouteMatrixConfig EmptyScheme;
    const static inline Qv2ray::Models::RouteMatrixConfig NoAdsScheme({ {}, { "geosite:category-ads-all" }, {} }, { {}, {}, {} }, "AsIs");

    /**
     * @brief The Qv2rayRouteScheme struct
     * @author DuckSoft <realducksoft@gmail.com>
     */
    struct Qv2rayRouteScheme : Qv2ray::Models::RouteMatrixConfig
    {
        /**
         * @brief the name of the scheme.
         * @example "Untitled Scheme"
         */
        Bindable<QString> name;
        /**
         * @brief the author of the scheme.
         * @example "DuckSoft <realducksoft@gmail.com>"
         */
        Bindable<QString> author;
        /**
         * @brief details of this scheme.
         * @example "A scheme to bypass China mainland, while allowing bilibili to go through proxy."
         */
        Bindable<QString> description;
        QJS_JSON(P(name, author, description), B(RouteMatrixConfig))
    };
} // namespace Qv2ray::components::RouteSchemeIO
