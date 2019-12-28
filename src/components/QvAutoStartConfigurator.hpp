#ifndef QVAUTOSTARTCONFIGURATOR_H
#define QVAUTOSTARTCONFIGURATOR_H

#include "QvUtils.hpp"

namespace Qv2ray
{
    namespace Components
    {
        bool InstallAutoStart();
        bool RemoveAutoStart();
    }
}

using namespace Qv2ray::Components;

#endif // QVAUTOSTARTCONFIGURATOR_H
