#include "ConnectionWidget.hpp"

ConnectionWidget::ConnectionWidget(QWidget *parent) : QWidget(parent), groupId("null"), subscriptionId("null")
{
    setupUi(this);
    connect(ConnectionHandler, &QvConnectionHandler::OnConnected, this, &ConnectionWidget::OnConnected);
}

ConnectionWidget::ConnectionWidget(const ConnectionIdentifier &identifier, QWidget *parent): ConnectionWidget(parent)
{
    auto connection = ConnectionHandler->GetConnection(identifier.connectionId);
    connectionIdentifier = identifier;
    groupId = identifier.groupId;
    subscriptionId = identifier.subscriptionId;
    itemType = NODE_ITEM;
    rawDisplayName = connection.displayName;
    connNameLabel->setText(rawDisplayName);
    latencyLabel->setText(tr("Latency: ") + QSTRN(connection.latency) + " " + tr("ms"));
}

// ======================================= Initialisation for root nodes.
ConnectionWidget::ConnectionWidget(const GroupId &id, QWidget *parent) : ConnectionWidget(parent)
{
    groupId = id;
    itemType = GROUP_HEADER_ITEM;
    InitialiseForGroup(ConnectionHandler->GetGroup(id).displayName, ConnectionHandler->Connections(id).count());
}

ConnectionWidget::ConnectionWidget(const SubscriptionId &id, QWidget *parent) : ConnectionWidget(parent)
{
    subscriptionId = id;
    itemType = SUBS_HEADER_ITEM;
    InitialiseForGroup(ConnectionHandler->GetSubscription(id).displayName, ConnectionHandler->Connections(id).count());
}

void ConnectionWidget::InitialiseForGroup(const QString &displayName, int connectionCount)
{
    rawDisplayName = displayName;
    connNameLabel->setText(rawDisplayName);
    latencyLabel->setText(QSTRN(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
    //
    layout()->removeWidget(connTypeLabel);
    layout()->removeWidget(dataLabel);
    delete connTypeLabel;
    delete dataLabel;
}

void ConnectionWidget::BeginConnection()
{
    if (itemType == NODE_ITEM) {
        ConnectionHandler->StartConnection(connectionIdentifier);
    } else {
        LOG(MODULE_UI, "Trying to start a non-connection entry, this call is illegal.")
    }
}

void ConnectionWidget::OnConnected(const ConnectionId &id)
{
    if (id == connectionIdentifier.connectionId) {
        LOG(MODULE_UI, "OnConnected signal received for: " + id.toString())
        emit RequestWidgetFocus(this);
    }
}

ConnectionWidget::~ConnectionWidget()
{
    //
}
