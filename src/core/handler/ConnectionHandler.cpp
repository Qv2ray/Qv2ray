#include "ConnectionHandler.hpp"
#include "common/QvHelpers.hpp"
#include "core/config/ConfigBackend.hpp"

namespace Qv2ray::core::handlers
{

    QvConnectionHandler::QvConnectionHandler() : currentConnectionId("null")
    {
        DEBUG(MODULE_CORE_HANDLER, "ConnectionHandler Constructor.")

        // Do we need to check how many of them are loaded?
        for (auto i = 0; i < GlobalConfig.connections.count(); i++) {
            connections[ConnectionId(GlobalConfig.connections.keys()[i])] = GlobalConfig.connections.values()[i];
        }

        for (auto key : GlobalConfig.subscriptions.keys()) {
            auto val = GlobalConfig.subscriptions[key];
            groups[GroupId(key)] = val;

            for (auto conn : val.connections) {
                connections[ConnectionId(conn)].groupId = GroupId(key);
            }
        }

        for (auto key : GlobalConfig.groups.keys()) {
            auto val = GlobalConfig.groups[key];
            groups[GroupId(key)] = val;

            for (auto conn : val.connections) {
                connections[ConnectionId(conn)].groupId = GroupId(key);
            }
        }

        vCoreInstance = new V2rayKernelInstance();
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessErrored, this, &QvConnectionHandler::OnVCoreCrashed);
        connect(vCoreInstance, &V2rayKernelInstance::OnNewStatsDataArrived, this, &QvConnectionHandler::OnStatsDataArrived);
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessOutputReadyRead, this, &QvConnectionHandler::OnVCoreLogAvailable);
        saveTimerId = startTimer(10 * 1000);
    }

    void QvConnectionHandler::CHSaveConnectionData_p()
    {
        // Copy
        auto newGlobalConfig = GlobalConfig;
        newGlobalConfig.connections.clear();
        newGlobalConfig.groups.clear();
        newGlobalConfig.subscriptions.clear();

        for (auto i = 0; i < connections.count(); i++) {
            newGlobalConfig.connections[connections.keys()[i].toString()] = connections.values()[i];
        }

        for (auto i = 0; i < groups.count(); i++) {
            QStringList connections = IdListToStrings(groups.values()[i].connections);

            if (groups.values()[i].isSubscription) {
                SubscriptionObject_Config o = groups.values()[i];
                o.connections = connections;
                newGlobalConfig.subscriptions[groups.keys()[i].toString()] = o;
            } else {
                GroupObject_Config o = groups.values()[i];
                o.connections = connections;
                newGlobalConfig.groups[groups.keys()[i].toString()] = o;
            }
        }

        SaveGlobalConfig(newGlobalConfig);
    }

    void QvConnectionHandler::timerEvent(QTimerEvent *event)
    {
        if (event->timerId() == saveTimerId) {
            CHSaveConnectionData_p();
        }
    }

    const QList<ConnectionId> QvConnectionHandler::Connections() const
    {
        return connections.keys();
    }

    const QList<GroupId> QvConnectionHandler::AllGroups() const
    {
        return groups.keys();
    }

    const QList<GroupId> QvConnectionHandler::Subscriptions() const
    {
        QList<GroupId> subsList;

        for (auto group : groups.keys()) {
            if (groups[group].isSubscription) {
                subsList.push_back(group);
            }
        }

        return subsList;
    }

    const QList<ConnectionId> QvConnectionHandler::Connections(const GroupId &groupId) const
    {
        return groups[groupId].connections;
    }

    const QString QvConnectionHandler::GetDisplayName(const ConnectionId &id, int limit) const
    {
        return TruncateString(connections[id].displayName, limit);
    }

    const QString QvConnectionHandler::GetDisplayName(const GroupId &id, int limit) const
    {
        return TruncateString(groups[id].displayName, limit);
    }

    const ConnectionId QvConnectionHandler::GetConnectionIdByDisplayName(const QString &displayName) const
    {
        for (auto conn : connections.keys()) {
            if (connections[conn].displayName == displayName) {
                return conn;
            }
        }

        return ConnectionId("null");
    }

    const GroupId QvConnectionHandler::GetGroupIdByDisplayName(const QString &displayName) const
    {
        for (auto group : groups.keys()) {
            if (groups[group].displayName == displayName) {
                return group;
            }
        }

        return GroupId("null");
    }

    const GroupId QvConnectionHandler::GetConnectionGroupId(const ConnectionId &id) const
    {
        if (!connections.contains(id)) {
            LOG(MODULE_CORE_HANDLER, "Cannot find id: " + id.toString());
        }

        return connections[id].groupId;
    }

    int64_t QvConnectionHandler::GetConnectionLatency(const ConnectionId &id) const
    {
        if (!connections.contains(id)) {
            LOG(MODULE_CORE_HANDLER, "Cannot find id: " + id.toString());
        }

        return connections[id].latency;
    }

    const optional<QString> QvConnectionHandler::DeleteConnection(const ConnectionId &id)
    {
        // TODO
        Q_UNUSED(id)
        return "";
    }

    const optional<QString> QvConnectionHandler::StartConnection(const ConnectionId &identifier)
    {
        if (!connections.contains(identifier)) {
            return tr("No connection selected!") + NEWLINE + tr("Please select a config from the list.");
        }

        StopConnection();
        CONFIGROOT root = CHGetConnectionRoot_p(connections[identifier].groupId, identifier);
        return CHStartConnection_p(identifier, root);
    }

    void QvConnectionHandler::StopConnection() //const ConnectionId &id
    {
        // Currently just simply stop it.
        //_UNUSED(id)
        //if (currentConnectionId == id) {
        //}
        CHStopConnection_p();
    }

    bool QvConnectionHandler::IsConnected(const ConnectionId &id) const
    {
        return currentConnectionId == id;
    }

    const QString QvConnectionHandler::GetConnectionProtocolString(const ConnectionId &id) const
    {
        QString result;

        if (!connections.contains(id)) {
            result = tr("N/A");
        }

        CONFIGROOT root = CHGetConnectionRoot_p(connections[id].groupId, id);
        QStringList protocols;
        QStringList streamProtocols;
        auto outbound = root["outbounds"].toArray().first().toObject();
        result.append(outbound["protocol"].toString());

        if (outbound.contains("streamSettings")) {
            result.append(" + " + outbound["streamSettings"].toObject()["network"].toString());

            if (outbound["streamSettings"].toObject().contains("tls")) {
                result.append(outbound["streamSettings"].toObject()["tls"].toBool() ?  " + tls" : "");
            }
        }

        return result;
    }

    const tuple<quint64, quint64> QvConnectionHandler::GetConnectionUsageAmount(const ConnectionId &id) const
    {
        if (!connections.contains(id)) {
            return make_tuple(0, 0);
        }

        return make_tuple(connections[id].upLinkData, connections[id].downLinkData);
    }

    //const GroupMetaObject QvConnectionHandler::GetGroup(const GroupId &id) const
    //{
    //    return groups[id];
    //}

    QvConnectionHandler::~QvConnectionHandler()
    {
        CHSaveConnectionData_p();

        if (vCoreInstance->KernelStarted) {
            vCoreInstance->StopConnection();
            LOG(MODULE_CORE_HANDLER, "Stopped connection from destructor.")
        }

        delete vCoreInstance;
    }

    const CONFIGROOT QvConnectionHandler::CHGetConnectionRoot_p(const ConnectionId &id) const
    {
        return connections.contains(id) ? CHGetConnectionRoot_p(connections[id].groupId, id) : CONFIGROOT();
    }

    const CONFIGROOT QvConnectionHandler::CHGetConnectionRoot_p(const GroupId &group, const ConnectionId &id) const
    {
        auto path = group.toString() + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        path.prepend(groups[group].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR);
        return CONFIGROOT(JsonFromString(StringFromFile(path)));
    }
    //

    const tuple<QString, int> QvConnectionHandler::GetConnectionInfo(const ConnectionId &id) const
    {
        auto root = CHGetConnectionRoot_p(id);
        bool validOutboundFound = false;
        QString host;
        int port;

        for (auto item : root["outbounds"].toArray()) {
            OUTBOUND outBoundRoot = OUTBOUND(item.toObject());
            QString outboundType = "";
            validOutboundFound = CHGetOutboundData_p(outBoundRoot, &host, &port);

            if (validOutboundFound) {
                return make_tuple(host, port);
            } else {
                LOG(MODULE_CORE_HANDLER, "Unknown outbound entry: " + outboundType + ", cannot deduce host and port.")
            }
        }

        return make_tuple(QObject::tr("N/A"), 0);
    }

    bool QvConnectionHandler::CHGetOutboundData_p(const OUTBOUND &out, QString *host, int *port) const
    {
        // Set initial values.
        *host = QObject::tr("N/A");
        *port = 0;
        auto protocol = out["protocol"].toString(QObject::tr("N/A")).toLower();

        if (protocol == "vmess") {
            auto Server = StructFromJsonString<VMessServerObject>(JsonToString(out["settings"].toObject()["vnext"].toArray().first().toObject()));
            *host = Server.address;
            *port = Server.port;
            return true;
        } else if (protocol == "shadowsocks") {
            auto x = JsonToString(out["settings"].toObject()["servers"].toArray().first().toObject());
            auto Server = StructFromJsonString<ShadowSocksServerObject>(x);
            *host = Server.address;
            *port = Server.port;
            return true;
        } else if (protocol == "socks") {
            auto x = JsonToString(out["settings"].toObject()["servers"].toArray().first().toObject());
            auto Server = StructFromJsonString<SocksServerObject>(x);
            *host = Server.address;
            *port = Server.port;
            return true;
        } else {
            return false;
        }
    }
}
