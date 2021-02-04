#include "ConnectionItemWidget.hpp"

#include "core/handler/ConfigHandler.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"

#include <QStyleFactory>

#define QV_MODULE_NAME "ConnectionItemWidget"

ConnectionItemWidget::ConnectionItemWidget(QWidget *parent) : QWidget(parent), connectionId(NullConnectionId), groupId(NullGroupId)
{
    setupUi(this);
    connect(ConnectionManager, &QvConfigHandler::OnConnected, this, &ConnectionItemWidget::OnConnected);
    connect(ConnectionManager, &QvConfigHandler::OnDisconnected, this, &ConnectionItemWidget::OnDisConnected);
    connect(ConnectionManager, &QvConfigHandler::OnStatsAvailable, this, &ConnectionItemWidget::OnConnectionStatsArrived);
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestStarted, this, &ConnectionItemWidget::OnLatencyTestStart);
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestFinished, this, &ConnectionItemWidget::OnLatencyTestFinished);
}

ConnectionItemWidget::ConnectionItemWidget(const ConnectionGroupPair &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    connectionId = id.connectionId;
    groupId = id.groupId;
    originalItemName = GetDisplayName(id.connectionId);
    //
    indentSpacer->changeSize(10, indentSpacer->sizeHint().height());
    //
    auto latency = GetConnectionLatency(id.connectionId);
    latencyLabel->setText(latency == LATENCY_TEST_VALUE_NODATA ?     //
                              tr("Not Tested") :                     //
                              (latency == LATENCY_TEST_VALUE_ERROR ? //
                                   tr("Error") :                     //
                                   (QSTRN(latency) + " ms")));       //
    //
    connTypeLabel->setText(GetConnectionProtocolString(id.connectionId).toUpper());
    auto [uplink, downlink] = GetConnectionUsageAmount(connectionId);
    dataLabel->setText(FormatBytes(uplink) + " / " + FormatBytes(downlink));
    //
    if (ConnectionManager->IsConnected(id))
    {
        emit RequestWidgetFocus(this);
    }
    // Fake trigger
    OnConnectionItemRenamed(id.connectionId, "", originalItemName);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRenamed, this, &ConnectionItemWidget::OnConnectionItemRenamed);
    //
    // Rename events
    connect(renameTxt, &QLineEdit::returnPressed, this, &ConnectionItemWidget::on_doRenameBtn_clicked);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionModified, this, &ConnectionItemWidget::OnConnectionModified);
}

// ======================================= Initialisation for root nodes.
ConnectionItemWidget::ConnectionItemWidget(const GroupId &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    layout()->removeWidget(connTypeLabel);
    layout()->removeWidget(dataLabel);
    delete connTypeLabel;
    delete dataLabel;
    //
    delete doRenameBtn;
    delete renameTxt;
    //
    groupId = id;
    originalItemName = GetDisplayName(id);
    RecalculateConnectionsCount();
    //
    auto font = connNameLabel->font();
    font.setBold(true);
    connNameLabel->setFont(font);
    //
    OnGroupItemRenamed(id, "", originalItemName);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionModified, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnSubscriptionAsyncUpdateFinished, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRemovedFromGroup, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    //
    connect(ConnectionManager, &QvConfigHandler::OnGroupRenamed, this, &ConnectionItemWidget::OnGroupItemRenamed);
}

void ConnectionItemWidget::BeginConnection()
{
    if (IsConnection())
    {
        ConnectionManager->StartConnection({ connectionId, groupId });
    }
    else
    {
        LOG("Trying to start a non-connection entry, this call is illegal.");
    }
}

bool ConnectionItemWidget::NameMatched(const QString &arg) const
{
    auto searchString = arg.toLower();
    auto isGroupNameMatched = GetDisplayName(groupId).toLower().contains(arg);

    if (IsConnection())
    {
        return isGroupNameMatched || GetDisplayName(connectionId).toLower().contains(searchString);
    }
    else
    {
        return isGroupNameMatched;
    }
}

