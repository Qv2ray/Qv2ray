#ifndef UTILS_H
#define UTILS_H

#include "Qv2rayBase.h"
#include <QMessageBox>
#include <QUuid>

namespace Qv2ray
{
    namespace Utils
    {
        QTranslator *getTranslator(const QString *lang);

        QStringList getFileList(QDir *dir);

        QString Base64Encode(QString string);
        QString Base64Decode(QString string);

        bool CheckFile(QDir *dir, QString fileName);

        void SetConfigDirPath(const QString *path);
        QString GetConfigDirPath();

        void SetGlobalConfig(Qv2rayConfig conf);
        Qv2rayConfig GetGlobalConfig();

        void LoadGlobalConfig();

        void QvMessageBox(QWidget *parent, QString title, QString text);
        int QvMessageBoxAsk(QWidget *parent, QString title, QString text, QMessageBox::StandardButton extraButtons = QMessageBox::NoButton);
        //
        bool StringToFile(const QString *text, QFile *target);
        QString StringFromFile(QFile *sourceFile);
        //
        QJsonObject JsonFromString(QString string);
        QString JsonToString(QJsonObject json);
        QString VerifyJsonString(const QString *source);
        //
        QString Stringify(list<string> list, QString saperator = ";");
        QString Stringify(QList<QString> list, QString saperator = ";");
        //
        //
        template <typename TYPE>
        QString StructToJsonString(const TYPE t)
        {
            return QString::fromStdString(X::tojson(t, "", 4, ' '));
        }
        //
        //
        template <typename TYPE>
        TYPE StructFromJsonString(const QString &str)
        {
            TYPE v;
            X::loadjson(str.toStdString(), v, false);
            return v;
        }
        QString FormatBytes(long long bytes, char *str);
    }
}

using namespace Qv2ray::Utils;
#endif // UTILS_H
