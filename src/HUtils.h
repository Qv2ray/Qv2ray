#include <QDir>
#include <QFile>
#include <QMap>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QWidget>
#include <assert.h>

#include "Hv2ConfigObject.h"

#ifndef UTILS_H
#define UTILS_H

namespace Hv2ray::Utils
{
    QJsonObject switchJsonArrayObject(QJsonObject objest, QString value);
    QJsonObject findValueFromJsonArray(QJsonArray arr, QString key, QString val);
    QJsonObject loadRootObjFromConf();
    QJsonArray getInbounds();
    void showWarnMessageBox(QWidget *parent, QString title, QString text);
    void overrideInbounds(QString path);
    int getIndexByValue(QJsonArray array, QString key, QString val);

    QStringList getAllFilesList(QDir *dir);
    bool hasFile(QDir *dir, QString fileName);

    template <typename TYPE>
    QString StructToJSON(const TYPE &t)
    {
        string s;
        encode_json<TYPE>(t, s, indenting::indent);
        return QString::fromStdString(s);
    }

    template <typename TYPE>
    TYPE StructFromJSON(const std::string &str)
    {
        TYPE v = decode_json<TYPE>(str);
        return v;
    }
}

#endif // UTILS_H
