#include "QvUtils.h"
#include <QTextStream>
#include <QMessageBox>

namespace Qv2ray
{
    namespace Utils
    {
        static Qv2Config GlobalConfig;
        static QString ConfigDirPath;
        void SetGlobalConfig(Qv2Config conf)
        {
            GlobalConfig = conf;
        }

        Qv2Config GetGlobalConfig()
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
            SaveJSONToFile(GetGlobalConfig(), &config);
        }

        template <typename T>
        void SaveJSONToFile(T t, QFile *targetFile)
        {
            SaveStringToFile(StructToJSON(t), targetFile);
        }

        void SaveStringToFile(QString text, QFile *targetFile)
        {
            targetFile->open(QFile::WriteOnly);
            QTextStream stream(targetFile);
            stream << text << endl;
            stream.flush();
            targetFile->close();
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
            auto config  = StructFromJSON<Qv2Config>(str.toStdString());
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


