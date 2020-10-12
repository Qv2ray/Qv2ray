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
        bool cRouting = root.contains("routing");
        bool cRule = cRouting && root["routing"].toObject().contains("rules");
        bool cRules = cRule && root["routing"].toObject()["rules"].toArray().count() > 0;
        //
        bool cInbounds = root.contains("inbounds");
        bool cInboundCount = cInbounds && root["inbounds"].toArray().count() > 0;
        //
        bool cOutbounds = root.contains("outbounds");
        bool cOutboundCount = cOutbounds && root["outbounds"].toArray().count() > 1;
        return cRules || cInboundCount || cOutboundCount;
    }

    bool GetOutboundInfo(const OUTBOUND &out, QString *host, int *port, QString *protocol)
    {
        *protocol = out["protocol"].toString(QObject::tr("N/A")).toLower();
        bool ok;
        const auto info = PluginHost->GetOutboundInfo(*protocol, out["settings"].toObject(), ok);
        if (ok)
        {
            *host = info[INFO_SERVER].toString();
            *port = info[INFO_PORT].toInt();
        }
        else
        {
            *host = QObject::tr("N/A");
            *port = 0;
        }
        return ok;
    }

    ///
    /// [Protocol, Host, Port]
    const ProtocolSettingsInfoObject GetConnectionInfo(const ConnectionId &id, bool *status)
    {
        if (status != nullptr)
            *status = false;
        const auto root = ConnectionManager->GetConnectionRoot(id);
        return GetConnectionInfo(root, status);
    }

    const ProtocolSettingsInfoObject GetConnectionInfo(const CONFIGROOT &out, bool *status)
    {
        if (status != nullptr)
            *status = false;
        //
        //
        for (const auto &item : out["outbounds"].toArray())
        {
            const auto outboundRoot = OUTBOUND(item.toObject());
            QString host;
            int port;
            QString outboundType = "";

            if (GetOutboundInfo(outboundRoot, &host, &port, &outboundType))
            {
                if (status != nullptr)
                    *status = true;
                // These lines will mess up the detection of protocols in subscription update.
                // if (IsComplexConfig(out))
                //{
                //    outboundType += " " + QObject::tr("(Guessed)");
                //    host += " " + QObject::tr("(Guessed)");
                //}
                return ProtocolSettingsInfoObject{ outboundType, host, port };
            }
            else
            {
                LOG("Unknown outbound type: " + outboundType + ", cannot deduce host and port.");
            }
        }
        return { QObject::tr("N/A"), QObject::tr("N/A"), 0 };
    }

    const std::tuple<quint64, quint64> GetConnectionUsageAmount(const ConnectionId &id)
    {
        auto connection = ConnectionManager->GetConnectionMetaObject(id);
        return { connection.stats[CurrentStatAPIType].upLinkData, connection.stats[CurrentStatAPIType].downLinkData };
    }

    uint64_t GetConnectionTotalData(const ConnectionId &id)
    {
        const auto &[a, b] = GetConnectionUsageAmount(id);
        return a + b;
    }

    int64_t GetConnectionLatency(const ConnectionId &id)
    {
        const auto connection = ConnectionManager->GetConnectionMetaObject(id);
        return std::max(connection.latency, {});
    }

    const QString GetConnectionProtocolString(const ConnectionId &id)
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

    const QString GetDisplayName(const ConnectionId &id, int limit)
    {
        return TruncateString(ConnectionManager->GetConnectionMetaObject(id).displayName, limit);
    }

    const QString GetDisplayName(const GroupId &id, int limit)
    {
        return TruncateString(ConnectionManager->GetGroupMetaObject(id).displayName, limit);
    }

    bool GetInboundInfo(const INBOUND &in, QString *listen, int *port, QString *protocol)
    {
        *protocol = in["protocol"].toString();
        *listen = in["listen"].toString();
        *port = in["port"].toVariant().toInt();
        return true;
    }

    const QMap<QString, ProtocolSettingsInfoObject> GetInboundInfo(const CONFIGROOT &root)
    {
        QMap<QString, ProtocolSettingsInfoObject> inboundPorts;
        for (const auto &inboundVal : root["inbounds"].toArray())
        {
            INBOUND in{ inboundVal.toObject() };
            QString host, protocol;
            int port;
            if (GetInboundInfo(in, &host, &port, &protocol))
                inboundPorts[getTag(in)] = { protocol, host, port };
        }
        return inboundPorts;
    }

    const QMap<QString, ProtocolSettingsInfoObject> GetInboundInfo(const ConnectionId &id)
    {
        return GetInboundInfo(ConnectionManager->GetConnectionRoot(id));
    }
} // namespace Qv2ray::core
