#include "utils.h"
#include <QJsonArray>
#include <QJsonObject>

QJsonObject parseJson(QJsonObject obj, QString value)
{
    QJsonObject returnObj;
    if(obj.value(value).isNull()) {
        returnObj = obj.value(value).toObject();
    } else {
        returnObj = obj.value(value).toArray().first().toObject();
    }
    return returnObj;
}
