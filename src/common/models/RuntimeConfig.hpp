#ifndef QVRUNTIMECONFIG_HPP
#define QVRUNTIMECONFIG_HPP

#define SET_RUNTIME_CONFIG(conf, val) RuntimeConfig.conf = val();
#define RESTORE_RUNTIME_CONFIG(conf, func) func(RuntimeConfig.conf);

namespace Qv2ray
{
    namespace Utils
    {
        struct Qv2rayRuntimeConfig {
            bool screenShotHideQv2ray;
        };
        extern Qv2rayRuntimeConfig RuntimeConfig;
    }
}
#endif // QVRUNTIMECONFIG_HPP
