#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "confedit.h"
#include "vinteract.h"
#include "utils.h"
#include "importconf.h"
#include "ui_importconf.h"

importConf::importConf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::importConf)
{
    ui->setupUi(this);
    connect(this, SIGNAL(updateConfTable()), parentWidget(), SLOT(updateConfTable()));
}

importConf::~importConf()
{
    delete ui;
}

void importConf::on_pushButton_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Open Config File"), "~/");
    ui->fileLinedit->setText(dir);
}

void importConf::savefromFile(QString path, QString alias)
{
    vConfig newConf;
    newConf.alias = alias;
    QFile configFile(path);
    if(!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Could't open config json";
        return;
    }
    QByteArray allData = configFile.readAll();
    configFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    QJsonObject rootobj = v2conf.object();
    QJsonObject outbound;
    if(rootobj.contains("outbounds")) {
        outbound = rootobj.value("outbounds").toArray().first().toObject();
    } else {
        outbound = rootobj.value("outbound").toObject();
    }
    QJsonObject vnext = switchJsonArrayObject(outbound.value("settings").toObject(), "vnext");
    QJsonObject user = switchJsonArrayObject(vnext, "users");
    newConf.host = vnext.value("address").toString();
    newConf.port = QString::number(vnext.value("port").toInt());
    newConf.alterid = QString::number(user.value("alterId").toInt());
    newConf.uuid = user.value("id").toString();
    newConf.security = user.value("security").toString();
    if (newConf.security.isNull()) {
        newConf.security = "auto";
    }
    newConf.isCustom = 1;
    int id = newConf.save();
    emit updateConfTable();
    QString newFile = "conf/" + QString::number(id) + ".conf";
    if(!QFile::copy(path, newFile)) {
        alterMessage("Copy error", "Failed to copy custom config file.");
    }
}

void importConf::on_buttonBox_accepted()
{
    QString path = ui->fileLinedit->text();
    QString alias = ui-> aliasLinedit->text();
    bool isValid = validationCheck(path);
    if(isValid) {
        savefromFile(path, alias);
    }
}
