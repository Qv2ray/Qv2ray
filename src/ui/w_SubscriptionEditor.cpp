#include "w_SubscriptionEditor.hpp"
#include "QvHTTPRequestHelper.hpp"
#include "QvUtils.hpp"
#include "QvCoreConfigOperations.hpp"


SubscribeEditor::SubscribeEditor(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    auto conf = GetGlobalConfig();

    foreach (auto value, conf.subscribes) {
        subsribeTable->insertRow(subsribeTable->rowCount());
        subsribeTable->setItem(subsribeTable->rowCount() - 1, 0, new QTableWidgetItem(QString::fromStdString(value.first)));
        subsribeTable->setItem(subsribeTable->rowCount() - 1, 1, new QTableWidgetItem(QString::fromStdString(value.second)));
    }
}

SubscribeEditor::~SubscribeEditor()
{
}

void SubscribeEditor::on_buttonBox_accepted()
{
    map<string, string> subscribes;

    for (int i = 0; i < subsribeTable->rowCount(); i++) {
        pair<string, string> kvp;
        kvp.first = subsribeTable->item(i, 0)->text().toStdString();
        kvp.second = subsribeTable->item(i, 1)->text().toStdString();
        subscribes.insert(kvp);
    }

    auto conf = GetGlobalConfig();
    conf.subscribes = subscribes;
    SetGlobalConfig(conf);
    emit s_update_config();
}

void SubscribeEditor::on_addSubsButton_clicked()
{
    subsribeTable->insertRow(subsribeTable->rowCount());
}

void SubscribeEditor::on_updateButton_clicked()
{
    if (isUpdateInProgress) {
        QvMessageBox(this, tr("#UpdateInProcess"), tr("#TryLater"));
        return;
    }

    isUpdateInProgress = true;
    int index = subsribeTable->currentRow();
    auto name = subsribeTable->item(index, 0)->text();
    auto url = subsribeTable->item(index, 1)->text();
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
    for (auto rowIndex = 0; subscribeList->count(); rowIndex++) {
        auto url = subsribeTable->item(rowIndex, 1)->text();
        helper.get(url);
    }
}

