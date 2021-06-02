#include "CoreUtils.hpp"

#include "core/connection/Serialization.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "CoreUtils"

namespace Qv2ray::core
{
    bool IsComplexConfig(const ConnectionId &id)
    {
        return IsComplexConfig(ConnectionManager->GetConnectionRoot(id));
    }

    bool IsComplexConfig(const CONFIGROOT &root)
    {
        // If has routing, and there're rules exist.
        bool cRouting = root.contains("routing");
        bool cRule = cRouting && root["routing"].toObject().contains("rules");
        bool cRules = cRule && root["routing"].toObject()["rules"].toArray().count() > 0;

        // If has inbounds, and there're inbounds exist.
        bool cInbounds = root.contains("inbounds");
        bool cInboundCount = cInbounds && root["inbounds"].toArray().count() > 0;

        // If has outbounds, and there're more than 1 outbounds.
        bool cOutbounds = root.contains("outbounds");
        bool cOutboundCount = cOutbounds && root["outbounds"].toArray().count() > 1;
        return cRules || cInboundCount || cOutboundCount;
    }

    const std::tuple<quint64, quint64> GetConnectionUsageAmount(const ConnectionId &id)
    {
        auto connection = ConnectionManager->GetConnectionMetaObject(id);
        return { *connection.stats->get(CurrentStatAPIType).upLinkData, *connection.stats->get(CurrentStatAPIType).downLinkData };
    }

    uint64_t GetConnectionTotalData(const ConnectionId &id)
    {
        const auto &[a, b] = GetConnectionUsageAmount(id);
        return a + b;
    }

    int64_t GetConnectionLatency(const ConnectionId &id)
    {
        const auto connection = ConnectionManager->GetConnectionMetaObject(id);
        return std::max(*connection.latency, {});
    }

    const QString GetConnectionProtocolDescription(const ConnectionId &id)
    {
        // Don't bother with the complex connection configs.
        if (IsComplexConfig(id))
        {
            return QV2RAY_SERIALIZATION_COMPLEX_CONFIG_PLACEHOLDER;
        }

        const auto root = ConnectionManager->GetConnectionRoot(id);
        const auto outbound = root["outbounds"].toArray().first().toObject();

        QStringList result;
        result << outbound["protocol"].toString();

        const auto streamSettings = outbound["streamSettings"].toObject();

        if (streamSettings.contains("network"))
            result << streamSettings["network"].toString();

        const auto security = streamSettings["security"].toString();
        if (!security.isEmpty() && security != "none")
            result << streamSettings["security"].toString();

        return result.join("+");
    }

    QString GetDisplayName(const ConnectionId &id, int limit)
    {
        const QString name = ConnectionManager->GetConnectionMetaObject(id).displayName;
        return TruncateString(name, limit);
    }

    QString GetDisplayName(const GroupId &id, int limit)
    {
        const QString name = ConnectionManager->GetGroupMetaObject(id).displayName;
        return TruncateString(name, limit);
    }

    std::tuple<QString, QString, int> GetOutboundInfoTuple(const OUTBOUND &out)
    {
        const auto protocol = out["protocol"].toString(QObject::tr("N/A")).toLower();
        const auto info = PluginHost->Outbound_GetData(protocol, out["settings"].toObject());
        if (info)
        {
            const auto val = *info;
            return { val[IOBOUND::PROTOCOL].toString(), val[IOBOUND::ADDRESS].toString(), val[IOBOUND::PORT].toInt() };
        }
        return { protocol, QObject::tr("N/A"), 0 };
    }

    QMap<QString, PluginIOBoundData> GetOutboundsInfo(const ConnectionId &id)
    {
        const auto root = ConnectionManager->GetConnectionRoot(id);
        return GetOutboundsInfo(root);
    }

    QMap<QString, PluginIOBoundData> GetOutboundsInfo(const CONFIGROOT &out)
    {
        QMap<QString, PluginIOBoundData> result;
        for (const auto &item : out["outbounds"].toArray())
        {
            const auto outboundRoot = OUTBOUND(item.toObject());
            result[getTag(outboundRoot)] = GetOutboundInfo(outboundRoot);
        }
        return result;
    }

    std::tuple<QString, QString, int> GetInboundInfoTuple(const INBOUND &in)
    {
        return { in["protocol"].toString(), in["listen"].toString(), in["port"].toVariant().toInt() };
    }

    QMap<QString, PluginIOBoundData> GetInboundsInfo(const ConnectionId &id)
    {
        return GetInboundsInfo(ConnectionManager->GetConnectionRoot(id));
    }

    QMap<QString, PluginIOBoundData> GetInboundsInfo(const CONFIGROOT &root)
    {
        QMap<QString, PluginIOBoundData> infomap;
        for (const auto &inRef : root["inbounds"].toArray())
        {
            const auto in = inRef.toObject();
            infomap[in["tag"].toString()] = GetInboundInfo(INBOUND(in));
        }
        return infomap;
    }

    PluginIOBoundData GetInboundInfo(const INBOUND &in)
    {
        return PluginIOBoundData{ { IOBOUND::PROTOCOL, in["protocol"].toString() },
                                  { IOBOUND::ADDRESS, in["listen"].toString() },
                                  { IOBOUND::PORT, in["port"].toInt() } };
    }

    PluginIOBoundData GetOutboundInfo(const OUTBOUND &out)
    {
        const auto data = PluginHost->Outbound_GetData(out["protocol"].toString(), out["settings"].toObject());
        if (data)
            return *data;
        return {};
    }

} // namespace Qv2ray::core
