// Instantiation for Qv2ray global objects.

#include "Qv2rayBase.hpp"
#include "QvUtils.hpp"
#include "QvRuntimeConfig.hpp"
#include "QvCore/QvCommandLineArgs.hpp"

// Global scope variable.
#ifdef QT_DEBUG
const bool isDebugBuild = true;
#else
const bool isDebugBuild = false;
#endif

namespace Qv2ray
{
    namespace Utils
    {
        // Qv2ray runtime config
        Qv2rayRuntimeConfig RuntimeConfig = Qv2rayRuntimeConfig();
        bool isExiting = false;
        Qv2rayConfig GlobalConfig = Qv2rayConfig();
        QString Qv2rayConfigPath = "";
    }

    namespace CommandArgOperations
    {
        QvStartupOptions StartupOption = QvStartupOptions{};
    }
}
