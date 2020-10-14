#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::config
{
    void SaveGlobalSettings();
    bool LocateConfiguration();
    void SetConfigDirPath(const QString &path);
    bool CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig);
} // namespace Qv2ray::core::config

namespace Qv2ray
{
    // Extra header for QvConfigUpgrade.cpp
    QJsonObject UpgradeSettingsVersion(int fromVersion, int toVersion, const QJsonObject &root);
} // namespace Qv2ray

using namespace Qv2ray::core;
using namespace Qv2ray::core::config;
