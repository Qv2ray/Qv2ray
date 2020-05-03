#pragma once
#include "base/models/QvSettingsObject.hpp"
namespace Qv2ray::components::route
{
    const inline Qv2ray::base::config::Qv2rayConfig_Routing emptyScheme;
    /**
     * @brief The Qv2rayRouteScheme struct
     * @author DuckSoft <realducksoft@gmail.com>
     */
    struct Qv2rayRouteScheme : config::Qv2rayConfig_Routing
    {
        /**
         * @brief the name of the scheme.
         * @example "Untitled Scheme"
         */
        QString name;
        /**
         * @brief the author of the scheme.
         * @example "DuckSoft <realducksoft@gmail.com>"
         */
        QString author;
        /**
         * @brief details of this scheme.
         * @example "A scheme to bypass China mainland, while allowing bilibili to go through proxy."
         */
        QString description;

        // M: all these fields are mandatory
        JSONSTRUCT_REGISTER(Qv2rayRouteScheme, F(name, author, description, domainStrategy, domains, ips));
    };
} // namespace Qv2ray::components::route

using namespace Qv2ray::components::route;
