#include "ConnectionHandler.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::handlers
{

    QvConnectionHandler::QvConnectionHandler()
    {
        DEBUG(MODULE_CORE_HANDLER, "ConnectionHandler Constructor.")

        // Do we need to check how many of them are loaded?
        for (auto i = 0; i < GlobalConfig.connections.count(); i++) {
            connections[GlobalConfig.connections.keys()[i]] = GlobalConfig.connections.values()[i];
        }

        for (auto key : GlobalConfig.subscriptions.keys()) {
            auto val = GlobalConfig.subscriptions[key];
            groups[key] = val;

            for (auto conn : val.connections) {
                connections[conn].groupId = key;
            }
        }

        for (auto key : GlobalConfig.groups.keys()) {
            auto val = GlobalConfig.groups[key];
            groups[key] = val;

            for (auto conn : val.connections) {
                connections[conn].groupId = key;
            }
        }

        kernelInstance = new V2rayKernelInstance();
        saveTimerId = startTimer(60000);
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

    const ConnectionMetaObject QvConnectionHandler::GetConnection(const ConnectionId &id) const
    {
        if (!connections.contains(id)) {
            LOG(MODULE_CORE_HANDLER, "Cannot find id: " + id.toString());
        }

        return connections[id];
    }

    const optional<QString> QvConnectionHandler::StartConnection(const ConnectionId &identifier)
    {
        if (!connections.contains(identifier)) {
            return tr("No connection selected!") + NEWLINE + tr("Please select a config from the list.");
        }

        CONFIGROOT root = CHGetConnectionRoot_p(connections[identifier].groupId, identifier);
        return CHTryStartConnection_p(identifier, root);
    }


    const QString QvConnectionHandler:: GetConnectionBasicInfo(const ConnectionId &id) const
    {
        QString result;

        if (!connections.contains(id)) {
            result = tr("N/A");
        }

        CONFIGROOT root = CHGetConnectionRoot_p(connections[id].groupId, id);
        bool isTLSEnabled = false;

        if (root.contains("inbounds") && !root["inbounds"].toArray().isEmpty()) {
            QString inProtocol = tr("Inbounds: ");

            for (auto inbound : root["inbounds"].toArray()) {
                inProtocol.append(inbound.toObject()["protocol"].toString());
                inProtocol.append("/");
            }

            inProtocol.chop(1);
            result.append(inProtocol);
        }

        QStringList protocols;
        QStringList streamProtocols;

        for (auto outbound : root["outbounds"].toArray()) {
            protocols.append(outbound.toObject()["protocol"].toString());
        }

        result.chop(1);
        return result;
    }


    const GroupMetaObject QvConnectionHandler::GetGroup(const GroupId &id) const
    {
        return groups[id];
    }

    QvConnectionHandler::~QvConnectionHandler()
    {
        if (kernelInstance->KernelStarted) {
            kernelInstance->StopConnection();
            LOG(MODULE_CORE_HANDLER, "Stopped connection from destructor.")
        }

        delete kernelInstance;
    }

    const CONFIGROOT QvConnectionHandler::CHGetConnectionRoot_p(const GroupId &group, const ConnectionId &id) const
    {
        auto path = QV2RAY_CONNECTIONS_DIR + group.toString() + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION;
        path.prepend(groups[group].isSubscription ? QV2RAY_SUBSCRIPTION_DIR : QV2RAY_CONNECTIONS_DIR);
        return CONFIGROOT(JsonFromString(StringFromFile(path)));
    }
}
