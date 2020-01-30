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

namespace Qv2ray
{
    // Qv2ray runtime config
    inline bool isExiting = false;
    inline QString Qv2rayConfigPath = "/";
    inline base::Qv2rayRuntimeConfig RuntimeConfig = base::Qv2rayRuntimeConfig();
    inline base::config::Qv2rayConfig GlobalConfig = base::config::Qv2rayConfig();
    inline base::QvStartupOptions StartupOption = base::QvStartupOptions();
}
