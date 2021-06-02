#pragma once
#include "base/models/CoreObjectModels.hpp"
#include "base/models/QvSafeType.hpp"
#include "plugin-interface/connections/ConnectionsBaseTypes.hpp"

#define CurrentStatAPIType (*GlobalConfig.uiConfig->graphConfig->useOutboundStats ? API_OUTBOUND_PROXY : API_INBOUND)

namespace Qv2ray::core
{
    using namespace Qv2ray::base;
    using namespace Qv2ray::base::safetype;
    using namespace Qv2ray::base::objects;

    inline const QString getTag(const INBOUND &in)
    {
        return in["tag"].toString();
    }

    inline const QString getTag(const OUTBOUND &in)
    {
        return in["tag"].toString();
    }

    inline const QString getTag(const RuleObject &in)
    {
        return in.QV2RAY_RULE_TAG;
    }

    template<typename R>
    R GetConnectionPart(const CONFIGROOT &r, int index)
    {
        if constexpr (std::is_same_v<R, INBOUND>)
            return INBOUND{ QJsonIO::GetValue(r, "inbounds", index).toObject() };
        if constexpr (std::is_same_v<R, OUTBOUND>)
            return OUTBOUND{ QJsonIO::GetValue(r, "outbounds", index).toObject() };
    }

    bool IsComplexConfig(const CONFIGROOT &root);
    bool IsComplexConfig(const ConnectionId &id);
    QString GetDisplayName(const GroupId &id, int limit = -1);
    QString GetDisplayName(const ConnectionId &id, int limit = -1);

    const QString GetConnectionProtocolDescription(const ConnectionId &id);
    int64_t GetConnectionLatency(const ConnectionId &id);
    uint64_t GetConnectionTotalData(const ConnectionId &id);
    const std::tuple<quint64, quint64> GetConnectionUsageAmount(const ConnectionId &id);

    PluginIOBoundData GetInboundInfo(const INBOUND &in);
    std::tuple<QString, QString, int> GetInboundInfoTuple(const INBOUND &in);
    QMap<QString, PluginIOBoundData> GetInboundsInfo(const CONFIGROOT &root);
    QMap<QString, PluginIOBoundData> GetInboundsInfo(const ConnectionId &id);

    PluginIOBoundData GetOutboundInfo(const OUTBOUND &out);
    std::tuple<QString, QString, int> GetOutboundInfoTuple(const OUTBOUND &out);
    QMap<QString, PluginIOBoundData> GetOutboundsInfo(const CONFIGROOT &out);
    QMap<QString, PluginIOBoundData> GetOutboundsInfo(const ConnectionId &id);

} // namespace Qv2ray::core

using namespace Qv2ray::core;
