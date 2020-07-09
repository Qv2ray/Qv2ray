#pragma once

namespace Qv2ray::base
{
    struct QvStartupOptions
    {
        /// No API subsystem
        bool noAPI;

        /// Do not automatically connect to a server
        bool noAutoConnection;

        /// Enable Debug Log.
        bool debugLog;

        /// Disable Qt scale factors support.
        bool noScaleFactor;

        /// Disable all plugin features.
        bool noPlugins;

        /// Exit existing Qv2ray instance
        bool exitQv2ray;

        ///
    };
} // namespace Qv2ray::base

inline Qv2ray::base::QvStartupOptions StartupOption = Qv2ray::base::QvStartupOptions();
