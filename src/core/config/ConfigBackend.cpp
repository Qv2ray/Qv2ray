#include "ConfigBackend.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::config
{
    void SaveGlobalConfig(const Qv2rayConfig &conf)
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
}

using namespace Qv2ray::core::config;
