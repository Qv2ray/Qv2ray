#include "ConnectionModelHelper.hpp"

#include "Qv2rayBase/Common/ProfileHelpers.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Plugin/LatencyTestHost.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"
#include "components/QueryParser/QueryParser.hpp"
#include "ui/widgets/ConnectionItemWidget.hpp"

const auto NumericString = [](auto i) { return u"%1"_qs.arg(i, 30, 10, QChar('0')); };

ConnectionListHelper::ConnectionListHelper(QTreeView *parentView, QObject *parent) : QObject(parent)
{
    this->parentView = parentView;
    model = new QStandardItemModel();
    parentView->setModel(model);
    for (const auto &group : QvProfileManager->GetGroups())
    {
        addGroupItem(group);
        for (const auto &connection : QvProfileManager->GetConnections(group))
            addConnectionItem({ connection, group });
    }

    const auto renamedLambda = [&](const ConnectionId &id, const QString &, const QString &newName)
    {
        for (const auto &gid : QvProfileManager->GetGroups(id))
        {
            ProfileId pair{ id, gid };
            if (pairs.contains(pair))
                pairs[pair]->setData(newName, ROLE_DISPLAYNAME);
        }
    };

    const auto latencyLambda = [&](const ConnectionId &id, const Qv2rayPlugin::Latency::LatencyTestResponse &data)
    {
        for (const auto &gid : QvProfileManager->GetGroups(id))
        {
            ProfileId pair{ id, gid };
            if (pairs.contains(pair))
                pairs[pair]->setData(NumericString(data.avg), ROLE_LATENCY);
        }
    };

    const auto statsLambda = [&](const ProfileId &id, const StatisticsObject &)
    {
        if (connections.contains(id.connectionId))
        {
            for (const auto &index : connections[id.connectionId])
                index->setData(NumericString(GetConnectionTotalUsage(id.connectionId, StatisticsObject::PROXY)), ROLE_DATA_USAGE);
        }
    };

    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionRemovedFromGroup, this, &ConnectionListHelper::OnConnectionDeleted);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionCreated, this, &ConnectionListHelper::addConnectionItem);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionLinkedWithGroup, this, &ConnectionListHelper::addConnectionItem);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnGroupCreated, this, &ConnectionListHelper::addGroupItem);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnGroupDeleted, this, &ConnectionListHelper::OnGroupDeleted);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionRenamed, this, renamedLambda);
    connect(QvLatencyTestHost, &Qv2rayBase::Plugin::LatencyTestHost::OnLatencyTestCompleted, this, latencyLambda);

    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnStatsDataAvailable, this, statsLambda);
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

void ConnectionListHelper::Filter(const QString &_key)
{
    const auto lowerKey = _key.toLower();
    for (const auto &groupId : QvProfileManager->GetGroups())
    {
        const auto groupIndex = model->indexFromItem(groups[groupId]);
        bool isTotallyHide = true;
        for (const auto &connectionId : QvProfileManager->GetConnections(groupId))
        {
            bool hasMatch = GetDisplayName(connectionId).toLower().contains(lowerKey);
            const auto connectionIndex = model->indexFromItem(pairs[{ connectionId, groupId }]);
            parentView->setRowHidden(connectionIndex.row(), connectionIndex.parent(), !hasMatch);
            isTotallyHide &= hasMatch;
        }
        parentView->indexWidget(groupIndex)->setHidden(isTotallyHide);
        if (!isTotallyHide)
            parentView->expand(groupIndex);
    }
}

void ConnectionListHelper::Filter(const components::QueryParser::SemanticAnalyzer::Program &program)
{
    for (const auto &groupId : QvProfileManager->GetGroups())
    {
        const auto groupIndex = model->indexFromItem(groups[groupId]);
        bool isTotallyHide = true;
        for (const auto &connectionId : QvProfileManager->GetConnections(groupId))
        {
            const auto conn = QvProfileManager->GetConnection(connectionId);
            const auto connObject = QvProfileManager->GetConnectionObject(connectionId);
            QVariantMap variables{
                { u"group"_qs, GetDisplayName(groupId) },
                { u"name"_qs, GetDisplayName(connectionId) },
                { u"tags"_qs, QVariantList(connObject.tags.begin(), connObject.tags.end()) },
                { u"latency"_qs, connObject.latency },
                { u"outbounds"_qs, conn.outbounds.count() },
                { u"inbounds"_qs, conn.inbounds.count() },
                { u"connected"_qs, QvKernelManager->CurrentConnection() == ProfileId{ connectionId, groupId } },
            };

            if (!conn.outbounds.isEmpty())
            {
                variables.insert(u"protocol"_qs, conn.outbounds.first().outboundSettings.protocol);
                variables.insert(u"address"_qs, conn.outbounds.first().outboundSettings.address);
                variables.insert(u"port"_qs, conn.outbounds.first().outboundSettings.port.from);
            }

            bool hasMatch = Qv2ray::components::QueryParser::EvaluateProgram(program, variables, Qt::CaseInsensitive);

            const auto connectionIndex = model->indexFromItem(pairs[{ connectionId, groupId }]);
            parentView->setRowHidden(connectionIndex.row(), connectionIndex.parent(), !hasMatch);
            isTotallyHide &= hasMatch;
        }
        parentView->indexWidget(groupIndex)->setHidden(isTotallyHide);
        if (!isTotallyHide)
            parentView->expand(groupIndex);
    }
}

QStandardItem *ConnectionListHelper::addConnectionItem(const ProfileId &id)
{
    // Create Standard Item
    auto connectionItem = new QStandardItem();
    connectionItem->setData(GetDisplayName(id.connectionId), ConnectionInfoRole::ROLE_DISPLAYNAME);
    connectionItem->setData(NumericString(GetConnectionLatency(id.connectionId)), ConnectionInfoRole::ROLE_LATENCY);
    connectionItem->setData(NumericString(GetConnectionTotalUsage(id.connectionId, StatisticsObject::PROXY)), ConnectionInfoRole::ROLE_DATA_USAGE);
    //
    // Find groups
    if (!groups.contains(id.groupId))
        qInfo() << "Unexpected";
    const auto groupIndex = groups.contains(id.groupId) ? groups[id.groupId] : addGroupItem(id.groupId);
    // Append into model
    groupIndex->appendRow(connectionItem);
    const auto connectionIndex = connectionItem->index();
    //
    auto widget = new ConnectionItemWidget(id);
    parentView->setIndexWidget(connectionIndex, widget);
    pairs[id] = connectionItem;
    connections[id.connectionId].append(connectionItem);
    return connectionItem;
}

QStandardItem *ConnectionListHelper::addGroupItem(const GroupId &groupId)
{
    // Create Item
    const auto item = new QStandardItem;
    // Set item into model
    model->appendRow(item);
    // Get item index
    parentView->setIndexWidget(item->index(), new ConnectionItemWidget(groupId, parentView));
    groups[groupId] = item;
    return item;
}

void ConnectionListHelper::OnConnectionDeleted(const ProfileId &id)
{
    auto item = pairs.take(id);
    const auto index = model->indexFromItem(item);
    if (!index.isValid())
        return;
    model->removeRow(index.row(), index.parent());
    connections[id.connectionId].removeAll(item);
}

void ConnectionListHelper::OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections)
{
    for (const auto &conn : connections)
    {
        const ProfileId pair{ conn, id };
        OnConnectionDeleted(pair);
    }
    const auto item = groups.take(id);
    const auto index = model->indexFromItem(item);
    if (!index.isValid())
        return;
    model->removeRow(index.row(), index.parent());
}
