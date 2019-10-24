#include "w_SubscriptionEditor.h"
#include "ui_w_SubscriptionEditor.h"
#include "QvHTTPRequestHelper.h"
#include "QvUtils.h"
#include "QvCoreConfigOperations.h"


SubscribeEditor::SubscribeEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_SubscribeEditor)
{
    ui->setupUi(this);
    auto conf = GetGlobalConfig();

    foreach (auto value, conf.subscribes) {
        ui->subsribeTable->insertRow(ui->subsribeTable->rowCount());
        ui->subsribeTable->setItem(ui->subsribeTable->rowCount() - 1, 0, new QTableWidgetItem(QString::fromStdString(value.first)));
        ui->subsribeTable->setItem(ui->subsribeTable->rowCount() - 1, 1, new QTableWidgetItem(QString::fromStdString(value.second)));
    }
}

SubscribeEditor::~SubscribeEditor()
{
    delete ui;
}

void SubscribeEditor::on_buttonBox_accepted()
{
    map<string, string> subscribes;

    for (int i = 0; i < ui->subsribeTable->rowCount(); i++) {
        pair<string, string> kvp;
        kvp.first = ui->subsribeTable->item(i, 0)->text().toStdString();
        kvp.second = ui->subsribeTable->item(i, 1)->text().toStdString();
        subscribes.insert(kvp);
    }

    auto conf = GetGlobalConfig();
    conf.subscribes = subscribes;
    SetGlobalConfig(conf);
    emit s_update_config();
}

void SubscribeEditor::on_addSubsButton_clicked()
{
    ui->subsribeTable->insertRow(ui->subsribeTable->rowCount());
}

void SubscribeEditor::on_updateButton_clicked()
{
    if (isUpdateInProgress) {
        QvMessageBox(this, tr("#UpdateInProcess"), tr("#TryLater"));
        return;
    }

    isUpdateInProgress = true;
    int index = ui->subsribeTable->currentRow();
    auto name = ui->subsribeTable->item(index, 0)->text();
    auto url = ui->subsribeTable->item(index, 1)->text();
    auto data = helper.syncget(url);
    ProcessSubscriptionEntry(data, name);
}

void SubscribeEditor::ProcessSubscriptionEntry(QByteArray result, QString subsciptionName)
{
    auto content = GetVmessFromBase64OrPlain(result).replace("\r", "");

    if (!content.isEmpty()) {
        auto vmessList = content.split("\n");

        for (auto vmess : vmessList) {
            auto config = ConvertConfigFromVMessString(vmess);

            if (subscriptions.contains(subsciptionName)) {
            }
        }

        isUpdateInProgress = false;
    }
}

void SubscribeEditor::on_updateAllButton_clicked()
{
    for (auto rowIndex = 0; ui->subscribeList->count(); rowIndex++) {
        auto url = ui->subsribeTable->item(rowIndex, 1)->text();
        helper.get(url);
    }
}

