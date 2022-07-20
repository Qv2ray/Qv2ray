#include "ConnectionItemWidget.hpp"

#include "Qv2rayBase/Common/ProfileHelpers.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Plugin/LatencyTestHost.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "ui/WidgetUIBase.hpp"

#include <QStyleFactory>

ConnectionItemWidget::ConnectionItemWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnConnected, this, &ConnectionItemWidget::OnConnected);
    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnDisconnected, this, &ConnectionItemWidget::OnDisConnected);
    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnStatsDataAvailable, this, &ConnectionItemWidget::OnConnectionStatsArrived);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnLatencyTestStarted, this, &ConnectionItemWidget::OnLatencyTestStart);
    connect(QvLatencyTestHost, &Qv2rayBase::Plugin::LatencyTestHost::OnLatencyTestCompleted, this, &ConnectionItemWidget::OnLatencyTestFinished);
}

ConnectionItemWidget::ConnectionItemWidget(const ProfileId &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    connectionId = id.connectionId;
    groupId = id.groupId;
    originalItemName = GetDisplayName(id.connectionId);
    //
    indentSpacer->changeSize(10, indentSpacer->sizeHint().height());
    //
    auto latency = GetConnectionLatency(id.connectionId);
    const auto latencyString = [](int latency)
    {
        if (latency == LATENCY_TEST_VALUE_NODATA)
            return tr("Not Tested");
        else if (latency == LATENCY_TEST_VALUE_ERROR)
            return tr("Error");
        else
            return QString::number(latency) + u" ms"_qs;
    }(latency);

    latencyLabel->setText(latencyString);

    connTypeLabel->setText(GetConnectionProtocolDescription(id.connectionId).toUpper());
    const auto [uplink, downlink] = GetConnectionUsageAmount(connectionId, StatisticsObject::PROXY);
    dataLabel->setText(u"%1 / %2"_qs.arg(FormatBytes(uplink), FormatBytes(downlink)));

    // Fake trigger
    OnConnectionItemRenamed(id.connectionId, u""_qs, originalItemName);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionRenamed, this, &ConnectionItemWidget::OnConnectionItemRenamed);
    //
    // Rename events
    connect(renameTxt, &QLineEdit::returnPressed, this, &ConnectionItemWidget::on_doRenameBtn_clicked);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionModified, this, &ConnectionItemWidget::OnConnectionModified);
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
    RecalculateConnections();
    //
    auto font = connNameLabel->font();
    font.setBold(true);
    connNameLabel->setFont(font);
    //
    OnGroupItemRenamed(id, u""_qs, originalItemName);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionCreated, this, &ConnectionItemWidget::RecalculateConnections);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionModified, this, &ConnectionItemWidget::RecalculateConnections);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionLinkedWithGroup, this, &ConnectionItemWidget::RecalculateConnections);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnSubscriptionUpdateFinished, this, &ConnectionItemWidget::RecalculateConnections);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionRemovedFromGroup, this, &ConnectionItemWidget::RecalculateConnections);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnGroupRenamed, this, &ConnectionItemWidget::OnGroupItemRenamed);
}

void ConnectionItemWidget::BeginConnection() const
{
    assert(IsConnection());
    QvProfileManager->StartConnection({ connectionId, groupId });
}

void ConnectionItemWidget::RecalculateConnections()
{
    auto connectionCount = QvProfileManager->GetConnections(groupId).count();
    latencyLabel->setText(QString::number(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
    OnGroupItemRenamed(groupId, u""_qs, originalItemName);
}

void ConnectionItemWidget::OnConnected(const ProfileId &id)
{
    if (id == ProfileId{ connectionId, groupId })
    {
        connNameLabel->setText("● " + originalItemName);
        qDebug() << "ConnectionItemWidgetOnConnected signal received for:" << id.connectionId;
    }
}

void ConnectionItemWidget::OnDisConnected(const ProfileId &id)
{
    if (id == ProfileId{ connectionId, groupId })
    {
        connNameLabel->setText(originalItemName);
    }
}

void ConnectionItemWidget::OnConnectionStatsArrived(const ProfileId &id, const StatisticsObject &)
{
    if (id.connectionId == connectionId)
    {
        const auto &[up, down] = Qv2rayBase::Utils::GetConnectionUsageAmount(id.connectionId, StatisticsObject::PROXY);
        dataLabel->setText(FormatBytes(up) + " / " + FormatBytes(down));
    }
}

void ConnectionItemWidget::OnConnectionModified(const ConnectionId &id)
{
    if (connectionId == id)
        connTypeLabel->setText(GetConnectionProtocolDescription(id).toUpper());
}

void ConnectionItemWidget::OnLatencyTestStart(const ConnectionId &id)
{
    if (id == connectionId)
    {
        latencyLabel->setText(tr("Testing..."));
    }
}
void ConnectionItemWidget::OnLatencyTestFinished(const ConnectionId &id, const Qv2rayPlugin::Latency::LatencyTestResponse &data)
{
    if (id == connectionId)
    {
        latencyLabel->setText(data.avg == LATENCY_TEST_VALUE_ERROR ? tr("Error") : QString::number(data.avg) + tr("ms"));
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
        renameTxt->setStyle(QStyleFactory::create(u"Fusion"_qs));
        renameTxt->setStyleSheet("background-color: " + this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
        renameTxt->setText(originalItemName);
        renameTxt->setFocus();
    }
}

void ConnectionItemWidget::on_doRenameBtn_clicked()
{
    if (renameTxt->text().isEmpty())
        return;
    if (connectionId.isNull())
        QvProfileManager->RenameGroup(groupId, renameTxt->text());
    else
        QvProfileManager->RenameConnection(connectionId, renameTxt->text());
    stackedWidget->setCurrentIndex(0);
}

void ConnectionItemWidget::OnConnectionItemRenamed(const ConnectionId &id, const QString &, const QString &newName)
{
    if (id == connectionId)
    {
        connNameLabel->setText(QvProfileManager->IsConnected({ connectionId, groupId }) ? u"● "_qs + newName : newName);
        originalItemName = newName;
        const auto conn = QvProfileManager->GetConnectionObject(connectionId);
        this->setToolTip(newName + NEWLINE + tr("Last Connected: ") + TimeToString(conn.last_connected) + NEWLINE + tr("Last Updated: ") + TimeToString(conn.updated));
    }
}

void ConnectionItemWidget::OnGroupItemRenamed(const GroupId &id, const QString &, const QString &newName)
{
    if (id == groupId)
    {
        originalItemName = newName;
        connNameLabel->setText(newName);
        const auto grp = QvProfileManager->GetGroupObject(id);
        this->setToolTip(newName + NEWLINE +                                                              //
                         (grp.subscription_config.isSubscription ? (tr("Subscription") + NEWLINE) : "") + //
                         tr("Last Updated: ") + TimeToString(grp.updated));
    }
}
