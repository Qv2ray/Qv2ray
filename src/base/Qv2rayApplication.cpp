#include "Qv2rayApplication.hpp"

#include "Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"
#include "core/settings/SettingsBackend.hpp"

namespace Qv2ray
{
    Qv2rayApplication::Qv2rayApplication(int &argc, char *argv[])
        : SingleApplication(argc, argv, true, User | ExcludeAppPath | ExcludeAppVersion)
    {
        LOG("QV2RAY_BUILD_INFO", QV2RAY_BUILD_INFO)
        LOG("QV2RAY_BUILD_EXTRA_INFO", QV2RAY_BUILD_EXTRA_INFO)
        LOG("QV2RAY_BUILD_NUMBER", QSTRN(QV2RAY_VERSION_BUILD))
        LOG(MODULE_INIT, "Qv2ray " QV2RAY_VERSION_STRING " on " + QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture())
        LOG(MODULE_INIT, "Qv2ray Start Time: " + QSTRN(QTime::currentTime().msecsSinceStartOfDay()))
    }

    bool Qv2rayApplication::SecondaryInstanceCheck()
    {
        if (isSecondary())
        {
            sendMessage(arguments().join(' ').toUtf8());
            QEventLoop loop;
            ExitQv2ray();
            return true;
        }
        return false;
    }

    void Qv2rayApplication::SetHiDPIEnableState(bool enabled)
    {
        if (enabled)
        {
            LOG(MODULE_INIT, "High DPI scaling is enabled.")
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
        }
        else
        {
            LOG(MODULE_INIT, "Force set QT_SCALE_FACTOR to 1.")
            LOG(MODULE_UI, "Original QT_SCALE_FACTOR was: " + qEnvironmentVariable("QT_SCALE_FACTOR"))
            qputenv("QT_SCALE_FACTOR", "1");
        }
    }

    bool Qv2rayApplication::Initilize()
    {
        if (initilized)
        {
            LOG(MODULE_INIT, "Qv2ray has already been initilized!")
            return false;
        }
        LOG(MODULE_INIT, "Application exec path: " + applicationDirPath())
        // Non-standard paths needs special handing for "_debug"
        const auto currentPathConfig = applicationDirPath() + "/config" QV2RAY_CONFIG_DIR_SUFFIX;
        const auto homeQv2ray = QDir::homePath() + "/.qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
        // Standard paths already handles the "_debug" suffix for us.
        const auto configQv2ray = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        //
        //
        // Some built-in search paths for Qv2ray to find configs. (load the first one if possible).
        //
        QStringList configFilePaths;
        configFilePaths << currentPathConfig;
        configFilePaths << configQv2ray;
        configFilePaths << homeQv2ray;
        //
        QString configPath = "";
        bool hasExistingConfig = false;

        for (const auto &path : configFilePaths)
        {
            // Verify the config path, check if the config file exists and in the
            // correct JSON format. True means we check for config existence as
            // well. ----------------------------------------------|HERE|
            bool isValidConfigPath = CheckSettingsPathAvailability(path, true);

            // If we already found a valid config file. just simply load it...
            if (hasExistingConfig)
                break;

            if (isValidConfigPath)
            {
                DEBUG(MODULE_INIT, "Path: " + path + " is valid.")
                configPath = path;
                hasExistingConfig = true;
            }
            else
            {
                LOG(MODULE_INIT, "Path: " + path + " does not contain a valid config file.")
            }
        }

        if (hasExistingConfig)
        {
            // Use the config path found by the checks above
            SetConfigDirPath(configPath);
            LOG(MODULE_INIT, "Using " + QV2RAY_CONFIG_DIR + " as the config path.")
        }
        else
        {
            // If there's no existing config.
            //
            // Create new config at these dirs, these are default values for each
            // platform.
#if defined(Q_OS_WIN) && !defined(QV2RAY_NO_ASIDECONFIG)
            configPath = currentPathConfig;
#else
            configPath = configQv2ray;
#endif
            bool mkpathResult = QDir().mkpath(configPath);
            bool hasPossibleNewLocation = mkpathResult && CheckSettingsPathAvailability(configPath, false);
            // Check if the dirs are write-able
            if (!hasPossibleNewLocation)
            {
                //
                // None of the path above can be used as a dir for storing config.
                // Even the last folder failed to pass the check.
                LOG(MODULE_INIT, "FATAL")
                LOG(MODULE_INIT, " ---> CANNOT find a proper place to store Qv2ray config files.")
                QvMessageBoxWarn(nullptr, QObject::tr("Cannot Start Qv2ray"),
                                 QObject::tr("Cannot find a place to store config files.") + NEWLINE +
                                     QObject::tr("Qv2ray has searched these paths below:") + NEWLINE + NEWLINE + //
                                     configFilePaths.join(NEWLINE) + NEWLINE +
                                     QObject::tr("It usually means you don't have the write permission to all of those locations.") +
                                     QObject::tr("Qv2ray will now exit."));
                return false;
            }
            // Found a valid config dir, with write permission, but assume no config is located in it.
            LOG(MODULE_INIT, "Set " + configPath + " as the config path.")
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
                LOG(MODULE_INIT, "This should not occur: Qv2ray config exists but failed to load.")
                QvMessageBoxWarn(nullptr, QObject::tr("Failed to initialise Qv2ray"),
                                 QObject::tr("Failed to determine the location of config file:") + NEWLINE +                                   //
                                     QObject::tr("Qv2ray has found a config file, but it failed to be loaded due to some errors.") + NEWLINE + //
                                     QObject::tr("A workaround is to remove the this file and restart Qv2ray:") + NEWLINE +                    //
                                     QV2RAY_CONFIG_FILE + NEWLINE +                                                                            //
                                     QObject::tr("Qv2ray will now exit.") + NEWLINE +                                                          //
                                     QObject::tr("Please report if you think it's a bug."));                                                   //
                return false;
            }

            Qv2rayConfigObject conf;
            conf.kernelConfig.KernelPath(QString(QV2RAY_DEFAULT_VCORE_PATH));
            conf.kernelConfig.AssetsPath(QString(QV2RAY_DEFAULT_VASSETS_PATH));
            conf.logLevel = 3;
            conf.uiConfig.language = QLocale::system().name();
            conf.defaultRouteConfig.dnsConfig.servers << QvConfig_DNS::DNSServerObject{ "1.1.1.1" } //
                                                      << QvConfig_DNS::DNSServerObject{ "8.8.8.8" } //
                                                      << QvConfig_DNS::DNSServerObject{ "8.8.4.4" };

            // Save initial config.
            SaveGlobalSettings(conf);
            LOG(MODULE_INIT, "Created initial config file.")
        }

        if (!QDir(QV2RAY_GENERATED_DIR).exists())
        {
            // The dir used to generate final config file, for V2ray interaction.
            QDir().mkdir(QV2RAY_GENERATED_DIR);
            LOG(MODULE_INIT, "Created config generation dir at: " + QV2RAY_GENERATED_DIR)
        }

        return true;
    }

    bool Qv2rayApplication::CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig)
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
                    auto err = VerifyJsonString(StringFromFile(configFile));

                    if (!err.isEmpty())
                    {
                        LOG(MODULE_INIT, "Json parse returns: " + err)
                        return false;
                    }
                    else
                    {
                        // If the file format is valid.
                        auto conf = JsonFromString(StringFromFile(configFile));
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
        {
            return true;
        }
    }
} // namespace Qv2ray
