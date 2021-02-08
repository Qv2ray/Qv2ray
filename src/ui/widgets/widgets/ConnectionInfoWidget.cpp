#include "ConnectionInfoWidget.hpp"

#include "core/CoreUtils.hpp"
#include "core/connection/Serialization.hpp"
#include "ui/common/QRCodeHelper.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"

constexpr auto INDEX_CONNECTION = 0;
constexpr auto INDEX_GROUP = 1;

QvMessageBusSlotImpl(ConnectionInfoWidget)
{
    switch (msg)
    {
        MBRetranslateDefaultImpl;
        MBUpdateColorSchemeDefaultImpl;
        case HIDE_WINDOWS:
        case SHOW_WINDOWS: break;
    }
}

void ConnectionInfoWidget::updateColorScheme()
{
    latencyBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ping_gauge")));
    deleteBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ashbin")));
    editBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("edit")));
    editJsonBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("code")));
    shareLinkTxt->setStyleSheet("border-bottom: 1px solid gray; border-radius: 0px; padding: 2px; background-color: " +
                                this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
    groupSubsLinkTxt->setStyleSheet("border-bottom: 1px solid gray; border-radius: 0px; padding: 2px; background-color: " +
                                    this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
    //
    auto isDarkTheme = GlobalConfig.uiConfig.useDarkTheme;
    qrPixmapBlured = BlurImage(ColorizeImage(qrPixmap, isDarkTheme ? QColor(Qt::black) : QColor(Qt::white), 0.7), 35);
    qrLabel->setPixmap(IsComplexConfig(connectionId) ? QPixmap(":/assets/icons/qv2ray.png") : (isRealPixmapShown ? qrPixmap : qrPixmapBlured));
    const auto isCurrentItem = KernelInstance->CurrentConnection().connectionId == connectionId;
    connectBtn->setIcon(QIcon(isCurrentItem ? QV2RAY_COLORSCHEME_FILE("stop") : QV2RAY_COLORSCHEME_FILE("start")));
}

ConnectionInfoWidget::ConnectionInfoWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    //
    QvMessageBusConnect(ConnectionInfoWidget);
    updateColorScheme();
    //
    shareLinkTxt->setAutoFillBackground(true);
    shareLinkTxt->setCursor(QCursor(Qt::CursorShape::IBeamCursor));
    shareLinkTxt->installEventFilter(this);
    groupSubsLinkTxt->installEventFilter(this);
    qrLabel->installEventFilter(this);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnected, this, &ConnectionInfoWidget::OnConnected);
    connect(ConnectionManager, &QvConfigHandler::OnDisconnected, this, &ConnectionInfoWidget::OnDisConnected);
    connect(ConnectionManager, &QvConfigHandler::OnGroupRenamed, this, &ConnectionInfoWidget::OnGroupRenamed);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionModified, this, &ConnectionInfoWidget::OnConnectionModified);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, this, &ConnectionInfoWidget::OnConnectionModified_Pair);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRemovedFromGroup, this, &ConnectionInfoWidget::OnConnectionModified_Pair);
}

void ConnectionInfoWidget::ShowDetails(const ConnectionGroupPair &_identifier)
{
    this->groupId = _identifier.groupId;
    this->connectionId = _identifier.connectionId;
    bool isConnection = connectionId != NullConnectionId;
    //
    editBtn->setEnabled(isConnection);
    editJsonBtn->setEnabled(isConnection);
    connectBtn->setEnabled(isConnection);
    stackedWidget->setCurrentIndex(isConnection ? INDEX_CONNECTION : INDEX_GROUP);
    if (isConnection)
    {
        auto shareLink = ConvertConfigToString(_identifier);
        //
        shareLinkTxt->setText(shareLink);
        protocolLabel->setText(GetConnectionProtocolString(connectionId));
        //
        groupLabel->setText(GetDisplayName(groupId, 175));
        auto [protocol, host, port] = GetConnectionInfo(connectionId);
        Q_UNUSED(protocol)
        addressLabel->setText(host);
        portLabel->setNum(port);
        //
        shareLinkTxt->setCursorPosition(0);
        auto isDarkTheme = GlobalConfig.uiConfig.useDarkTheme;
        qrPixmap = QPixmap::fromImage(EncodeQRCode(shareLink, qrLabel->width() * devicePixelRatio()));
        //
        qrPixmapBlured = BlurImage(ColorizeImage(qrPixmap, isDarkTheme ? QColor(Qt::black) : QColor(Qt::white), 0.7), 35);
        //
        isRealPixmapShown = false;
        qrLabel->setPixmap(IsComplexConfig(connectionId) ? QPixmap(":/assets/icons/qv2ray.png") : qrPixmapBlured);
        qrLabel->setScaledContents(true);
        const auto isCurrentItem = KernelInstance->CurrentConnection().connectionId == connectionId;
        connectBtn->setIcon(QIcon(isCurrentItem ? QV2RAY_COLORSCHEME_FILE("stop") : QV2RAY_COLORSCHEME_FILE("start")));
    }
    else
    {
        connectBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("start")));
        groupNameLabel->setText(GetDisplayName(groupId));
        QStringList shareLinks;
        for (const auto &connection : ConnectionManager->GetConnections(groupId))
        {
            shareLinks << ConvertConfigToString({ connection, groupId }, !GlobalConfig.uiConfig.useOldShareLinkFormat);
        }
        //
        auto complexCount = shareLinks.removeAll(QV2RAY_SERIALIZATION_COMPLEX_CONFIG_PLACEHOLDER);
        complexCount += shareLinks.removeAll("");
        if (complexCount > 0)
        {
            shareLinks << "# " + tr("(Ignored %n complex config(s))", "", complexCount);
        }
        //
        groupShareTxt->setPlainText(shareLinks.join(NEWLINE));
        const auto &groupMetaData = ConnectionManager->GetGroupMetaObject(groupId);
        groupSubsLinkTxt->setText(groupMetaData.isSubscription ? groupMetaData.subscriptionOption.address : tr("Not a subscription"));
    }
}

