#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::config
{
    void SaveGlobalSettings();
    void SaveGlobalSettings(const Qv2rayConfig &conf);
    void SetConfigDirPath(const QString &path);
    bool CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig);
} // namespace Qv2ray::core::config

using namespace Qv2ray::core;
using namespace Qv2ray::core::config;
