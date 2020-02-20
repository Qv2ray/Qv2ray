#include "ConnectionInfoWidget.hpp"

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
        connNameLabel->setText(ConnectionHandler->GetConnection(connectionId).displayName);
        groupLabel->setText(ConnectionHandler->GetGroup(groupId).displayName);
    } else {
        connNameLabel->setText(ConnectionHandler->GetGroup(groupId).displayName);
    }

    //
    //auto isComplexConfig = IsComplexConfig(conf.config);
    //routeCountLabel->setText(isComplexConfig ? tr("Complex") : tr("Simple"));
    //
    //if (conf.configType == CONNECTION_SUBSCRIPTION) {
    //    routeCountLabel->setText(routeCountLabel->text().append(" (" + tr("Subscription") + ":" + conf.subscriptionName + ")"));
    //}
    //
    //// Get Connection info
    //auto host_port = MWGetConnectionInfo(fullIdentifier.IdentifierString());
    //_hostLabel->setText(get<0>(host_port));
    //_portLabel->setText(QSTRN(get<1>(host_port)));
    //_OutBoundTypeLabel->setText(get<2>(host_port));
}

ConnectionInfoWidget::~ConnectionInfoWidget()
{
}

void ConnectionInfoWidget::on_connectBtn_clicked()
{
}

void ConnectionInfoWidget::on_editBtn_clicked()
{
}

void ConnectionInfoWidget::on_editJsonBtn_clicked()
{
}

void ConnectionInfoWidget::on_pushButton_clicked()
{
    QGuiApplication::clipboard()->setText(shareLinkTxt->text());
}