void ConnectionItemWidget::RecalculateConnectionsCount()
{
    auto connectionCount = ConnectionManager->GetConnections(groupId).count();
    latencyLabel->setText(QSTRN(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
    OnGroupItemRenamed(groupId, "", originalItemName);
}

void ConnectionItemWidget::OnConnected(const ConnectionGroupPair &id)
{
    if (id == ConnectionGroupPair{ connectionId, groupId })
    {
        connNameLabel->setText("● " + originalItemName);
        DEBUG("ConnectionItemWidgetOnConnected signal received for: " + id.connectionId.toString());
        emit RequestWidgetFocus(this);
    }
}

void ConnectionItemWidget::OnDisConnected(const ConnectionGroupPair &id)
{
    if (id == ConnectionGroupPair{ connectionId, groupId })
    {
        connNameLabel->setText(originalItemName);
    }
}

void ConnectionItemWidget::OnConnectionStatsArrived(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeedData> &data)
{
    if (id.connectionId == connectionId)
    {
        dataLabel->setText(FormatBytes(data[CurrentStatAPIType].second.first) + " / " + FormatBytes(data[CurrentStatAPIType].second.second));
    }
}

void ConnectionItemWidget::OnConnectionModified(const ConnectionId &id)
{
    if (connectionId == id)
        connTypeLabel->setText(GetConnectionProtocolString(id).toUpper());
}

void ConnectionItemWidget::OnLatencyTestStart(const ConnectionId &id)
{
    if (id == connectionId)
    {
        latencyLabel->setText(tr("Testing..."));
    }
}
void ConnectionItemWidget::OnLatencyTestFinished(const ConnectionId &id, const int average)
{
    if (id == connectionId)
    {
        latencyLabel->setText(average == LATENCY_TEST_VALUE_ERROR ? tr("Error") : QSTRN(average) + tr("ms"));
    }
}

void ConnectionItemWidget::CancelRename()
{
    stackedWidget->setCurrentIndex(0);
}

void ConnectionItemWidget::BeginRename()
{
    if (IsConnection())
    {
        stackedWidget->setCurrentIndex(1);
        renameTxt->setStyle(QStyleFactory::create("Fusion"));
        renameTxt->setStyleSheet("background-color: " + this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
        renameTxt->setText(originalItemName);
        renameTxt->setFocus();
    }
}

ConnectionItemWidget::~ConnectionItemWidget()
{
}

void ConnectionItemWidget::on_doRenameBtn_clicked()
{
    if (renameTxt->text().isEmpty())
        return;
    if (connectionId == NullConnectionId)
        ConnectionManager->RenameGroup(groupId, renameTxt->text());
    else
        ConnectionManager->RenameConnection(connectionId, renameTxt->text());
    stackedWidget->setCurrentIndex(0);
}

void ConnectionItemWidget::OnConnectionItemRenamed(const ConnectionId &id, const QString &, const QString &newName)
{
    if (id == connectionId)
    {
        connNameLabel->setText((ConnectionManager->IsConnected({ connectionId, groupId }) ? "● " : "") + newName);
        originalItemName = newName;
        const auto conn = ConnectionManager->GetConnectionMetaObject(connectionId);
        this->setToolTip(newName +                                                             //
                         NEWLINE + tr("Last Connected: ") + timeToString(conn.lastConnected) + //
                         NEWLINE + tr("Last Updated: ") + timeToString(conn.lastUpdatedDate));
    }
}

void ConnectionItemWidget::OnGroupItemRenamed(const GroupId &id, const QString &, const QString &newName)
{
    if (id == groupId)
    {
        originalItemName = newName;
        connNameLabel->setText(newName);
        const auto grp = ConnectionManager->GetGroupMetaObject(id);
        this->setToolTip(newName + NEWLINE +                                          //
                         (grp.isSubscription ? (tr("Subscription") + NEWLINE) : "") + //
                         tr("Last Updated: ") + timeToString(grp.lastUpdatedDate));
    }
}
