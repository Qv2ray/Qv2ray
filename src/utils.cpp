#include "utils.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>


QJsonObject switchJsonArrayObject(QJsonObject obj, QString value)
{
    QJsonObject returnObj;
    if(obj.value(value).isNull()) {
        returnObj = obj.value(value).toObject();
    } else {
        returnObj = obj.value(value).toArray().first().toObject();
    }
    return returnObj;
}

QJsonObject findValueFromJsonArray(QJsonArray arr, QString key, QString val)
{
    for (const auto obj : arr) {
        if (obj.toObject().value(key).toString() == val) {
            return obj.toObject();
        }
    }
    return QJsonObject();
}

QJsonObject loadRootObjFromConf()
{
    QFile loadFile("conf/Hv2ray.config.json");
    loadFile.open(QIODevice::ReadOnly);
    QByteArray allData = loadFile.readAll();
    loadFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    QJsonObject rootObj = v2conf.object();
    return rootObj;
}

QJsonArray getInbounds()
{
    QJsonArray inbounds;
    inbounds = loadRootObjFromConf().value("inbounds").toArray();
    return inbounds;
}

bool getRootEnabled()
{
    return loadRootObjFromConf().value("v2suidEnabled").toBool();
}

bool testCoreFiles()
{
    if (QFileInfo("v2ray").exists() && QFileInfo("geoip.dat").exists() && QFileInfo("geosite.dat").exists() && QFileInfo("v2ctl").exists()) {
        return true;
    } else {
        alterMessage("v2ray core not found", "V2ray core files not found. Please download the latest version of v2ray and extract it into the current folder.");
        return false;
    }
}

void alterMessage(QString title, QString text)
{
    QMessageBox::critical(0, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
}
