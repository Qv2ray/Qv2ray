#include "hvconf.h"
#include "ui_hvconf.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>

hvConf::hvConf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::hvConf)
{
    ui->setupUi(this);
    QFile loadFile("conf/Hv2ray.config.json");
    loadFile.open(QIODevice::ReadOnly);
    QByteArray allData = loadFile.readAll();
    loadFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    *this->rootObj = v2conf.object();
}

hvConf::~hvConf()
{
    delete ui;
}

void hvConf::on_buttonBox_accepted()
{
}

QJsonObject getInbound()
{
}
