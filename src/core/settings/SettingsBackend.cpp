#include "SettingsBackend.hpp"

#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "SettingsBackend"

namespace Qv2ray::core::config
{
    void SaveGlobalSettings()
    {
        const auto str = JsonToString(GlobalConfig.toJson());
        StringToFile(str, QV2RAY_CONFIG_FILE);
    }

    void SetConfigDirPath(const QString &path)
    {
        qvApplicationInstance->ConfigPath = path;

        if (!path.endsWith("/"))
        {
            qvApplicationInstance->ConfigPath += "/";
        }
    }

    bool CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig)
    {
        auto path = _path;

        if (!path.endsWith("/"))
            path.append("/");

        // Does not exist.
        if (!QDir(path).exists())
            return false;

        {
            // A temp file used to test file permissions in that folder.
            QFile testFile(path + ".qv2ray_test_file" + QSTRN(QTime::currentTime().msecsSinceStartOfDay()));

            if (!testFile.open(QFile::OpenModeFlag::ReadWrite))
            {
                LOG("Directory at: " + path + " cannot be used as a valid config file path.");
                LOG("---> Cannot create a new file or open a file for writing.");
                return false;
            }

            testFile.write("Qv2ray test file, feel free to remove.");
            testFile.flush();
            testFile.close();

            if (!testFile.remove())
            {
                // This is rare, as we can create a file but failed to remove it.
                LOG("Directory at: " + path + " cannot be used as a valid config file path.");
                LOG("---> Cannot remove a file.");
                return false;
            }
        }

        if (!checkExistingConfig)
        {
            // Just pass the test
            return true;
        }

        // Check if an existing config is found.
        QFile configFile(path + "Qv2ray.conf");

        // No such config file.
        if (!configFile.exists())
            return false;

        if (!configFile.open(QIODevice::ReadWrite))
        {
            LOG("File: " + configFile.fileName() + " cannot be opened!");
            return false;
        }

        const auto err = VerifyJsonString(StringFromFile(configFile));
        if (!err.isEmpty())
        {
            LOG("Json parse returns:", err);
            return false;
        }

        // If the file format is valid.
        const auto conf = JsonFromString(StringFromFile(configFile));
        LOG("Found a config file," A(conf["config_version"].toString()) A(path));
        configFile.close();
        return true;
    }

} // namespace Qv2ray::core::config

using namespace Qv2ray::core::config;