ConnectionInfoWidget::~ConnectionInfoWidget()
{
}

void ConnectionInfoWidget::OnConnectionModified(const ConnectionId &id)
{
    if (id == this->connectionId)
        ShowDetails({ id, groupId });
}

void ConnectionInfoWidget::OnConnectionModified_Pair(const ConnectionGroupPair &id)
{
    if (id.connectionId == this->connectionId && id.groupId == this->groupId)
        ShowDetails(id);
}
void ConnectionInfoWidget::OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName)
    if (this->groupId == id)
    {
        groupNameLabel->setText(newName);
        groupLabel->setText(newName);
    }
}

void ConnectionInfoWidget::on_connectBtn_clicked()
{
    if (ConnectionManager->IsConnected({ connectionId, groupId }))
    {
        ConnectionManager->StopConnection();
    }
    else
    {
        ConnectionManager->StartConnection({ connectionId, groupId });
    }
}

void ConnectionInfoWidget::on_editBtn_clicked()
{
    emit OnEditRequested(connectionId);
}

void ConnectionInfoWidget::on_editJsonBtn_clicked()
{
    emit OnJsonEditRequested(connectionId);
}

void ConnectionInfoWidget::on_deleteBtn_clicked()
{
    if (QvMessageBoxAsk(this, tr("Delete an item"), tr("Are you sure to delete the current item?")) == Yes)
    {
        if (connectionId != NullConnectionId)
        {
            ConnectionManager->RemoveConnectionFromGroup(connectionId, groupId);
        }
        else
        {
            ConnectionManager->DeleteGroup(groupId);
        }
    }
}

bool ConnectionInfoWidget::eventFilter(QObject *object, QEvent *event)
{
    if (shareLinkTxt->underMouse() && event->type() == QEvent::MouseButtonRelease)
    {
        if (!shareLinkTxt->hasSelectedText())
            shareLinkTxt->selectAll();
    }
    else if (groupSubsLinkTxt->underMouse() && event->type() == QEvent::MouseButtonRelease)
    {
        if (!groupSubsLinkTxt->hasSelectedText())
            groupSubsLinkTxt->selectAll();
    }
    else if (qrLabel->underMouse() && event->type() == QEvent::MouseButtonRelease)
    {
        qrLabel->setPixmap(IsComplexConfig(connectionId) ? QPixmap(":/assets/icons/qv2ray.png") : (isRealPixmapShown ? qrPixmapBlured : qrPixmap));
        isRealPixmapShown = !isRealPixmapShown;
    }

    return QWidget::eventFilter(object, event);
}

void ConnectionInfoWidget::OnConnected(const ConnectionGroupPair &id)
{
    if (id == ConnectionGroupPair{ connectionId, groupId })
    {
        connectBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("stop")));
    }
}

void ConnectionInfoWidget::OnDisConnected(const ConnectionGroupPair &id)
{
    if (id == ConnectionGroupPair{ connectionId, groupId })
    {
        connectBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("start")));
    }
}

void ConnectionInfoWidget::on_latencyBtn_clicked()
{
    if (connectionId != NullConnectionId)
    {
        ConnectionManager->StartLatencyTest(connectionId);
    }
    else
    {
        ConnectionManager->StartLatencyTest(groupId);
    }
}
