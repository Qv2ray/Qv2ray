#include "QvUtils.h"
#include <QTextStream>

namespace Qv2ray
{
    namespace Utils
    {
        static Qv2rayConfig GlobalConfig;
        static QString ConfigDirPath;
        void SetGlobalConfig(Qv2rayConfig conf)
        {
            GlobalConfig = conf;
        }

        Qv2rayConfig GetGlobalConfig()
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
            QFile config(QV2RAY_CONFIG_FILE_PATH);
            QString str = StructToJSONString(GetGlobalConfig());
            StringToFile(str, &config);
        }

        bool StringToFile(QString text, QFile *targetFile)
        {
            bool override = targetFile->exists();
            targetFile->open(QFile::WriteOnly);
            QTextStream stream(targetFile);
            stream << text << endl;
            stream.flush();
            targetFile->close();
            return override;
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
            QFile file(QV2RAY_CONFIG_FILE_PATH);
            file.open(QFile::ReadOnly);
            QTextStream stream(&file);
            auto str = stream.readAll();
            auto config  = StructFromJSONString<Qv2rayConfig>(str);
            SetGlobalConfig(config);
            file.close();
        }

        QStringList getFileList(QDir *dir)
        {
            return dir->entryList(QStringList() << "*" << "*.*", QDir::Hidden | QDir::Files);
        }

        bool CheckFile(QDir *dir, QString fileName)
        {
            return getFileList(dir).indexOf(fileName) >= 0;
        }

        void QvMessageBox(QWidget *parent, QString title, QString text)
        {
            QMessageBox::warning(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
        }

        int QvMessageBoxAsk(QWidget *parent, QString title, QString text, QMessageBox::StandardButton extraButtons)
        {
            return QMessageBox::information(parent, title, text, QMessageBox::Yes | QMessageBox::No | extraButtons);
        }


        QTranslator *getTranslator(const QString *lang)
        {
            QTranslator *translator = new QTranslator();
            translator->load(*lang + ".qm", ":/translations/");
            return translator;
        }
    }
}


