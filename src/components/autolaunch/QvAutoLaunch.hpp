#ifndef QVLAUNCH_AT_LOGINCONFIGURATOR_H
#define QVLAUNCH_AT_LOGINCONFIGURATOR_H

#include "QvUtils.hpp"

namespace Qv2ray
{
    namespace Components
    {
        bool GetLaunchAtLoginStatus();
        void SetLaunchAtLoginStatus(bool enable);
    }
}

using namespace Qv2ray::Components;

#endif // QVLAUNCH_AT_LOGINCONFIGURATOR_H
