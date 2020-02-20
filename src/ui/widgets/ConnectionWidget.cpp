#include "ConnectionWidget.hpp"
#include "common/QvHelpers.hpp"

ConnectionWidget::ConnectionWidget(QWidget *parent) : QWidget(parent), connectionId("null"), groupId("null")
{
    setupUi(this);
    connect(ConnectionHandler, &QvConnectionHandler::OnConnected, this, &ConnectionWidget::OnConnected);
}

ConnectionWidget::ConnectionWidget(const ConnectionId &identifier, QWidget *parent): ConnectionWidget(parent)
{
    auto connection = ConnectionHandler->GetConnection(identifier);
    connectionId = identifier;
    groupId = connection.groupId;
    itemType = NODE_ITEM;
    connNameLabel->setText(connection.displayName);
    latencyLabel->setText(QSTRN(connection.latency) + " " + tr("ms"));
    connTypeLabel->setText("DODO");
    dataLabel->setText(FormatBytes(connection.upLinkData + connection.downLinkData));
}

// ======================================= Initialisation for root nodes.
ConnectionWidget::ConnectionWidget(const GroupId &id, QWidget *parent) : ConnectionWidget(parent)
{
    groupId = id;
    itemType = GROUP_HEADER_ITEM;
    auto displayName = ConnectionHandler->GetGroup(id).displayName;
    auto connectionCount = ConnectionHandler->Connections(id).count();
    connNameLabel->setText(displayName);
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
        ConnectionHandler->StartConnection(connectionId);
    } else {
        LOG(MODULE_UI, "Trying to start a non-connection entry, this call is illegal.")
    }
}

void ConnectionWidget::OnConnected(const ConnectionId &id)
{
    if (id == connectionId) {
        LOG(MODULE_UI, "OnConnected signal received for: " + id.toString())
        emit RequestWidgetFocus(this);
    }
}

ConnectionWidget::~ConnectionWidget()
{
    //
}
