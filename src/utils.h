#include <QWidget>

#ifndef UTILS_H
#define UTILS_H
#include <QJsonObject>

QJsonObject switchJsonArrayObject(QJsonObject objest, QString value);
QJsonObject findValueFromJsonArray(QJsonArray arr, QString key, QString val);
QJsonObject loadRootObjFromConf();
QJsonArray getInbounds();
bool checkVCoreExes();
void showWarnMessageBox(QWidget* parent, QString title, QString text);
void overrideInbounds(QString path);
int getIndexByValue(QJsonArray array, QString key, QString val);

#endif // UTILS_H
