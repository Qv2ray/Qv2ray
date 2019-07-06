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
}

ImportConfigWindow::~ImportConfigWindow()
{
    delete ui;
}

void ImportConfigWindow::on_pushButton_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("OpenConfigFile"), "~/");
    ui->fileLineTxt->setText(dir);
}

void ImportConfigWindow::savefromFile(QString path, QString alias)
{
    Q_UNUSED(path)
    Q_UNUSED(alias)
    QFile configFile(path);

    if (!configFile.open(QIODevice::ReadOnly)) {
        QvMessageBox(this, tr("ImportConfig"), tr("CannotOpenFile"));
        qDebug() << "ImportConfig::CannotOpenFile";
        return;
    }

    QByteArray allData = configFile.readAll();
    configFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    QJsonObject rootobj = v2conf.object();
    QJsonObject outbound;

    if (rootobj.contains("outbounds")) {
        outbound = rootobj.value("outbounds").toArray().first().toObject();
    } else {
        outbound = rootobj.value("outbound").toObject();
    }

    if (!QFile::copy(path, "newFile")) {
        QvMessageBox(this, tr("ImportConfig"), tr("CannotCopyCustomConfig"));
        qDebug() << "ImportConfig::CannotCopyCustomConfig";
    }
}

void ImportConfigWindow::on_buttonBox_accepted()
{
    QString alias = ui->nameTxt->text();

    if (ui->importSourceCombo->currentIndex() == 0) { // From File...
        bool overrideInBound = ui->useCurrentSettingRidBtn->isChecked();
        on_verifyFileBtn_clicked();
        QString path = ui->fileLineTxt->text();
        savefromFile(path, alias);
        // !!! TODO
    } else {
        on_verifyVMessBtn_clicked();
        auto config = ConvertOutboundFromVMessString(ui->vmessConnectionStringTxt->toPlainText());
        alias = alias != "" ? alias : config["QV2RAY_ALIAS"].toString();
        config.remove("QV2RAY_ALIAS");
        Qv2Config conf = GetGlobalConfig();
        conf.configs.push_back(alias.toStdString());
        SetGlobalConfig(conf);
        SaveConnectionConfig(config, &alias);
        emit s_reload_config();
    }
}

void ImportConfigWindow::on_verifyVMessBtn_clicked()
{
    QString vmess = ui->vmessConnectionStringTxt->toPlainText();
    int result = VerifyVMessProtocolString(vmess);

    if (result == 0) {
        QvMessageBox(this, tr("#VMessCheck"), tr("#AbleToImportConfig"));
    } else if (result == -1) {
        QvMessageBox(this, tr("#VMessCheck"), tr("#NotValidVMessProtocolString"));
    } else {
        QvMessageBox(this, tr("#VMessCheck"), tr("#INTERNAL_ERROR"));
    }
}

void ImportConfigWindow::on_verifyFileBtn_clicked()
{
    if (!Qv2Instance::checkConfigFile(ui->fileLineTxt->text())) {
        QvMessageBox(this, tr("#InvalidConfigFile"), tr("ConfigFileCheckFailed"));
    } else {
        QvMessageBox(this, tr("#VConfigFileCheckPassed"), tr("#AbleToImportConfig"));
    }
}
