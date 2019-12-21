#include "QvUtils.hpp"

namespace Qv2ray
{
    namespace Utils
    {
        static Qv2rayConfig GlobalConfig;
        static QString ConfigDirPath;
        void SetGlobalConfig(Qv2rayConfig conf)
        {
            GlobalConfig = conf;
            QFile config(QV2RAY_CONFIG_FILE);
            QString str = StructToJsonString(GetGlobalConfig());
            StringToFile(&str, &config);
        }

        Qv2rayConfig GetGlobalConfig()
        {
            return GlobalConfig;
        }

        QString GetConfigDirPath()
        {
            return ConfigDirPath;
        }

        void SetConfigDirPath(const QString *path)
        {
            ConfigDirPath = *path;

            if (!path->endsWith("/")) {
                ConfigDirPath += "/";
            }
        }

        void LoadGlobalConfig()
        {
            QFile file(QV2RAY_CONFIG_FILE);
            file.open(QFile::ReadOnly);
            QTextStream stream(&file);
            auto str = stream.readAll();
            auto config  = StructFromJsonString<Qv2rayConfig>(str);
            SetGlobalConfig(config);
            file.close();
        }
    }
}
