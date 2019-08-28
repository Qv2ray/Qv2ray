#ifndef UTILS_H
#define UTILS_H

#include "Qv2rayBase.h"
#include <QMessageBox>
#include <QUuid>

namespace Qv2ray
{
    namespace Utils
    {
        QTranslator *getTranslator(QString lang);

        QStringList getFileList(QDir *dir);

        QString Base64Encode(QString string);
        QString Base64Decode(QString string);

        bool CheckFile(QDir *dir, QString fileName);

        void SetConfigDirPath(QString path);
        QString GetConfigDirPath();

        void SetGlobalConfig(Qv2rayConfig conf);
        Qv2rayConfig GetGlobalConfig();

        void SaveGlobalConfig();
        void LoadGlobalConfig();

        void QvMessageBox(QWidget *parent, QString title, QString text);
        int QvMessageBoxAsk(QWidget *parent, QString title, QString text, QMessageBox::StandardButton extraButtons = QMessageBox::NoButton);
        //
        bool StringToFile(QString text, QFile *target);
        QString StringFromFile(QFile *sourceFile);
        //
        QJsonObject JSONFromString(QString string);
        QString JSONToString(QJsonObject json);
        //
        //
        template <typename TYPE>
        QString StructToJSONString(const TYPE t)
        {
            return QString::fromStdString(X::tojson(t, "", 4, ' '));
        }
        //
        //
        template <typename TYPE>
        TYPE StructFromJSONString(const QString &str)
        {
            TYPE v;
            X::loadjson(str.toStdString(), v, false);
            return v;
        }
    }
}

using namespace Qv2ray::Utils;
#endif // UTILS_H
