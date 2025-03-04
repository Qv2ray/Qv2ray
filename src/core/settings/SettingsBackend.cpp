#include "SettingsBackend.hpp"

#include "base/Qv2rayLog.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "SettingsBackend"
constexpr auto QV2RAY_CONFIG_PATH_ENV_NAME = "QV2RAY_CONFIG_PATH";

namespace Qv2ray::core::config
{
    void SaveGlobalSettings()
    {
        const auto str = JsonToString(GlobalConfig.toJson());
        StringToFile(str, QV2RAY_CONFIG_FILE);
    }

    void SetConfigDirPath(const QString &path)
    {
        QvCoreApplication->ConfigPath = path;

        if (!path.endsWith("/"))
        {
            QvCoreApplication->ConfigPath += "/";
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
        LOG("Found a config file," QVLOG_A(conf["config_version"].toString()) QVLOG_A(path));
        configFile.close();
        return true;
    }

    bool LocateConfiguration()
    {
        LOG("Application exec path: " + qApp->applicationDirPath());
        // Non-standard paths needs special handing for "_debug"
        const auto currentPathConfig = qApp->applicationDirPath() + "/config" QV2RAY_CONFIG_DIR_SUFFIX;
        const auto homeQv2ray = QDir::homePath() + "/.qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
        //
        // Standard paths already handles the "_debug" suffix for us.
        const auto configQv2ray = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        //
        //
        // Some built-in search paths for Qv2ray to find configs. (load the first one if possible).
        const auto useManualConfigPath = qEnvironmentVariableIsSet(QV2RAY_CONFIG_PATH_ENV_NAME);
        const auto manualConfigPath = qEnvironmentVariable(QV2RAY_CONFIG_PATH_ENV_NAME);
        //
        QStringList configFilePaths;
        if (useManualConfigPath)
        {
            LOG("Using config path from env: " + manualConfigPath);
            configFilePaths << manualConfigPath;
        }
        else
        {
            configFilePaths << currentPathConfig;
            configFilePaths << configQv2ray;
            configFilePaths << homeQv2ray;
        }

        QString configPath = "";
        bool hasExistingConfig = false;
        for (const auto &path : configFilePaths)
        {
            // Verify the config path, check if the config file exists and in the
            // correct JSON format. True means we check for config existence as
            // well. ---------------------------------------------|HERE|
            bool isValidConfigPath = CheckSettingsPathAvailability(path, true);

            // If we already found a valid config file. just simply load it...
            if (hasExistingConfig)
                break;

            if (isValidConfigPath)
            {
                DEBUG("Path:", path, " is valid.");
                configPath = path;
                hasExistingConfig = true;
            }
            else
            {
                LOG("Path:", path, "does not contain a valid config file.");
            }
        }

        if (hasExistingConfig)
        {
            // Use the config path found by the checks above
            SetConfigDirPath(configPath);
            LOG("Using ", QV2RAY_CONFIG_DIR, " as the config path.");
        }
        else
        {
            // If there's no existing config.
            //
            // Create new config at these dirs, these are default values for each platform.
            if (useManualConfigPath)
            {
                configPath = manualConfigPath;
            }
            else
            {
#if defined(Q_OS_WIN) && !defined(QV2RAY_NO_ASIDECONFIG)
                configPath = currentPathConfig;
#else
                configPath = configQv2ray;
#endif
            }

            bool hasPossibleNewLocation = QDir().mkpath(configPath) && CheckSettingsPathAvailability(configPath, false);
            // Check if the dirs are write-able
            if (!hasPossibleNewLocation)
            {
                // None of the path above can be used as a dir for storing config.
                // Even the last folder failed to pass the check.
                LOG("FATAL");
                LOG(" ---> CANNOT find a proper place to store Qv2ray config files.");
                QvMessageBoxWarn(nullptr, QObject::tr("Cannot Start Qv2ray"),
                                 QObject::tr("Cannot find a place to store config files.") + NEWLINE +                                          //
                                     QObject::tr("Qv2ray has searched these paths below:") + NEWLINE + NEWLINE +                                //
                                     configFilePaths.join(NEWLINE) + NEWLINE +                                                                  //
                                     QObject::tr("It usually means you don't have the write permission to all of those locations.") + NEWLINE + //
                                     QObject::tr("Qv2ray will now exit."));                                                                     //
                return false;
            }

            // Found a valid config dir, with write permission, but assume no config is located in it.
            LOG("Set " + configPath + " as the config path.");
            SetConfigDirPath(configPath);

            if (QFile::exists(QV2RAY_CONFIG_FILE))
            {
                // As we already tried to load config from every possible dir.
                //
                // This condition branch (!hasExistingConfig check) holds the fact that current config dir,
                // should NOT contain any valid file (at least in the same name)
                //
                // It usually means that QV2RAY_CONFIG_FILE here has a corrupted JSON format.
                //
                // Otherwise Qv2ray would have loaded this config already instead of notifying to create a new config in this folder.
                //
                LOG("This should not occur: Qv2ray config exists but failed to load.");
                QvMessageBoxWarn(nullptr, QObject::tr("Failed to initialise Qv2ray"),
                                 QObject::tr("Failed to determine the location of config file:") + NEWLINE +                                   //
                                     QObject::tr("Qv2ray has found a config file, but it failed to be loaded due to some errors.") + NEWLINE + //
                                     QObject::tr("A workaround is to remove the this file and restart Qv2ray:") + NEWLINE +                    //
                                     QV2RAY_CONFIG_FILE + NEWLINE +                                                                            //
                                     QObject::tr("Qv2ray will now exit.") + NEWLINE +                                                          //
                                     QObject::tr("Please report if you think it's a bug."));                                                   //
                return false;
            }

            GlobalConfig.kernelConfig.KernelPath(QV2RAY_DEFAULT_VCORE_PATH);
            GlobalConfig.kernelConfig.AssetsPath(QV2RAY_DEFAULT_VASSETS_PATH);
            GlobalConfig.logLevel = 3;
            GlobalConfig.uiConfig.language = QLocale::system().name();
            GlobalConfig.defaultRouteConfig.dnsConfig.servers.append({ "1.1.1.1" });
            GlobalConfig.defaultRouteConfig.dnsConfig.servers.append({ "8.8.8.8" });
            GlobalConfig.defaultRouteConfig.dnsConfig.servers.append({ "8.8.4.4" });

            // Save initial config.
            SaveGlobalSettings();
            LOG("Created initial config file.");
        }

        if (!QDir(QV2RAY_GENERATED_DIR).exists())
        {
            // The dir used to generate final config file, for V2Ray interaction.
            QDir().mkdir(QV2RAY_GENERATED_DIR);
            LOG("Created config generation dir at: " + QV2RAY_GENERATED_DIR);
        }
        //
        // BEGIN LOAD CONFIGURATIONS
        //
        {
            // Load the config for upgrade, but do not parse it to the struct.
            auto conf = JsonFromString(StringFromFile(QV2RAY_CONFIG_FILE));
            const auto configVersion = conf["config_version"].toInt();

            if (configVersion > QV2RAY_CONFIG_VERSION)
            {
                // Config version is larger than the current version...
                // This is rare but it may happen....
                QvMessageBoxWarn(nullptr, QObject::tr("Qv2ray Cannot Continue"),                                                           //
                                 QObject::tr("You are running a lower version of Qv2ray compared to the current config file.") + NEWLINE + //
                                     QObject::tr("Please check if there's an issue explaining about it.") + NEWLINE +                      //
                                     QObject::tr("Or submit a new issue if you think this is an error.") + NEWLINE + NEWLINE +             //
                                     QObject::tr("Qv2ray will now exit."));
                return false;
            }
            else if (configVersion < QV2RAY_CONFIG_VERSION)
            {
                // That is the config file needs to be upgraded.
                conf = Qv2ray::UpgradeSettingsVersion(configVersion, QV2RAY_CONFIG_VERSION, conf);
            }

            // Let's save the config.
            GlobalConfig.loadJson(conf);
            SaveGlobalSettings();
            return true;
        }
    }

} // namespace Qv2ray::core::config

using namespace Qv2ray::core::config;
