#pragma once
#include <QString>
#include <base/models/QvSafeType.hpp>
#include <base/models/CoreObjectModels.hpp>

namespace Qv2ray::core
{
    using namespace Qv2ray::base;
    using namespace Qv2ray::base::safetype;
    using namespace Qv2ray::base::objects;
    //
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

    bool GetOutboundData(const OUTBOUND &out, QString *host, int *port, QString *protocol);
    bool IsComplexConfig(const CONFIGROOT &root);
}

using namespace Qv2ray::core;
