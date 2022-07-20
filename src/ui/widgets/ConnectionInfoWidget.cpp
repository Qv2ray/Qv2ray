#include "ConnectionInfoWidget.hpp"

#include "QRCodeHelper/QRCodeHelper.hpp"
#include "Qv2rayBase/Common/ProfileHelpers.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui/widgets/TagLineEditorWidget.hpp"

constexpr auto INDEX_CONNECTION = 0;
constexpr auto INDEX_GROUP = 1;

QvMessageBusSlotImpl(ConnectionInfoWidget)
{
    switch (msg)
    {
        MBUpdateColorSchemeDefaultImpl;
    }
}

void ConnectionInfoWidget::updateColorScheme()
{
    latencyBtn->setIcon(QIcon(STYLE_RESX("ping_gauge")));
    deleteBtn->setIcon(QIcon(STYLE_RESX("ashbin")));
    editBtn->setIcon(QIcon(STYLE_RESX("edit")));
    editJsonBtn->setIcon(QIcon(STYLE_RESX("code")));
    shareLinkTxt->setStyleSheet("border-bottom: 1px solid gray; border-radius: 0px; padding: 2px; background-color: " +
                                this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
    groupSubsLinkTxt->setStyleSheet("border-bottom: 1px solid gray; border-radius: 0px; padding: 2px; background-color: " +
                                    this->palette().color(this->backgroundRole()).name(QColor::HexRgb));

    qrLabel->setPixmap(IsComplexConfig(connectionId) ? QvApp->Qv2rayLogo : (isRealPixmapShown ? qrPixmap : qrPixmapBlured));

    const auto isCurrentItem = QvKernelManager->CurrentConnection().connectionId == connectionId;
    connectBtn->setIcon(QIcon(isCurrentItem ? STYLE_RESX("stop") : STYLE_RESX("start")));
}

ConnectionInfoWidget::ConnectionInfoWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    QvMessageBusConnect();
    updateColorScheme();

    shareLinkTxt->setAutoFillBackground(true);
    shareLinkTxt->setCursor(QCursor(Qt::CursorShape::IBeamCursor));
    shareLinkTxt->installEventFilter(this);
    groupSubsLinkTxt->installEventFilter(this);
    qrLabel->installEventFilter(this);
    qrLabel->setScaledContents(true);

    {
        tagsEditor = new TagsLineEdit;
        connect(tagsEditor, &TagsLineEdit::OnTagClicked, this, &ConnectionInfoWidget::OnTagSearchRequested);
        connect(tagsEditor, &TagsLineEdit::OnTagsChanged, this, [this](const QStringList &tags) { QvProfileManager->SetConnectionTags(connectionId, tags); });
        tagsLayout->addWidget(tagsEditor);
        tagsLayout->setContentsMargins(0, 0, 0, 0);
    }

    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnConnected, this, &ConnectionInfoWidget::OnConnected);
    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnDisconnected, this, &ConnectionInfoWidget::OnDisConnected);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnGroupRenamed, this, &ConnectionInfoWidget::OnGroupRenamed);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionModified, this, &ConnectionInfoWidget::OnConnectionModified);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionLinkedWithGroup, this, &ConnectionInfoWidget::OnConnectionModified_Pair);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionRemovedFromGroup, this, &ConnectionInfoWidget::OnConnectionModified_Pair);
}

