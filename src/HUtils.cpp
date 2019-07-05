#include "HUtils.hpp"
#include <QTextStream>

namespace Qv2ray
{
    namespace Utils
    {
        static QvConfigModels::Qv2Config GlobalConfig;
        void SetGlobalConfig(QvConfigModels::Qv2Config conf)
        {
            GlobalConfig = conf;
        }

        QvConfigModels::Qv2Config GetGlobalConfig()
        {
            return GlobalConfig;
        }

        void SaveConfig(QFile *configFile)
        {
            configFile->open(QFile::WriteOnly);
            QString jsonConfig = StructToJSON(GetGlobalConfig());
            QTextStream stream(configFile);
            stream << jsonConfig << endl;
            stream.flush();
            configFile->close();
        }

        void LoadConfig(QFile *configFile)
        {
            using namespace Qv2ray::QvConfigModels;
            configFile->open(QFile::ReadOnly);
            QTextStream stream(configFile);
            auto str = stream.readAll();
            auto config  = StructFromJSON<Qv2Config>(str.toStdString());
            SetGlobalConfig(config);
            configFile->close();
        }

        QStringList getAllFilesList(QDir *dir)
        {
            return dir->entryList(QStringList() << "*" << "*.*", QDir::Hidden | QDir::Files);
        }
        QTranslator *getTranslator(string lang)
        {
            QTranslator *translator = new QTranslator();
            translator->load(QString::fromStdString(lang + ".qm"), ":/translations");
            return translator;
        }
        bool hasFile(QDir *dir, QString fileName)
        {
            return getAllFilesList(dir).indexOf(fileName) >= 0;
        }
        void showWarnMessageBox(QWidget *parent, QString title, QString text)
        {
            QMessageBox::warning(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
        }
    }
}


