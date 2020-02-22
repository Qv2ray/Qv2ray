#include "ConnectionInfoWidget.hpp"
#include "core/CoreUtils.hpp"

ConnectionInfoWidget::ConnectionInfoWidget(QWidget *parent) :
    QWidget(parent), connectionId("null"), groupId("null")
{
    setupUi(this);
}

void ConnectionInfoWidget::ShowDetails(const tuple<GroupId, ConnectionId> &_identifier)
{
    groupId = get<0>(_identifier);
    connectionId = get<1>(_identifier);

    if (connectionId.toString() != "null") {
        connNameLabel->setText(ConnectionManager->GetConnection(connectionId).displayName);
        groupLabel->setText(ConnectionManager->GetGroup(groupId).displayName);
        protocolLabel->setText(ConnectionManager->GetConnectionBasicInfo(connectionId));
        auto x = ConnectionManager->GetConnectionInfo(connectionId);
        addressLabel->setText(get<0>(x));
        portLabel->setNum(get<1>(x));
        //
        editJsonBtn->setEnabled(true);
        connectBtn->setEnabled(true);
        duplicateBtn->setEnabled(true);
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
    ConnectionManager->StartConnection(connectionId);
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
