#pragma once

#include "base/Qv2rayBase.hpp"
#include "core/config/ConfigBackend.hpp"

namespace Qv2ray::core
{
    inline QString getTag(const INBOUND &in)
    {
        return in["tag"].toString();
    }

    inline QString getTag(const OUTBOUND &in)
    {
        return in["tag"].toString();
    }

    inline QString getTag(const RuleObject &in)
    {
        return in.QV2RAY_RULE_TAG;
    }

    /// Host, port, type
    tuple<QString, int, QString> GetConnectionInfo(const CONFIGROOT &alias);
    bool GetOutboundData(const OUTBOUND &out, QString *host, int *port, QString *protocol);
    bool CheckIsComplexConfig(CONFIGROOT root);
}

using namespace Qv2ray::core;
