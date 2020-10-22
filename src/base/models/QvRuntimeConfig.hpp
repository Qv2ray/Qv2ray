#pragma once

#define SET_RUNTIME_CONFIG(conf, val) RuntimeConfig.conf = val();
#define RESTORE_RUNTIME_CONFIG(conf, func) func(RuntimeConfig.conf);

namespace Qv2ray::base
{
    struct Qv2rayRuntimeConfig
    {
        bool screenShotHideQv2ray = false;
    };
    inline base::Qv2rayRuntimeConfig RuntimeConfig = base::Qv2rayRuntimeConfig();
} // namespace Qv2ray::base
