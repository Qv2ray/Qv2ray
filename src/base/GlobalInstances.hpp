#pragma once

#include "base/models/QvConfigModel.hpp"
#include "base/models/QvRuntimeConfig.hpp"
#include "base/models/QvStartupConfig.hpp"

#include <QTranslator>

// Instantiation for Qv2ray global objects.

namespace Qv2ray
{
    // Qv2ray runtime config
    inline bool isExiting = false;
    inline QString Qv2rayConfigPath = "/";
    inline base::Qv2rayRuntimeConfig RuntimeConfig = base::Qv2rayRuntimeConfig();
    inline base::config::Qv2rayConfig GlobalConfig = base::config::Qv2rayConfig();
    inline base::QvStartupOptions StartupOption = base::QvStartupOptions();
    //
    inline std::unique_ptr<QTranslator> Qv2rayTranslator;
} // namespace Qv2ray
