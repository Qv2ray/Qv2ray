#include "CoreUtils.hpp"

#include "common/QvHelpers.hpp"
#include "core/connection/Serialization.hpp"
#include "core/handler/ConfigHandler.hpp"

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
        // Set initial values.
        *host = QObject::tr("N/A");
        *port = 0;
        *protocol = out["protocol"].toString(QObject::tr("N/A")).toLower();

        if (*protocol == "vmess" || *protocol == "vless")
        {
            auto Server = VMessServerObject::fromJson(QJsonIO::GetValue(out, "settings", "vnext", 0).toObject());
            *host = Server.address;
            *port = Server.port;
            return true;
        }
        else if (*protocol == "shadowsocks")
        {
            auto Server = ShadowSocksServerObject::fromJson(QJsonIO::GetValue(out, "settings", "servers", 0).toObject());
            *host = Server.address;
            *port = Server.port;
            return true;
        }
        else if (*protocol == "socks")
        {
            auto Server = SocksServerObject::fromJson(QJsonIO::GetValue(out, "settings", "servers", 0).toObject());
            *host = Server.address;
            *port = Server.port;
            return true;
        }
        else if (*protocol == "http")
        {
            auto Server = HttpServerObject::fromJson(QJsonIO::GetValue(out, "settings", "servers", 0).toObject());
            *host = Server.address;
            *port = Server.port;
            return true;
        }
        else
        {
            bool status;
            auto info = PluginHost->TryGetOutboundInfo(*protocol, out["settings"].toObject(), &status);
            *host = info.hostName;
            *port = info.port;
            return status;
        }
    }

    ///
    /// [Protocol, Host, Port]
    const std::tuple<QString, QString, int> GetConnectionInfo(const ConnectionId &id, bool *status)
    {
        if (status != nullptr)
            *status = false;
        auto root = ConnectionManager->GetConnectionRoot(id);
        return GetConnectionInfo(root, status);
    }

    const std::tuple<QString, QString, int> GetConnectionInfo(const CONFIGROOT &out, bool *status)
    {
        if (status != nullptr)
            *status = false;
        //
        //
        for (auto item : out["outbounds"].toArray())
        {
            OUTBOUND outBoundRoot = OUTBOUND(item.toObject());
            QString host;
            int port;
            QString outboundType = "";

            if (GetOutboundInfo(outBoundRoot, &host, &port, &outboundType))
            {
                if (status != nullptr)
                    *status = true;
                // These lines will mess up the detection of protocols in subscription update.
                // if (IsComplexConfig(out))
                //{
                //    outboundType += " " + QObject::tr("(Guessed)");
                //    host += " " + QObject::tr("(Guessed)");
                //}
                return { outboundType, host, port };
            }
            else
            {
                LOG(MODULE_CORE_HANDLER, "Unknown outbound type: " + outboundType + ", cannot deduce host and port.")
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
        auto result = GetConnectionUsageAmount(id);
        return std::get<0>(result) + std::get<1>(result);
    }

    int64_t GetConnectionLatency(const ConnectionId &id)
    {
        auto connection = ConnectionManager->GetConnectionMetaObject(id);
        return std::max(connection.latency, {});
    }

    const QString GetConnectionProtocolString(const ConnectionId &id)
    {
        // Don't bother with the complex connection configs.
        if (IsComplexConfig(id))
        {
            return QV2RAY_SERIALIZATION_COMPLEX_CONFIG_PLACEHOLDER;
        }
        CONFIGROOT root = ConnectionManager->GetConnectionRoot(id);
        QString result;
        QStringList streamProtocols;
        auto outbound = root["outbounds"].toArray().first().toObject();
        result.append(outbound["protocol"].toString());

        if (outbound.contains("streamSettings") && outbound["streamSettings"].toObject().contains("network"))
        {
            result.append(" / " + outbound["streamSettings"].toObject()["network"].toString());
            if (outbound["streamSettings"].toObject().contains("tls"))
            {
                result.append(outbound["streamSettings"].toObject()["tls"].toBool() ? " / tls" : "");
            }
        }

        return result;
    }

    const QString GetDisplayName(const ConnectionId &id, int limit)
    {
        return TruncateString(ConnectionManager->GetConnectionMetaObject(id).displayName, limit);
    }

    const QString GetDisplayName(const GroupId &id, int limit)
    {
        return TruncateString(ConnectionManager->GetGroupMetaObject(id).displayName, limit);
    }

    const QMap<QString, InboundInfoObject> GetConfigInboundInfo(const CONFIGROOT &root)
    {
        QMap<QString, InboundInfoObject> inboundPorts;
        for (const auto &inboundVal : root["inbounds"].toArray())
        {
            const auto &inbound = inboundVal.toObject();
            inboundPorts[inbound["tag"].toString()] = {
                inbound["protocol"].toString(),     //
                inbound["listen"].toString(),       //
                inbound["port"].toVariant().toInt() //
            };
        }
        return inboundPorts;
    }

    const QMap<QString, InboundInfoObject> GetConfigInboundInfo(const ConnectionId &id)
    {
        return GetConfigInboundInfo(ConnectionManager->GetConnectionRoot(id));
    }
} // namespace Qv2ray::core
