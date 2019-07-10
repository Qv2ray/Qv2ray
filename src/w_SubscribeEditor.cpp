#include "w_SubscribeEditor.h"
#include "ui_w_SubscribeEditor.h"
#include "QvHTTPRequestHelper.h"

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
