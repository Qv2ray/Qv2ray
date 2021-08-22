#pragma once

namespace Qv2ray::components::autolaunch
{
    bool GetLaunchAtLoginStatus();
    void SetLaunchAtLoginStatus(bool enable);
} // namespace Qv2ray::components::autolaunch

using namespace Qv2ray::components;
using namespace Qv2ray::components::autolaunch;