void ConnectionInfoWidget::ShowDetails(const ProfileId &idpair)
{
    this->groupId = idpair.groupId;
    this->connectionId = idpair.connectionId;
    bool isConnection = !connectionId.isNull();
    //
    editBtn->setEnabled(isConnection);
    editJsonBtn->setEnabled(isConnection);
    connectBtn->setEnabled(isConnection);
    stackedWidget->setCurrentIndex(isConnection ? INDEX_CONNECTION : INDEX_GROUP);
    if (isConnection)
    {
        const auto tags = QvProfileManager->GetConnectionObject(connectionId).tags;
        tagsEditor->SetTags(tags);
        auto shareLink = ConvertConfigToString(idpair.connectionId);
        if (shareLink)
        {
            qrPixmap = QPixmap::fromImage(EncodeQRCode(*shareLink, qrLabel->width() * devicePixelRatio()));
            shareLinkTxt->setText(*shareLink);
        }
        shareLinkTxt->setCursorPosition(0);
        protocolLabel->setText(GetConnectionProtocolDescription(connectionId));
        groupLabel->setText(GetDisplayName(groupId));

        if (IsComplexConfig(connectionId))
        {
            qrLabel->setPixmap(QvApp->Qv2rayLogo);
        }
        else
        {
            const auto root = QvProfileManager->GetConnection(connectionId);
            if (!root.outbounds.isEmpty())
            {
                const auto &[protocol, host, port] = GetOutboundInfo(root.outbounds.first());
                Q_UNUSED(protocol)
                addressLabel->setText(host);
                portLabel->setNum(port.from);
            }
            qrPixmapBlured = BlurImage(ColorizeImage(qrPixmap, StyleManager->isDarkMode() ? QColor(Qt::black) : QColor(Qt::white), 0.7), 35);
            qrLabel->setPixmap(qrPixmapBlured);
        }

        const auto isCurrentItem = QvKernelManager->CurrentConnection().connectionId == connectionId;
        connectBtn->setIcon(QIcon(isCurrentItem ? STYLE_RESX("stop") : STYLE_RESX("start")));
        isRealPixmapShown = false;
    }
    else
    {
        connectBtn->setIcon(QIcon(STYLE_RESX("start")));
        groupNameLabel->setText(GetDisplayName(groupId));

        QStringList shareLinks;
        {
            const auto conns = QvProfileManager->GetConnections(groupId);
            shareLinks.reserve(conns.size());
            for (const auto &connection : conns)
            {
                const auto link = ConvertConfigToString(connection);
                if (link)
                    shareLinks.append(*link);
            }
        }

        groupShareTxt->setPlainText(shareLinks.join('\n'));
        const auto &groupMetaData = QvProfileManager->GetGroupObject(groupId);

        if (groupMetaData.subscription_config.isSubscription)
        {
            const auto info = QvPluginAPIHost->Subscription_GetProviderInfo(groupMetaData.subscription_config.providerId).second;
            switch (info.mode)
            {
                case Qv2rayPlugin::Subscription::Subscribe_Decoder: groupSubsLinkTxt->setText(groupMetaData.subscription_config.address); break;
                case Qv2rayPlugin::Subscription::Subscribe_FetcherAndDecoder: groupSubsLinkTxt->setText(tr("(Custom Subscription Settings)")); break;
            }
        }
        else
        {
            groupSubsLinkTxt->setText(tr("Not a subscription"));
        }
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

void ConnectionInfoWidget::OnConnectionModified_Pair(const ProfileId &id)
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
    if (QvProfileManager->IsConnected({ connectionId, groupId }))
    {
        QvProfileManager->StopConnection();
    }
    else
    {
        QvProfileManager->StartConnection({ connectionId, groupId });
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
    if (QvBaselib->Ask(tr("Delete an item"), tr("Are you sure to delete the current item?")) == Qv2rayBase::MessageOpt::Yes)
    {
        if (!connectionId.isNull())
            QvProfileManager->RemoveFromGroup(connectionId, groupId);
        else
            QvProfileManager->DeleteGroup(groupId, false);
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
        qrLabel->setPixmap(IsComplexConfig(connectionId) ? QvApp->Qv2rayLogo : (isRealPixmapShown ? qrPixmapBlured : qrPixmap));
        isRealPixmapShown = !isRealPixmapShown;
    }

    return QWidget::eventFilter(object, event);
}

void ConnectionInfoWidget::OnConnected(const ProfileId &id)
{
    if (id == ProfileId{ connectionId, groupId })
    {
        connectBtn->setIcon(QIcon(STYLE_RESX("stop")));
    }
}

void ConnectionInfoWidget::OnDisConnected(const ProfileId &id)
{
    if (id == ProfileId{ connectionId, groupId })
    {
        connectBtn->setIcon(QIcon(STYLE_RESX("start")));
    }
}

void ConnectionInfoWidget::on_latencyBtn_clicked()
{
    if (!connectionId.isNull())
    {
        QvProfileManager->StartLatencyTest(connectionId, GlobalConfig->behaviorConfig->DefaultLatencyTestEngine);
    }
    else
    {
        QvProfileManager->StartLatencyTest(groupId, GlobalConfig->behaviorConfig->DefaultLatencyTestEngine);
    }
}
