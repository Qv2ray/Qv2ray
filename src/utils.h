#ifndef UTILS_H
#define UTILS_H
#include <QJsonObject>

QJsonObject switchJsonArrayObject(QJsonObject objest, QString value);
QJsonObject findValueFromJsonArray(QJsonArray arr, QString key, QString val);
QJsonObject loadRootObjFromConf();
QJsonArray getInbounds();
bool testCoreFiles();
void alterMessage(QString title, QString text);
void overrideInbounds(QString path);
int getIndexInArrayByValue(QJsonArray array, QString key, QString val);

#endif // UTILS_H
