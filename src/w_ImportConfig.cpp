#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "vinteract.hpp"
#include "w_ConnectionEditWindow.h"
#include "w_ImportConfig.h"



ImportConfigWindow::ImportConfigWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImportConfigWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(updateConfTable()), parentWidget(), SLOT(updateConfTable()));
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
        showWarnMessageBox(this, tr("ImportConfig"), tr("CannotOpenFile"));
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
        showWarnMessageBox(this, tr("ImportConfig"), tr("CannotCopyCustomConfig"));
        qDebug() << "ImportConfig::CannotCopyCustomConfig";
    }
}

void ImportConfigWindow::on_buttonBox_accepted()
{
    QString alias = ui->nameTxt->text();

    if (ui->importSourceCombo->currentIndex() == 0) { // From File...
        QString path = ui->fileLineTxt->text();
        on_verifyFileBtn_clicked();
        savefromFile(path, alias);
    } else {
        on_verifyVMessBtn_clicked();
        // TODO: SAVE CONFIG!!!!
        //
        //if (QFile::exists(ConfigDir.path() + "/config.json.tmp")) {
        //    ImportConfig *im = new ImportConfig(this->parentWidget());
        //
        //    if (v2Instance::checkConfigFile(QCoreApplication::applicationDirPath() + "/config.json.tmp")) {
        //        im->savefromFile("config.json.tmp", alias);
        //    }
        //
        //    QFile::remove("config.json.tmp");
        //} else {
        //    Utils::showWarnMessageBox(this, tr("ImportConfig"), tr("CannotGenerateConfig"));
        //    qDebug() << "ImportConfig::CannotGenerateConfig";
        //}
    }

    if (ui->useCurrentSettingRidBtn->isChecked()) {
        // TODO: Use Current Settings...
    } else {
        // TODO: Override Inbound....
    }
}

void ImportConfigWindow::on_verifyVMessBtn_clicked()
{
    QString vmess = ui->vmessConnectionStringTxt->toPlainText();

    if (!vmess.toLower().startsWith("vmess://")) {
        showWarnMessageBox(this, tr("#VMessDecodeError"), tr("#NotValidVMessProtocolString"));
        return;
    }

    try {
        QStringRef vmessJsonB64(&vmess, 8, vmess.length() - 8);
        auto vmessString = Utils::base64_decode(vmessJsonB64.toString());
        auto vmessConf = Utils::StructFromJSON<VMessProtocolConfigObject>(vmessString.toStdString());
        showWarnMessageBox(this, tr("#VMessCheckPassed"), tr("#AbleToImportConfig"));
    } catch (exception *e) {
        showWarnMessageBox(this, tr("#VMessDecodeError"), e->what());
        return;
    }
}

void ImportConfigWindow::on_verifyFileBtn_clicked()
{
    if (!v2Instance::checkConfigFile(ui->fileLineTxt->text())) {
        showWarnMessageBox(this, tr("#InvalidConfigFile"), tr("ConfigFileCheckFailed"));
    } else {
        showWarnMessageBox(this, tr("#VConfigFileCheckPassed"), tr("#AbleToImportConfig"));
    }
}
