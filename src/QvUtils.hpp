#ifndef UTILS_H
#define UTILS_H

#include "Qv2rayBase.hpp"
#include "QvHelpers.hpp"
#include "QvCore/QvCommandLineArgs.hpp"

namespace Qv2ray
{
    namespace Utils
    {
        void SetConfigDirPath(const QString *path);
        QString GetConfigDirPath();

        void SetGlobalConfig(Qv2rayConfig conf);
        Qv2rayConfig GetGlobalConfig();

        void LoadGlobalConfig();

        inline QString getTag(const INBOUND &in)
        {
            return in["tag"].toString();
        }

        inline QString getTag(const OUTBOUND &in)
        {
            return in["tag"].toString();
        }

        void ExitQv2ray();
        bool isExiting();
        tuple<QString, int, QString> GetConnectionInfo(const CONFIGROOT &alias);
        bool GetOutboundData(const OUTBOUND &out, QString *host, int *port, QString *protocol);
    }
}

using namespace Qv2ray::Utils;
#endif // UTILS_H
