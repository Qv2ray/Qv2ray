#include "QvUtils.hpp"
#include <QTextStream>

namespace Qv2ray
{
    namespace Utils
    {
        static Qv2Config GlobalConfig;
        void SetGlobalConfig(Qv2Config conf)
        {
            GlobalConfig = conf;
        }

        Qv2Config GetGlobalConfig()
        {
            return GlobalConfig;
        }

        void SaveGlobalConfig()
        {
            QFile conf(QV2RAY_CONFIG_PATH + "Qv2ray.conf");
            conf.open(QFile::WriteOnly);
            QString jsonConfig = StructToJSON(GetGlobalConfig());
            QTextStream stream(&conf);
            stream << jsonConfig << endl;
            stream.flush();
            conf.close();
        }

        QString base64_encode(QString string)
        {
            QByteArray ba;
            ba.append(string);
            return ba.toBase64();
        }

        QString base64_decode(QString string)
        {
            QByteArray ba;
            ba.append(string);
            return QString(QByteArray::fromBase64(ba));
        }
        void LoadConfig(QString filePath)
        {
            QFile file(filePath);
            file.open(QFile::ReadOnly);
            QTextStream stream(&file);
            auto str = stream.readAll();
            auto config  = StructFromJSON<Qv2Config>(str.toStdString());
            SetGlobalConfig(config);
            file.close();
        }

        QStringList getAllFilesList(QDir *dir)
        {
            return dir->entryList(QStringList() << "*" << "*.*", QDir::Hidden | QDir::Files);
        }
        bool getFileExistance(QDir *dir, QString fileName)
        {
            return getAllFilesList(dir).indexOf(fileName) >= 0;
        }
        void showWarnMessageBox(QWidget *parent, QString title, QString text)
        {
            QMessageBox::warning(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
        }
        QTranslator *getTranslator(string lang)
        {
            QTranslator *translator = new QTranslator();
            translator->load(QString::fromStdString(lang + ".qm"), ":/translations");
            return translator;
        }
    }
}


