#include "importconf.h"
#include "ui_importconf.h"
#include <QFileDialog>
#include "confedit.h"
#include "vinteract.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include "utils.h"

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
    QFile loadFile(path);
    if(!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "could't open config json";
        return;
    }
    QByteArray allData = loadFile.readAll();
    loadFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    QJsonObject rootobj = v2conf.object();
    QJsonObject outbound;
    if(rootobj.contains("outbounds")) {
        outbound = rootobj.value("outbounds").toArray().first().toObject();
    } else {
        outbound = rootobj.value("outbound").toObject();
    }
    QJsonObject vnext = parseJson(outbound.value("settings").toObject(), "vnext");
    QJsonObject user = parseJson(vnext, "users");
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
        QMessageBox::critical(0, "Copy error", "Failed to copy custom config file.", QMessageBox::Ok | QMessageBox::Default);
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
