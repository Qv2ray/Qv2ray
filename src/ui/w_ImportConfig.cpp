#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "QvUtils.h"
#include "QvCoreInteractions.h"
#include "QvCoreConfigOperations.h"
#include "w_OutboundEditor.h"
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
    QString dir = QFileDialog::getOpenFileName(this, tr("Select file to import"), QDir::currentPath());
    ui->fileLineTxt->setText(dir);
}

void ImportConfigWindow::on_buttonBox_accepted()
{
    QString alias = ui->nameTxt->text();
    QJsonObject config;
    auto conf = GetGlobalConfig();
    auto needReload = false;

    if (ui->importSourceCombo->currentIndex() == 0) {
        // From File...
        bool overrideInBound = !ui->keepImportedInboundCheckBox->isChecked();
        auto fileName = ui->fileLineTxt->text();

        if (!Qv2Instance::ValidateConfig(&fileName)) {
            QvMessageBox(this, tr("Import config file"), tr("Failed to check the validity of the config file."));
            return;
        }

        QString path = ui->fileLineTxt->text();
        alias = alias != "" ? alias : QFileInfo(path).fileName();
        config = ConvertConfigFromFile(path, overrideInBound);
        //
        // We save first, "alias" may change to prevent override existing file.
        needReload = SaveConnectionConfig(config, &alias, false);
        conf.configs.push_back(alias.toStdString());
        //
        SetGlobalConfig(conf);
    } else {
        QString vmess = ui->vmessConnectionStringTxt->toPlainText();
        //
        // We saperate the string into lines.
        QStringList vmessList = vmess.split(NEWLINE, QString::SplitBehavior::SkipEmptyParts);
        LOG(MODULE_CONNECTION_IMPORT, to_string(vmessList.count()) + " vmess connection found.")

        foreach (auto vmessString, vmessList) {
            int result = VerifyVMessProtocolString(vmess);

            switch (result) {
                case 0:
                    // This result code passes the validation check.
                    config = ConvertConfigFromVMessString(ui->vmessConnectionStringTxt->toPlainText());
                    //
                    alias = alias.isEmpty() ? alias : config["QV2RAY_ALIAS"].toString();
                    config.remove("QV2RAY_ALIAS");
                    //
                    // Save first.
                    needReload = needReload || SaveConnectionConfig(config, &alias, false);
                    conf.configs.push_back(alias.toStdString());
                    break;

                case -1:
                    QvMessageBox(this, tr("VMess String Check"), tr("VMess string is not valid."));
                    done(0);
                    return;

                default:
                    QvMessageBox(this, tr("VMess String Check"), tr("VMess config is not valid."));
                    return;
            }
        }
    }

    SetGlobalConfig(conf);
    emit s_reload_config(needReload);
}
