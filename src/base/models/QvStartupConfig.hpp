#pragma once

namespace Qv2ray
{
    namespace base
    {
        struct QvStartupOptions
        {
            /// No API subsystem
            bool noAPI;
            /// Explicitly run as root user.
            bool forceRunAsRootUser;
            /// Enable Debug Log.
            bool debugLog;
            /// Enable Network toolbar plugin.
            bool enableToolbarPlguin;
            /// Endable HiDPI support.
            bool hiDPI;
            /// Force endable HiDPI support.
            bool forceHiDPI;
        };
    } // namespace base
    inline base::QvStartupOptions StartupOption = base::QvStartupOptions();
} // namespace Qv2ray
