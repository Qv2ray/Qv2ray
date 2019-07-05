#ifndef UTILS_H
#define UTILS_H
#include <QDir>
#include <QFile>
#include <QMap>
#include <QFileInfo>
#include <QMessageBox>
#include <QWidget>
#include <assert.h>
#include <QTranslator>
#include <QApplication>

#include <iostream>

#include "HConfigObjects.hpp"

namespace Qv2ray
{
    namespace Utils
    {
        void showWarnMessageBox(QWidget *parent, QString title, QString text);
        QTranslator *getTranslator(string lang);
        void SetGlobalConfig(QvConfigModels::Qv2Config conf);
        QvConfigModels::Qv2Config GetGlobalConfig();
        void SaveConfig(QFile *configFile);
        void LoadConfig(QFile *configFile);
        /// Get file list in a Dir
        QStringList getAllFilesList(QDir *dir);
        bool hasFile(QDir *dir, QString fileName);
        QString base64_encode(QString string);
        QString base64_decode(QString string);
        template <typename TYPE>
        QString StructToJSON(const TYPE &t)
        {
            string s;
            s = X::tojson(t, "", 4, ' ');
            cout << s << endl;
            return QString::fromStdString(s);
        }

        template <typename TYPE>
        TYPE StructFromJSON(const string &str)
        {
            TYPE v;
            X::loadjson(str, v, false);
            return v;
        }
    }
}
#endif // UTILS_H
