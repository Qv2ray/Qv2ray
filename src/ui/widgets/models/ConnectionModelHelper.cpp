#include "ConnectionModelHelper.hpp"

#include "core/handler/ConfigHandler.hpp"
#include "ui/widgets/widgets/ConnectionItemWidget.hpp"

#define NumericString(i) (QString("%1").arg(i, 30, 10, QLatin1Char('0')))

ConnectionListHelper::ConnectionListHelper(QTreeView *view, QObject *parent) : QObject(parent)
{
    parentView = view;
    model = new QStandardItemModel();
    view->setModel(model);
    for (const auto &group : ConnectionManager->AllGroups())
    {
        addGroupItem(group);
        for (const auto &connection : ConnectionManager->GetConnections(group))
        {
            addConnectionItem({ connection, group });
        }
    }
    const auto renamedLambda = [&](const ConnectionId &id, const QString &, const QString &newName) {
        for (const auto &gid : ConnectionManager->GetConnectionContainedIn(id))
        {
            ConnectionGroupPair pair{ id, gid };
            if (pairs.contains(pair))
                pairs[pair]->setData(newName, ROLE_DISPLAYNAME);
        }
    };

    const auto latencyLambda = [&](const ConnectionId &id, const int avg) {
        for (const auto &gid : ConnectionManager->GetConnectionContainedIn(id))
        {
            ConnectionGroupPair pair{ id, gid };
            if (pairs.contains(pair))
                pairs[pair]->setData(NumericString(avg), ROLE_LATENCY);
        }
    };

    const auto statsLambda = [&](const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeedData> &data) {
        if (connections.contains(id.connectionId))
        {
            for (const auto &index : connections[id.connectionId])
                index->setData(NumericString(GetConnectionTotalData(id.connectionId)), ROLE_DATA_USAGE);
        }
    };

    connect(ConnectionManager, &QvConfigHandler::OnConnectionRemovedFromGroup, this, &ConnectionListHelper::OnConnectionDeleted);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, this, &ConnectionListHelper::OnConnectionCreated);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, this, &ConnectionListHelper::OnConnectionLinkedWithGroup);
    connect(ConnectionManager, &QvConfigHandler::OnGroupCreated, this, &ConnectionListHelper::OnGroupCreated);
    connect(ConnectionManager, &QvConfigHandler::OnGroupDeleted, this, &ConnectionListHelper::OnGroupDeleted);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRenamed, renamedLambda);
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestFinished, latencyLambda);
    connect(ConnectionManager, &QvConfigHandler::OnStatsAvailable, statsLambda);
}

ConnectionListHelper::~ConnectionListHelper()
{
    delete model;
}

void ConnectionListHelper::Sort(ConnectionInfoRole role, Qt::SortOrder order)
{
    model->setSortRole(role);
    model->sort(0, order);
}

void ConnectionListHelper::Filter(const QString &key)
{
    for (const auto &groupId : ConnectionManager->AllGroups())
    {
        const auto groupItem = model->indexFromItem(groups[groupId]);
        bool isTotallyHide = true;
        for (const auto &connectionId : ConnectionManager->GetConnections(groupId))
        {
            const auto connectionItem = model->indexFromItem(pairs[{ connectionId, groupId }]);
            const auto willTotallyHide = static_cast<ConnectionItemWidget *>(parentView->indexWidget(connectionItem))->NameMatched(key);
            parentView->setRowHidden(connectionItem.row(), connectionItem.parent(), !willTotallyHide);
            isTotallyHide &= willTotallyHide;
        }
        parentView->indexWidget(groupItem)->setHidden(isTotallyHide);
        if (!isTotallyHide)
            parentView->expand(groupItem);
    }
}

QStandardItem *ConnectionListHelper::addConnectionItem(const ConnectionGroupPair &id)
{
    // Create Standard Item
    auto connectionItem = new QStandardItem();
    connectionItem->setData(GetDisplayName(id.connectionId), ConnectionInfoRole::ROLE_DISPLAYNAME);
    connectionItem->setData(NumericString(GetConnectionLatency(id.connectionId)), ConnectionInfoRole::ROLE_LATENCY);
    connectionItem->setData(NumericString(GetConnectionTotalData(id.connectionId)), ConnectionInfoRole::ROLE_DATA_USAGE);
    //
    // Find groups
    const auto groupIndex = groups.contains(id.groupId) ? groups[id.groupId] : addGroupItem(id.groupId);
    // Append into model
    groupIndex->appendRow(connectionItem);
    const auto connectionIndex = connectionItem->index();
    //
    auto widget = new ConnectionItemWidget(id, parentView);
    connect(widget, &ConnectionItemWidget::RequestWidgetFocus, [widget, connectionIndex, this]() {
        parentView->setCurrentIndex(connectionIndex);
        parentView->scrollTo(connectionIndex);
        parentView->clicked(connectionIndex);
    });
    //
    parentView->setIndexWidget(connectionIndex, widget);
    pairs[id] = connectionItem;
    connections[id.connectionId].append(connectionItem);
    return connectionItem;
}

QStandardItem *ConnectionListHelper::addGroupItem(const GroupId &groupId)
{
    // Create Item
    const auto item = new QStandardItem();
    // Set item into model
    model->appendRow(item);
    // Get item index
    const auto index = item->index();
    parentView->setIndexWidget(index, new ConnectionItemWidget(groupId, parentView));
    groups[groupId] = item;
    return item;
}

void ConnectionListHelper::OnConnectionCreated(const ConnectionGroupPair &id, const QString &)
{
    addConnectionItem(id);
}

void ConnectionListHelper::OnConnectionDeleted(const ConnectionGroupPair &id)
{
    auto item = pairs.take(id);
    const auto index = model->indexFromItem(item);
    if (!index.isValid())
        return;
    model->removeRow(index.row(), index.parent());
    connections[id.connectionId].removeAll(item);
}

void ConnectionListHelper::OnConnectionLinkedWithGroup(const ConnectionGroupPair &pairId)
{
    addConnectionItem(pairId);
}

void ConnectionListHelper::OnGroupCreated(const GroupId &id, const QString &)
{
    addGroupItem(id);
}

void ConnectionListHelper::OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections)
{
    for (const auto &conn : connections)
    {
        const ConnectionGroupPair pair{ conn, id };
        OnConnectionDeleted(pair);
    }
    const auto item = groups.take(id);
    const auto index = model->indexFromItem(item);
    if (!index.isValid())
        return;
    model->removeRow(index.row(), index.parent());
}
