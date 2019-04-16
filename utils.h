#ifndef UTILS_H
#define UTILS_H
#include <QJsonObject>

class utils
{
public:
    utils();
    QJsonObject parseJson(QJsonObject objest, QString value);
};

#endif // UTILS_H
