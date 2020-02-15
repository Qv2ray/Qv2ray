#include "ConnectionWidget.hpp"
#include "QMessageBox"

ConnectionWidget::ConnectionWidget(const ConnectionId &id, QWidget *parent): QWidget(parent),
    _id(id), connection(connectionHandler->GetConnection(id))
{
    setupUi(this);
    connNameLabel->setText(connection.displayName);
    latencyLabel->setText(tr("Latency: ") + QSTRN(connection.latency) + " " + tr("ms"));
}

ConnectionWidget::~ConnectionWidget()
{
}

void ConnectionWidget::on_editBtn_clicked()
{
}

void ConnectionWidget::on_latencyBtn_clicked()
{
}
