#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::config
{
    void SaveGlobalConfig(Qv2rayConfig conf);
    void SetConfigDirPath(const QString &path);
    void LoadGlobalConfig();
}

using namespace Qv2ray::core;
using namespace Qv2ray::core::config;
