#pragma once

#define SET_RUNTIME_CONFIG(conf, val) RuntimeConfig.conf = val();
#define RESTORE_RUNTIME_CONFIG(conf, func) func(RuntimeConfig.conf);

namespace Qv2ray::base
{
    struct Qv2rayRuntimeConfig {
        /// Whether to hide Qv2ray when taking a screenshot
        bool screenShotHideQv2ray;
    };
}
