#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::components::geosite
{
    QStringList ReadGeoSiteFromFile(QString filepath);
}

using namespace Qv2ray::components;
using namespace Qv2ray::components::geosite;
