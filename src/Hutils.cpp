#include "HUtils.h"

using namespace Hv2ray;

QJsonObject Utils::switchJsonArrayObject(QJsonObject obj, QString value)
{
    QJsonObject returnObj = obj.value(value).isNull()
                            ? obj.value(value).toObject()
                            : obj.value(value).toArray().first().toObject();
    return returnObj;
}

QJsonObject Utils::findValueFromJsonArray(QJsonArray arr, QString key, QString val)
{
    for (const auto obj : arr) {
        if (obj.toObject().value(key).toString() == val) {
            return obj.toObject();
        }
    }

    return QJsonObject();
}

QJsonObject Utils::loadRootObjFromConf()
{
    QFile globalConfigFile("Hv2ray.conf");
    globalConfigFile.open(QIODevice::ReadOnly);
    QByteArray conf = globalConfigFile.readAll();
    globalConfigFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(conf));
    QJsonObject rootObj = v2conf.object();
    return rootObj;
}

QJsonArray Utils::getInbounds()
{
    QJsonArray inbounds;
    inbounds = loadRootObjFromConf().value("inbounds").toArray();
    return inbounds;
}

void Utils::showWarnMessageBox(QWidget *parent, QString title, QString text)
{
    QMessageBox::warning(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
}

void Utils::overrideInbounds(QString path)
{
    QFile confFile(path);
    confFile.open(QIODevice::ReadOnly);
    QByteArray conf = confFile.readAll();
    confFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(conf));
    QJsonObject rootObj = v2conf.object();
    QJsonArray modifiedIn = getInbounds();
    rootObj.remove("inbounds");
    rootObj.remove("inbound");
    rootObj.remove("inboundDetour");
    rootObj.insert("inbounds", QJsonValue(modifiedIn));
    v2conf.setObject(rootObj);
    conf = v2conf.toJson(QJsonDocument::Indented);
    confFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    confFile.write(conf);
    confFile.close();
}

int Utils::getIndexByValue(QJsonArray array, QString key, QString val)
{
    QJsonArray::iterator it;
    int index = 0;

    for (it = array.begin(); it != array.end(); it++) {
        if (it->toObject().value(key) == val) {
            return index;
        }

        index++;
    }

    return -1;
}

/// Get file list in a Dir
QStringList Utils::getAllFilesList(QDir *dir)
{
    return dir->entryList(QStringList() << "*" << "*.*", QDir::Hidden | QDir::Files);
}

bool Utils::hasFile(QDir *dir, QString fileName)
{
    return getAllFilesList(dir).indexOf(fileName) >= 0;
}
