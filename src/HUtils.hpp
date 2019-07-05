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

namespace Hv2ray
{
    namespace Utils
    {
        void showWarnMessageBox(QWidget *parent, QString title, QString text);
        QTranslator *getTranslator(string lang);
        void SetGlobalConfig(HConfigModels::Hv2Config conf);
        HConfigModels::Hv2Config GetGlobalConfig();
        void SaveConfig(QFile *configFile);
        void LoadConfig(QFile *configFile);
        /// Get file list in a Dir
        QStringList getAllFilesList(QDir *dir);
        bool hasFile(QDir *dir, QString fileName);

        template <typename TYPE>
        QString StructToJSON(const TYPE &t)
        {
            string s;
#if USE_TODO_FEATURES
            encode_json<TYPE>(t, s, indenting::indent);
#else
            s = X::tojson(t, "", 4, ' ');
#endif
            cout << s << endl;
            return QString::fromStdString(s);
        }

        template <typename TYPE>
        TYPE StructFromJSON(const string &str)
        {
            TYPE v;
#if USE_TODO_FEATURES
            v = decode_json<TYPE>(str);
#else
            X::loadjson(str, v, false);
#endif
            return v;
        }
    }
}
#endif // UTILS_H
