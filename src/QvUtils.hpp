#ifndef UTILS_H
#define UTILS_H

#include "Qv2rayBase.hpp"
#include "QvHelpers.hpp"

namespace Qv2ray
{
    namespace Utils
    {

        void SetConfigDirPath(const QString *path);
        QString GetConfigDirPath();

        void SetGlobalConfig(Qv2rayConfig conf);
        Qv2rayConfig GetGlobalConfig();

        void LoadGlobalConfig();
    }
}

using namespace Qv2ray::Utils;
#endif // UTILS_H
