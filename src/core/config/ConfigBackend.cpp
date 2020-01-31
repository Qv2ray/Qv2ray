#include "ConfigBackend.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::config
{
    void SaveGlobalConfig(Qv2rayConfig conf)
    {
        GlobalConfig = conf;
        QFile config(QV2RAY_CONFIG_FILE);
        QString str = StructToJsonString(conf);
        StringToFile(&str, &config);
    }

    void SetConfigDirPath(const QString &path)
    {
        Qv2rayConfigPath = path;

        if (!path.endsWith("/")) {
            Qv2rayConfigPath += "/";
        }
    }

    void LoadGlobalConfig()
    {
        QFile file(QV2RAY_CONFIG_FILE);
        file.open(QFile::ReadOnly);
        QTextStream stream(&file);
        auto str = stream.readAll();
        auto config = StructFromJsonString<Qv2rayConfig>(str);
        SaveGlobalConfig(config);
        file.close();
    }
}

using namespace Qv2ray::core::config;
