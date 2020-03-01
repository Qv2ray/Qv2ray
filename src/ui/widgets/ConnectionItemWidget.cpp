#include "ConnectionItemWidget.hpp"

#include "common/QvHelpers.hpp"

#include <QStyleFactory>

ConnectionItemWidget::ConnectionItemWidget(QWidget *parent) : QWidget(parent), connectionId("null"), groupId("null")
{
    setupUi(this);
    connect(ConnectionManager, &QvConnectionHandler::OnConnected, this, &ConnectionItemWidget::OnConnected);
    connect(ConnectionManager, &QvConnectionHandler::OnDisconnected, this, &ConnectionItemWidget::OnDisConnected);
    connect(ConnectionManager, &QvConnectionHandler::OnStatsAvailable, this, &ConnectionItemWidget::OnConnectionStatsArrived);
    connect(ConnectionManager, &QvConnectionHandler::OnLatencyTestStarted, this, &ConnectionItemWidget::OnLatencyTestStart);
    connect(ConnectionManager, &QvConnectionHandler::OnLatencyTestFinished, this, &ConnectionItemWidget::OnLatencyTestFinished);
}

ConnectionItemWidget::ConnectionItemWidget(const ConnectionId &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    connectionId = id;
    groupId = ConnectionManager->GetConnectionGroupId(id);
    originalItemName = ConnectionManager->GetDisplayName(id);
    itemType = NODE_ITEM;
    auto latency = ConnectionManager->GetConnectionLatency(id);

    if (latency == 0)
    {
        latencyLabel->setText(tr("Not Tested"));
    }
    else
    {
        latencyLabel->setText(QSTRN(latency) + " " + tr("ms"));
    }

    connTypeLabel->setText(tr("Type: ") + ConnectionManager->GetConnectionProtocolString(id));
    auto [uplink, downlink] = ConnectionManager->GetConnectionUsageAmount(connectionId);
    dataLabel->setText(FormatBytes(uplink) + " / " + FormatBytes(downlink));
    //
    indentSpacer->changeSize(10, indentSpacer->sizeHint().height());
    //
    if (ConnectionManager->IsConnected(id))
    {
        emit RequestWidgetFocus(this);
    }
    OnConnectionItemRenamed(id, "", originalItemName);
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionRenamed, this, &ConnectionItemWidget::OnConnectionItemRenamed);
}

// ======================================= Initialisation for root nodes.
ConnectionItemWidget::ConnectionItemWidget(const GroupId &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    layout()->removeWidget(connTypeLabel);
    layout()->removeWidget(dataLabel);
    delete connTypeLabel;
    delete dataLabel;
    //
    groupId = id;
    itemType = GROUP_HEADER_ITEM;
    originalItemName = ConnectionManager->GetDisplayName(id);
    RecalculateConnectionsCount();
    //
    auto font = connNameLabel->font();
    font.setBold(true);
    connNameLabel->setFont(font);
    //
    OnGroupItemRenamed(id, "", originalItemName);
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionCreated, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionDeleted, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionChanged, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionGroupChanged, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConnectionHandler::OnSubscriptionUpdateFinished, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    //
    connect(ConnectionManager, &QvConnectionHandler::OnGroupRenamed, this, &ConnectionItemWidget::OnGroupItemRenamed);
}

void ConnectionItemWidget::BeginConnection()
{
    if (itemType == NODE_ITEM)
    {
        ConnectionManager->StartConnection(connectionId);
    }
    else
    {
        LOG(MODULE_UI, "Trying to start a non-connection entry, this call is illegal.")
    }
}

void ConnectionItemWidget::OnConnected(const ConnectionId &id)
{
    if (id == connectionId)
    {
        connNameLabel->setText("• " + originalItemName);
        LOG(MODULE_UI, "OnConnected signal received for: " + id.toString())
        emit RequestWidgetFocus(this);
    }
}

void ConnectionItemWidget::OnDisConnected(const ConnectionId &id)
{
    if (id == connectionId)
    {
        connNameLabel->setText(originalItemName);
    }
}

void ConnectionItemWidget::OnConnectionStatsArrived(const ConnectionId &id, const quint64 upSpeed, const quint64 downSpeed,
                                                    const quint64 totalUp, const quint64 totalDown)
{
    Q_UNUSED(upSpeed)
    Q_UNUSED(downSpeed)

    if (id == connectionId)
    {
        dataLabel->setText(FormatBytes(totalUp) + " / " + FormatBytes(totalDown));
    }
}

void ConnectionItemWidget::OnLatencyTestStart(const ConnectionId &id)
{
    if (id == connectionId)
    {
        latencyLabel->setText(tr("Testing..."));
    }
}
void ConnectionItemWidget::OnLatencyTestFinished(const ConnectionId &id, const uint average)
{
    if (id == connectionId)
    {
        if (average == 0)
        {
            latencyLabel->setText(tr("Error"));
        }
        else
        {
            latencyLabel->setText(QSTRN(average) + tr("ms"));
        }
    }
}

void ConnectionItemWidget::BeginRename()
{
    stackedWidget->setCurrentIndex(1);
    renameTxt->setStyle(QStyleFactory::create("Fusion"));
    renameTxt->setStyleSheet("background-color: " + this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
    renameTxt->setText(originalItemName);
}

ConnectionItemWidget::~ConnectionItemWidget()
{
}

void ConnectionItemWidget::on_doRenameBtn_clicked()
{
    if (renameTxt->text().isEmpty())
        return;
    if (connectionId == NullConnectionId)
    {
        ConnectionManager->RenameGroup(groupId, renameTxt->text());
    }
    else
    {
        ConnectionManager->RenameConnection(connectionId, renameTxt->text());
    }
    stackedWidget->setCurrentIndex(0);
}
