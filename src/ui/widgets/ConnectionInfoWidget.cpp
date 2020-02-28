#include "ConnectionInfoWidget.hpp"

#include "3rdparty/qzxing/src/QZXing.h"
#include "core/CoreUtils.hpp"
#include "core/connection/Serialization.hpp"

ConnectionInfoWidget::ConnectionInfoWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    duplicateBtn->setIcon(QICON_R("duplicate.png"));
    deleteBtn->setIcon(QICON_R("delete.png"));
    editBtn->setIcon(QICON_R("edit.png"));
    editJsonBtn->setIcon(QICON_R("json.png"));
    //
    shareLinkTxt->setAutoFillBackground(true);
    shareLinkTxt->setStyleSheet("border-bottom: 1px solid gray; border-radius: 0px; padding: 2px; background-color: " +
                                this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
    shareLinkTxt->setCursor(QCursor(Qt::CursorShape::IBeamCursor));
    shareLinkTxt->installEventFilter(this);
    //
    connect(ConnectionManager, &QvConnectionHandler::OnConnected, this, &ConnectionInfoWidget::OnConnected);
    connect(ConnectionManager, &QvConnectionHandler::OnDisConnected, this, &ConnectionInfoWidget::OnDisConnected);
}

void ConnectionInfoWidget::ShowDetails(const tuple<GroupId, ConnectionId> &_identifier)
{
    groupId = get<0>(_identifier);
    connectionId = get<1>(_identifier);
    bool isConnection = connectionId != NullConnectionId;
    editJsonBtn->setEnabled(isConnection);
    connectBtn->setEnabled(isConnection);
    duplicateBtn->setEnabled(isConnection);
    editBtn->setEnabled(isConnection);

    if (isConnection)
    {
        groupLabel->setText(ConnectionManager->GetDisplayName(groupId, 175));
        protocolLabel->setText(ConnectionManager->GetConnectionProtocolString(connectionId));
        auto [protocol, host, port] = ConnectionManager->GetConnectionData(connectionId);
        Q_UNUSED(protocol)
        addressLabel->setText(host);
        portLabel->setNum(port);
        //
        auto shareLink = ConvertConfigToString(connectionId);
        shareLinkTxt->setText(shareLink);
        shareLinkTxt->setCursorPosition(0);
        //
        QZXingEncoderConfig conf;
        conf.border = true;
        conf.imageSize = QSize(400, 400);
        auto img = QZXing().encodeData(shareLink, conf);
        qrLabel->setPixmap(QPixmap::fromImage(img));
        //
        connectBtn->setIcon(ConnectionManager->IsConnected(connectionId) ? QICON_R("stop.png") : QICON_R("connect.png"));
    }
    else
    {
        connectBtn->setIcon(QICON_R("connect.png"));
        groupLabel->setText(tr("N/A"));
        protocolLabel->setText(tr("N/A"));
        addressLabel->setText(tr("N/A"));
        portLabel->setText(tr("N/A"));
        //
        shareLinkTxt->clear();
        qrLabel->clear();
    }
}

ConnectionInfoWidget::~ConnectionInfoWidget()
{
}

void ConnectionInfoWidget::on_connectBtn_clicked()
{
    if (ConnectionManager->IsConnected(connectionId)) { ConnectionManager->StopConnection(); }
    else
    {
        ConnectionManager->StartConnection(connectionId);
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
    ConnectionManager->DeleteConnection(connectionId);
}

bool ConnectionInfoWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        if (shareLinkTxt->underMouse())
        {
            if (!shareLinkTxt->hasSelectedText()) { shareLinkTxt->selectAll(); }
        }
    }

    return QWidget::eventFilter(object, event);
}

void ConnectionInfoWidget::OnConnected(const ConnectionId &id)
{
    if (connectionId == id) { connectBtn->setIcon(QICON_R("stop.png")); }
}

void ConnectionInfoWidget::OnDisConnected(const ConnectionId &id)
{
    if (connectionId == id) { connectBtn->setIcon(QICON_R("connect.png")); }
}
// MWTryPingConnection(CurrentConnectionIdentifier);
void ConnectionInfoWidget::on_duplicateBtn_clicked()
{
    // QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    // if (!IsSelectionConnectable) {
    //    return;
    //}
    //
    // auto selectedFirst = connectionListWidget->currentItem();
    // auto _identifier = ItemConnectionIdentifier(selectedFirst);
    // SUBSCRIPTION_CONFIG_MODIFY_ASK(selectedFirst)
    // CONFIGROOT conf;
    //// Alias may change.
    // QString alias = _identifier.connectionName;
    // bool isComplex = IsComplexConfig(connections[_identifier].config);
    //
    // if (connections[_identifier].configType == CONNECTION_REGULAR) {
    //    conf = ConvertConfigFromFile(QV2RAY_CONFIG_DIR +
    //    _identifier.connectionName + QV2RAY_CONFIG_FILE_EXTENSION, isComplex);
    //} else {
    //    conf = ConvertConfigFromFile(QV2RAY_SUBSCRIPTION_DIR +
    //    _identifier.subscriptionName + "/" + _identifier.connectionName  +
    //    QV2RAY_CONFIG_FILE_EXTENSION, isComplex); alias =
    //    _identifier.subscriptionName + "_" + _identifier.connectionName;
    //}
    //
    // SaveConnectionConfig(conf, &alias, false);
    // GlobalConfig.configs.push_back(alias);
    // SaveGlobalConfig(GlobalConfig);
    // this->OnConfigListChanged(false);}
}

void ConnectionInfoWidget::on_latencyBtn_clicked()
{
    if (connectionId != NullConnectionId) { ConnectionManager->StartLatencyTest(connectionId); }
    else
    {
        ConnectionManager->StartLatencyTest(groupId);
    }
}
