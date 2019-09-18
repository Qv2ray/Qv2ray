#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "QvUtils.h"
#include "QvCoreInteractions.h"
#include "QvCoreConfigOperations.h"
#include "w_ConnectionEditWindow.h"
#include "w_ImportConfig.h"



ImportConfigWindow::ImportConfigWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImportConfigWindow)
{
    ui->setupUi(this);
    ui->nameTxt->setText(QUuid::createUuid().toString());
}

ImportConfigWindow::~ImportConfigWindow()
{
    delete ui;
}

void ImportConfigWindow::on_importSourceCombo_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void ImportConfigWindow::on_selectFileBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("OpenConfigFile"), "~/");
    ui->fileLineTxt->setText(dir);
}

void ImportConfigWindow::on_buttonBox_accepted()
{
    QString alias = ui->nameTxt->text();
    QJsonObject config;

    if (ui->importSourceCombo->currentIndex() == 0) {
        // From File...
        bool overrideInBound = !ui->keepImportedInboundCheckBox->isChecked();

        if (!Qv2Instance::VerifyVConfigFile(ui->fileLineTxt->text())) {
            QvMessageBox(this, tr("#InvalidConfigFile"), tr("ConfigFileCheckFailed"));
            return;
        }

        QString path = ui->fileLineTxt->text();
        alias = alias != "" ? alias : QFileInfo(path).fileName();
        config = ConvertConfigFromFile(path, overrideInBound);
    } else {
        QString vmess = ui->vmessConnectionStringTxt->toPlainText();
        int result = VerifyVMessProtocolString(vmess);

        if (result == 0) {
            // This result code passes the validation check.
            //QvMessageBox(this, tr("#VMessCheck"), tr("#AbleToImportConfig"));
        } else if (result == -1) {
            QvMessageBox(this, tr("#VMessCheck"), tr("#NotValidVMessProtocolString"));
            done(0);
            return;
        } else {
            QvMessageBox(this, tr("#VMessCheck"), tr("#INTERNAL_ERROR"));
            return;
        }

        config = ConvertConfigFromVMessString(ui->vmessConnectionStringTxt->toPlainText());
        //
        alias = alias != "" ? alias : config["QV2RAY_ALIAS"].toString();
        config.remove("QV2RAY_ALIAS");
    }

    Qv2rayConfig conf = GetGlobalConfig();
    conf.configs.push_back(alias.toStdString());
    SetGlobalConfig(conf);
    auto needReload = SaveConnectionConfig(config, &alias);
    LOG(MODULE_CONNECTION_VMESS, "WARNING: POSSIBLE LOSS OF DATA")
    emit s_reload_config(needReload);
}
