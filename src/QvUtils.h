#ifndef UTILS_H
#define UTILS_H

#include "QvGUIConfigObjects.h"

namespace Qv2ray
{
    namespace Utils
    {
        QTranslator *getTranslator(QString lang);

        QStringList GetFileList(QDir *dir);

        QString Base64Encode(QString string);
        QString Base64Decode(QString string);

        bool CheckFile(QDir *dir, QString fileName);

        void SetConfigDirPath(QString path);
        QString GetConfigDirPath();

        void SetGlobalConfig(Qv2Config conf);
        Qv2Config GetGlobalConfig();

        void SaveGlobalConfig();
        void LoadGlobalConfig();

        void QvMessageBox(QWidget *parent, QString title, QString text);
        //
        void SaveStringToFile(QString text, QFile *target);
        //
        template <typename T>
        void SaveJSONToFile(T t, QFile *targetFile);
        //
        //
        template <typename TYPE>
        QString StructToJSON(const TYPE &t)
        {
            return QString::fromStdString(X::tojson(t, "", 4, ' '));
        }
        //
        //
        template <typename TYPE>
        TYPE StructFromJSON(const string &str)
        {
            TYPE v;
            X::loadjson(str, v, false);
            return v;
        }
    }
}

using namespace Qv2ray::Utils;
#endif // UTILS_H
