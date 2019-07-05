#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "HUtils.hpp"
#include "vinteract.hpp"
#include "w_ConnectionEditWindow.h"
#include "w_ImportConfig.h"


using namespace Qv2ray;

ImportConfig::ImportConfig(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImportConfigWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(updateConfTable()), parentWidget(), SLOT(updateConfTable()));
}

ImportConfig::~ImportConfig()
{
    delete ui;
}

void ImportConfig::on_pushButton_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("OpenConfigFile"), "~/");
    ui->fileLineTxt->setText(dir);
}

void ImportConfig::savefromFile(QString path, QString alias)
{
    Q_UNUSED(path)
    Q_UNUSED(alias)
    //Hv2Config newConfig;
    //newConfig.alias = alias;
    //QFile configFile(path);
    //if(!configFile.open(QIODevice::ReadOnly)) {
    //    showWarnMessageBox(this, tr("ImportConfig"), tr("CannotOpenFile"));
    //    qDebug() << "ImportConfig::CannotOpenFile";
    //    return;
    //}
    //QByteArray allData = configFile.readAll();
    //configFile.close();
    //QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    //QJsonObject rootobj = v2conf.object();
    //QJsonObject outbound;
    //if(rootobj.contains("outbounds")) {
    //    outbound = rootobj.value("outbounds").toArray().first().toObject();
    //} else {
    //    outbound = rootobj.value("outbound").toObject();
    //}
    //QJsonObject vnext = switchJsonArrayObject(outbound.value("settings").toObject(), "vnext");
    //QJsonObject user = switchJsonArrayObject(vnext, "users");
    //newConfig.host = vnext.value("address").toString();
    //newConfig.port = QString::number(vnext.value("port").toInt());
    //newConfig.alterid = QString::number(user.value("alterId").toInt());
    //newConfig.uuid = user.value("id").toString();
    //newConfig.security = user.value("security").toString();
    //if (newConfig.security.isNull()) {
    //    newConfig.security = "auto";
    //}
    //newConfig.isCustom = 1;
    //int id = newConfig.save();
    //if(id < 0)
    //{
    //    showWarnMessageBox(this, tr("ImportConfig"), tr("SaveFailed"));
    //    qDebug() << "ImportConfig::SaveFailed";
    //    return;
    //}
    //emit updateConfTable();
    //QString newFile = "conf/" + QString::number(id) + ".conf";
    //if(!QFile::copy(path, newFile)) {
    //    showWarnMessageBox(this, tr("ImportConfig"), tr("CannotCopyCustomConfig"));
    //    qDebug() << "ImportConfig::CannotCopyCustomConfig";
    //}
}

void ImportConfig::on_buttonBox_accepted()
{
    QString alias = ui->nameTxt->text();

    if (ui->importSourceCombo->currentIndex() == 0) { // From File...
        QString path = ui->fileLineTxt->text();
        bool isValid = v2Instance::checkConfigFile(path);

        if (!isValid) {
            // Invalid file alert.
            return;
        }

        savefromFile(path, alias);
    } else {
        QString vmess = ui->vmessConnectionStringTxt->toPlainText();

        if (!vmess.toLower().startsWith("vmess://")) {
            Utils::showWarnMessageBox(this, tr("#VMessDecodeError"), tr("#NotValidVMessProtocolString"));
        }

        QStringRef vmessJsonB64(&vmess, 8, vmess.length() - 8);
        auto vmessString = Utils::base64_decode(vmessJsonB64.toString());
        auto vmessConf = Utils::StructFromJSON<V2ConfigModels::VMessProtocolConfigObject>(vmessString.toStdString());
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
