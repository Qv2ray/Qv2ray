#include "ConnectionItemWidget.hpp"
#include "common/QvHelpers.hpp"

ConnectionItemWidget::ConnectionItemWidget(QWidget *parent) : QWidget(parent), connectionId("null"), groupId("null")
{
    setupUi(this);
    connect(ConnectionManager, &QvConnectionHandler::OnConnected, this, &ConnectionItemWidget::OnConnected);
}

ConnectionItemWidget::ConnectionItemWidget(const ConnectionId &identifier, QWidget *parent): ConnectionItemWidget(parent)
{
    auto connection = ConnectionManager->GetConnection(identifier);
    connectionId = identifier;
    groupId = connection.groupId;
    itemType = NODE_ITEM;
    connNameLabel->setText("" + connection.displayName);
    latencyLabel->setText(QSTRN(connection.latency) + " " + tr("ms"));
    connTypeLabel->setText(tr("Type: ") + ConnectionManager->GetConnectionBasicInfo(identifier));
    dataLabel->setText(FormatBytes(connection.upLinkData + connection.downLinkData));
    //
    indentSpacer->changeSize(10, indentSpacer->sizeHint().height());
}

// ======================================= Initialisation for root nodes.
ConnectionItemWidget::ConnectionItemWidget(const GroupId &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    groupId = id;
    itemType = GROUP_HEADER_ITEM;
    auto displayName = ConnectionManager->GetGroup(id).displayName;
    auto connectionCount = ConnectionManager->Connections(id).count();
    connNameLabel->setText(/*"• " +*/ displayName);
    latencyLabel->setText(QSTRN(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
    //
    layout()->removeWidget(connTypeLabel);
    layout()->removeWidget(dataLabel);
    delete connTypeLabel;
    delete dataLabel;
}


void ConnectionItemWidget::BeginConnection()
{
    if (itemType == NODE_ITEM) {
        ConnectionManager->StartConnection(connectionId);
    } else {
        LOG(MODULE_UI, "Trying to start a non-connection entry, this call is illegal.")
    }
}

void ConnectionItemWidget::OnConnected(const ConnectionId &id)
{
    if (id == connectionId) {
        LOG(MODULE_UI, "OnConnected signal received for: " + id.toString())
        emit RequestWidgetFocus(this);
    }
}

ConnectionItemWidget::~ConnectionItemWidget()
{
    //
}
