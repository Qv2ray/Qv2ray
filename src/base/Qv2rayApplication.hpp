#pragma once

#include "3rdparty/SingleApplication/singleapplication.h"

namespace Qv2ray
{
    class Qv2rayApplication : public SingleApplication
    {
    };
} // namespace Qv2ray

#ifdef qApp
    #define qvApp (static_cast<Qv2ray::Qv2rayApplication *>(QCoreApplication::instance()))
#endif
