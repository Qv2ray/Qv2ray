#include "QvUtils.h"
#include <QTextStream>
#include <QMessageBox>

namespace Qv2ray
{
    namespace Utils
    {
        static Qv2Config_v1 GlobalConfig;
        static QString ConfigDirPath;
        void SetGlobalConfig(Qv2Config_v1 conf)
        {
            GlobalConfig = conf;
        }

        Qv2Config_v1 GetGlobalConfig()
        {
            return GlobalConfig;
        }

        QString GetConfigDirPath()
        {
            return ConfigDirPath;
        }

        void SetConfigDirPath(QString path)
        {
            ConfigDirPath = path;
        }

        void SaveGlobalConfig()
        {
            QFile config(QV2RAY_MAIN_CONFIG_FILE_PATH);
            QString str = StructToJSONString(GetGlobalConfig());
            StringToFile(str, &config);
        }

        void StringToFile(QString text, QFile *targetFile)
        {
            targetFile->open(QFile::WriteOnly);
            QTextStream stream(targetFile);
            stream << text << endl;
            stream.flush();
            targetFile->close();
        }

        QJsonObject JSONFromFile(QFile *sourceFile)
        {
            QString json = StringFromFile(sourceFile);
            return JSONFromString(json);
        }

        QString JSONToString(QJsonObject json)
        {
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson();
        }

        QJsonObject JSONFromString(QString string)
        {
            QJsonDocument doc = QJsonDocument::fromJson(string.toUtf8());
            return doc.object();
        }

        QString StringFromFile(QFile *sourceFile)
        {
            sourceFile->open(QFile::ReadOnly);
            QTextStream stream(sourceFile);
            QString str = stream.readAll();
            sourceFile->close();
            return str;
        }

        QString Base64Encode(QString string)
        {
            QByteArray ba;
            ba.append(string);
            return ba.toBase64();
        }

        QString Base64Decode(QString string)
        {
            QByteArray ba;
            ba.append(string);
            return QString(QByteArray::fromBase64(ba));
        }

        void LoadGlobalConfig()
        {
            QFile file(QV2RAY_MAIN_CONFIG_FILE_PATH);
            file.open(QFile::ReadOnly);
            QTextStream stream(&file);
            auto str = stream.readAll();
            auto config  = StructFromJSONString<Qv2Config_v1>(str.toStdString());
            SetGlobalConfig(config);
            file.close();
        }

        QStringList GetFileList(QDir *dir)
        {
            return dir->entryList(QStringList() << "*" << "*.*", QDir::Hidden | QDir::Files);
        }

        bool CheckFile(QDir *dir, QString fileName)
        {
            return GetFileList(dir).indexOf(fileName) >= 0;
        }

        void QvMessageBox(QWidget *parent, QString title, QString text)
        {
            QMessageBox::warning(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
        }

        QTranslator *getTranslator(QString lang)
        {
            QTranslator *translator = new QTranslator();
            translator->load(lang + ".qm", ":/translations");
            return translator;
        }
    }
}


