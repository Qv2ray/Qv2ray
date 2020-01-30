#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core
{
    void SaveGlobalConfig(Qv2rayConfig conf);
    void LoadGlobalConfig();
    void SetConfigDirPath(const QString &path);
    //
    inline QString getTag(const INBOUND &in)
    {
        return in["tag"].toString();
    }

    inline QString getTag(const OUTBOUND &in)
    {
        return in["tag"].toString();
    }

    void ExitQv2ray();
    tuple<QString, int, QString> GetConnectionInfo(const CONFIGROOT &alias);
    bool GetOutboundData(const OUTBOUND &out, QString *host, int *port, QString *protocol);
}

using namespace Qv2ray::core;
