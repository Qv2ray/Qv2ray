#pragma once

#include "base/models/QvRuntimeConfig.hpp"
#include "base/models/QvStartupConfig.hpp"
#include "base/models/QvConfigModel.hpp"

// Instantiation for Qv2ray global objects.
#ifdef QT_DEBUG
const bool isDebugBuild = true;
#else
const bool isDebugBuild = false;
#endif

namespace Qv2ray::base
{
    // Qv2ray runtime config
    inline bool isExiting = false;
    inline Qv2rayRuntimeConfig RuntimeConfig = Qv2rayRuntimeConfig();
    inline Qv2rayConfig GlobalConfig = Qv2rayConfig();
    inline QString Qv2rayConfigPath = "/";
    inline QvStartupOptions StartupOption = QvStartupOptions();
}
