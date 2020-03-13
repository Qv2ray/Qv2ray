#include "SettingsBackend.hpp"

#include "common/QvHelpers.hpp"

namespace Qv2ray::core::config
{
    void SaveGlobalConfig(const Qv2rayConfig &conf)
    {
        GlobalConfig = conf;
        QString str = StructToJsonString(conf);
        StringToFile(str, QV2RAY_CONFIG_FILE);
    }

    void SetConfigDirPath(const QString &path)
    {
        Qv2rayConfigPath = path;

        if (!path.endsWith("/"))
        {
            Qv2rayConfigPath += "/";
        }
    }

    bool CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig)
    {
        auto path = _path;
        if (!path.endsWith("/"))
        {
            path.append("/");
        }
        // Does not exist.
        if (!QDir(path).exists())
            return false;

        // A temp file used to test file permissions in that folder.
        QFile testFile(path + ".qv2ray_file_write_test_file" + QSTRN(QTime::currentTime().msecsSinceStartOfDay()));
        bool opened = testFile.open(QFile::OpenModeFlag::ReadWrite);

        if (!opened)
        {
            LOG(MODULE_SETTINGS, "Directory at: " + path + " cannot be used as a valid config file path.")
            LOG(MODULE_SETTINGS, "---> Cannot create a new file or open a file for writing.")
            return false;
        }
        else
        {
            testFile.write("Qv2ray test file, feel free to remove.");
            testFile.flush();
            testFile.close();
            bool removed = testFile.remove();

            if (!removed)
            {
                // This is rare, as we can create a file but failed to remove it.
                LOG(MODULE_SETTINGS, "Directory at: " + path + " cannot be used as a valid config file path.")
                LOG(MODULE_SETTINGS, "---> Cannot remove a file.")
                return false;
            }
        }

        if (checkExistingConfig)
        {
            // Check if an existing config is found.
            QFile configFile(path + "Qv2ray.conf");

            // No such config file.
            if (!configFile.exists())
                return false;

            bool opened2 = configFile.open(QIODevice::ReadWrite);

            try
            {
                if (opened2)
                {
                    // Verify if the config can be loaded.
                    // Failed to parse if we changed the file structure...
                    // Original:
                    //  --  auto conf =
                    //  StructFromJsonString<Qv2rayConfig>(configFile.readAll());
                    //
                    // Verify JSON file format. (only) because this file version may
                    // not be upgraded and may contain unsupported structure.
                    auto err = VerifyJsonString(StringFromFile(&configFile));

                    if (!err.isEmpty())
                    {
                        LOG(MODULE_INIT, "Json parse returns: " + err)
                        return false;
                    }
                    else
                    {
                        // If the file format is valid.
                        auto conf = JsonFromString(StringFromFile(&configFile));
                        LOG(MODULE_SETTINGS,
                            "Path: " + path + " contains a config file, in version " + conf["config_version"].toVariant().toString())
                        configFile.close();
                        return true;
                    }
                }
                else
                {
                    LOG(MODULE_SETTINGS, "File: " + configFile.fileName() + " cannot be opened!")
                    return false;
                }
            }
            catch (...)
            {
                LOG(MODULE_SETTINGS, "Exception raised when checking config: " + configFile.fileName())
                // LOG(INIT, e->what())
                QvMessageBoxWarn(nullptr, QObject::tr("Warning"),
                                 QObject::tr("Qv2ray cannot load the config file from here:") + NEWLINE + configFile.fileName());
                return false;
            }
        }
        else
            return true;
    }

} // namespace Qv2ray::core::config

using namespace Qv2ray::core::config;
