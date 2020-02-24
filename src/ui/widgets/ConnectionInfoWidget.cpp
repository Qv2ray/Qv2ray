#include "ConnectionInfoWidget.hpp"
#include "core/CoreUtils.hpp"

ConnectionInfoWidget::ConnectionInfoWidget(QWidget *parent) :
    QWidget(parent), connectionId("null"), groupId("null")
{
    setupUi(this);
    duplicateBtn->setIcon(QICON_R("duplicate.png"));
    deleteBtn->setIcon(QICON_R("delete.png"));
    editBtn->setIcon(QICON_R("edit.png"));
    editJsonBtn->setIcon(QICON_R("json.png"));
    //
    shareLinkTxt->setAutoFillBackground(true);
    shareLinkTxt->setStyleSheet("border-bottom: 1px solid gray; border-radius: 0px; padding: 2px; background-color: " + this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
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

    if (connectionId.toString() != "null") {
        connNameLabel->setText(ConnectionManager->GetConnection(connectionId).displayName);
        groupLabel->setText(ConnectionManager->GetGroup(groupId).displayName);
        protocolLabel->setText(ConnectionManager->GetConnectionProtocolString(connectionId));
        auto x = ConnectionManager->GetConnectionInfo(connectionId);
        addressLabel->setText(get<0>(x));
        portLabel->setNum(get<1>(x));
        //
        editJsonBtn->setEnabled(true);
        connectBtn->setEnabled(true);
        duplicateBtn->setEnabled(true);
        //
        shareLinkTxt->setText("scheme://user:pass@host:port/path/to/file?arg1=ARG1&arg2=ARG2#tag");
        shareLinkTxt->setCursorPosition(0);
    } else {
        connNameLabel->setText(ConnectionManager->GetGroup(groupId).displayName);
        groupLabel->setText(tr("N/A"));
        protocolLabel->setText(tr("N/A"));
        addressLabel->setText(tr("N/A"));
        portLabel->setText(tr("N/A"));
        //
        editJsonBtn->setEnabled(false);
        connectBtn->setEnabled(false);
        duplicateBtn->setEnabled(false);
    }
}

ConnectionInfoWidget::~ConnectionInfoWidget()
{
}

void ConnectionInfoWidget::on_connectBtn_clicked()
{
    if (ConnectionManager->IsConnected(connectionId)) {
        ConnectionManager->StartConnection(connectionId);
    } else {
        ConnectionManager->StopConnection();
    }
}

void ConnectionInfoWidget::on_editBtn_clicked()
{
}

void ConnectionInfoWidget::on_editJsonBtn_clicked()
{
}

void ConnectionInfoWidget::on_deleteBtn_clicked()
{
    ConnectionManager->DeleteConnection(connectionId);
}

bool ConnectionInfoWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        if (shareLinkTxt->underMouse()) {
            if (!shareLinkTxt->hasSelectedText()) {
                shareLinkTxt->selectAll();
            }
        }
    }

    return QWidget::eventFilter(object, event);
}

void ConnectionInfoWidget::OnConnected(const ConnectionId &id)
{
    if (connectionId == id) {
        connectBtn->setIcon(QICON_R("stop.png"));
    }
}

void ConnectionInfoWidget::OnDisConnected(const ConnectionId &id)
{
    if (connectionId == id) {
        connectBtn->setIcon(QICON_R("connect.png"));
    }
}
