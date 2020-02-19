#include "ConnectionHandler.hpp"
namespace Qv2ray::core::handlers
{
    template<typename IDType>
    QList<IDType> StringsToIdList(const QList<QString> &strings)
    {
        QList<IDType> list;

        for (auto str : strings) {
            list << IDType(str);
        }

        return list;
    }

    const QList<ConnectionIdentifier> QvConnectionHandler::IdentifierList(const SubscriptionId &subsId, const QList<ConnectionId> &idList) const
    {
        QList<ConnectionIdentifier> list;

        for (auto groupConnection : idList) {
            auto connId = ConnectionId(groupConnection);

            if (connections.keys().contains(connId)) {
                ConnectionIdentifier x;
                x.isSubscription = true;
                x.connectionId = connId;
                x.subscriptionId = subsId;
                list.push_back(x);
            }
        }

        return list;
    }

    const QList<ConnectionIdentifier> QvConnectionHandler::IdentifierList(const GroupId &groupId, const QList<ConnectionId> &idList) const
    {
        QList<ConnectionIdentifier> list;

        for (auto groupConnection : idList) {
            auto connId = ConnectionId(groupConnection);

            if (connections.keys().contains(connId)) {
                ConnectionIdentifier x;
                x.isSubscription = false;
                x.connectionId = connId;
                x.groupId = groupId;
                list.push_back(x);
            }
        }

        return list;
    }

    QvConnectionHandler::QvConnectionHandler()
    {
        DEBUG(MODULE_CORE_HANDLER, "ConnectionHandler Constructor.")

        // Do we need to check how many of them are loaded?
        for (auto i = 0; i < GlobalConfig.connections.count(); i++) {
            connections[ConnectionId(GlobalConfig.connections.keys()[i])] = GlobalConfig.connections.values()[i];
        }

        for (auto i = 0; i < GlobalConfig.subscriptions.count(); i++) {
            subscriptions[SubscriptionId(GlobalConfig.subscriptions.keys()[i])] = GlobalConfig.subscriptions.values()[i];
        }

        for (auto i = 0; i < GlobalConfig.groups.count(); i++) {
            groups[GroupId(GlobalConfig.groups.keys()[i])] = GlobalConfig.groups.values()[i];
        }

        kernelInstance = new V2rayKernelInstance();
        saveTimerId = startTimer(60000);
    }
    const QList<ConnectionIdentifier> QvConnectionHandler::Connections() const
    {
        QList<ConnectionIdentifier> list;

        for (auto i = 0; i < groups.count(); i++) {
            auto key = groups.keys()[i];
            auto group = groups[key];
            list.append(IdentifierList(key, StringsToIdList<ConnectionId>(group.connections)));
        }

        for (auto i = 0; i < subscriptions.count(); i++) {
            auto key = subscriptions.keys()[i];
            auto subs = subscriptions[key];
            list.append(IdentifierList(key, StringsToIdList<ConnectionId>(subs.connections)));
        }

        return list;
    }

    const QList<GroupId> QvConnectionHandler::Groups() const
    {
        return groups.keys();
    }

    const QList<SubscriptionId> QvConnectionHandler::Subscriptions() const
    {
        return subscriptions.keys();
    }

    const QList<ConnectionIdentifier> QvConnectionHandler::Connections(const GroupId &groupId) const
    {
        return IdentifierList(groupId, StringsToIdList<ConnectionId>(groups[groupId].connections));
    }

    const QList<ConnectionIdentifier> QvConnectionHandler::Connections(const SubscriptionId &subscriptionId) const
    {
        return IdentifierList(subscriptionId, StringsToIdList<ConnectionId>(subscriptions[subscriptionId].connections));
    }

    const ConnectionObject &QvConnectionHandler::GetConnection(const ConnectionId &id)
    {
        if (!connections.contains(id)) {
            LOG(MODULE_CORE_HANDLER, "Cannot find id: " + id.toString());
        }

        return connections[id];
    }

    const optional<QString> QvConnectionHandler::StartConnection(const ConnectionIdentifier &identifier)
    {
        if (!connections.contains(identifier.connectionId)) {
            return tr("No connection selected!") + NEWLINE + tr("Please select a config from the list.");
        }

        CONFIGROOT root = identifier.isSubscription
                          ? GetConnectionRoot(identifier.subscriptionId, identifier.connectionId)
                          : GetConnectionRoot(identifier.groupId, identifier.connectionId);
        return _CHTryStartConnection_p(identifier.connectionId, root);
    }


    const GroupObject &QvConnectionHandler::GetGroup(const GroupId &id)
    {
        return groups[id];
    }

    const SubscriptionObject &QvConnectionHandler::GetSubscription(const SubscriptionId &id)
    {
        return subscriptions[id];
    }

    QvConnectionHandler::~QvConnectionHandler()
    {
        if (kernelInstance->KernelStarted) {
            kernelInstance->StopConnection();
            LOG(MODULE_CORE_HANDLER, "Stopped connection from destructor.")
        }

        delete kernelInstance;
    }
}
