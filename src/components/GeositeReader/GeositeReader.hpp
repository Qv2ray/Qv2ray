#pragma once

#include <QString>

namespace Qv2ray::components::GeositeReader
{
    QStringList ReadGeoSiteFromFile(const QString &filepath, bool allowCache = true);
} // namespace Qv2ray::components::GeositeReader
