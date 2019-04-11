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
        qDebug() << "could't open projects json";
        return;
    }
    QByteArray allData = loadFile.readAll();
    loadFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    QJsonObject rootobj = v2conf.object();
    QJsonObject vnext = rootobj.value("outbound").toObject().value("settings").toObject().value("vnext").toArray().begin()->toObject();
    newConf.host = vnext.value("address").toString();
    newConf.port = QString::number(vnext.value("port").toInt());
    QJsonObject users = vnext.value("users").toArray().begin()->toObject();
    newConf.uuid = users.value("id").toString();
    newConf.alterid = QString::number(users.value("alterId").toInt());
    newConf.security = users.value("security").toString();
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
