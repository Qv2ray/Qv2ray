#include "Qv2rayBaseApplication.hpp"

#include "components/translations/QvTranslator.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"

#ifdef QT_DEBUG
const static inline QString QV2RAY_URL_SCHEME = "qv2ray-debug";
#else
const static inline QString QV2RAY_URL_SCHEME = "qv2ray";
#endif

#define QV_MODULE_NAME "BaseApplication"

constexpr auto QV2RAY_CONFIG_PATH_ENV_NAME = "QV2RAY_CONFIG_PATH";

Qv2rayApplicationManager::Qv2rayApplicationManager()
{
    qvApplicationInstance = this;
    LOG("Qv2ray " QV2RAY_VERSION_STRING " on " + QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture());
    DEBUG("Qv2ray Start Time: " + QSTRN(QTime::currentTime().msecsSinceStartOfDay()));
    DEBUG(std::string{ "QV2RAY_BUILD_INFO" }, QV2RAY_BUILD_INFO);
    DEBUG(std::string{ "QV2RAY_BUILD_EXTRA_INFO" }, QV2RAY_BUILD_EXTRA_INFO);
    DEBUG(std::string{ "QV2RAY_BUILD_NUMBER" }, QSTRN(QV2RAY_VERSION_BUILD));
}

Qv2rayApplicationManager::~Qv2rayApplicationManager()
{
    qvApplicationInstance = nullptr;
}

bool Qv2rayApplicationManager::FindAndCreateInitialConfiguration()
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
    QStringList configFilePaths;
    const auto useManualConfigPath = qEnvironmentVariableIsSet(QV2RAY_CONFIG_PATH_ENV_NAME);
    const auto manualConfigPath = qEnvironmentVariable(QV2RAY_CONFIG_PATH_ENV_NAME);
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

        // Load config object from upgraded config QJsonObject
        auto confObject = Qv2rayConfigObject::fromJson(conf);

        const auto allTranslations = Qv2rayTranslator->GetAvailableLanguages();
        const auto osLanguage = QLocale::system().name();

        if (!allTranslations.contains(confObject.uiConfig.language))
        {
            // If we need to reset the language.
            if (allTranslations.contains(osLanguage))
            {
                confObject.uiConfig.language = osLanguage;
            }
            else if (!allTranslations.isEmpty())
            {
                confObject.uiConfig.language = allTranslations.first();
            }
            // If configured language is not found.
            LOG("Fall back language setting to: " + osLanguage);
        }

        if (!Qv2rayTranslator->InstallTranslation(confObject.uiConfig.language))
        {
            QvMessageBoxWarn(nullptr, "Translation Failed",
                             "Cannot load translation for " + confObject.uiConfig.language + NEWLINE + //
                                 "English is now used." + NEWLINE + NEWLINE +                          //
                                 "Please go to Preferences Window to change language or open an Issue");
            confObject.uiConfig.language = "en_US";
        }

        // Let's save the config.
        SaveGlobalSettings(confObject);
        return true;
    }
}

Qv2rayPreInitResult Qv2rayApplicationManager::PreInitialize(int argc, char **argv)
{
    QString errorMessage;
    QCoreApplication coreApp(argc, argv);
    const auto &args = coreApp.arguments();
    Qv2rayProcessArgument.version = QV2RAY_VERSION_STRING;
    Qv2rayProcessArgument.fullArgs = args;
    auto result = ParseCommandLine(&errorMessage, args);
    LOG("Qv2ray PreInitialization: " + errorMessage);
    if (result != PRE_INIT_RESULT_CONTINUE)
        return result;
#ifdef Q_OS_WIN
    const auto appPath = QDir::toNativeSeparators(coreApp.applicationFilePath());
    const auto regPath = "HKEY_CURRENT_USER\\Software\\Classes\\" + QV2RAY_URL_SCHEME;

    QSettings reg(regPath, QSettings::NativeFormat);

    reg.setValue("Default", "Qv2ray");
    reg.setValue("URL Protocol", "");

    reg.beginGroup("DefaultIcon");
    reg.setValue("Default", QString("%1,1").arg(appPath));
    reg.endGroup();

    reg.beginGroup("shell");
    reg.beginGroup("open");
    reg.beginGroup("command");
    reg.setValue("Default", appPath + " %1");
#endif
    return result;
}
Qv2rayPreInitResult Qv2rayApplicationManager::ParseCommandLine(QString *errorMessage, const QStringList &_argx_)
{
    QStringList filteredArgs;
    for (const auto &arg : _argx_)
    {
#ifdef Q_OS_MACOS
        if (arg.contains("-psn"))
            continue;
#endif
        filteredArgs << arg;
    }
    QCommandLineParser parser;
    //
    QCommandLineOption noAPIOption("noAPI", QObject::tr("Disable gRPC API subsystem"));
    QCommandLineOption noPluginsOption("noPlugin", QObject::tr("Disable plugins feature"));
    QCommandLineOption noScaleFactorOption("noScaleFactor", QObject::tr("Disable Qt UI scale factor"));
    QCommandLineOption debugOption("debug", QObject::tr("Enable debug output"));
    QCommandLineOption noAutoConnectionOption("noAutoConnection", QObject::tr("Do not automatically connect"));
    QCommandLineOption disconnectOption("disconnect", QObject::tr("Stop current connection"));
    QCommandLineOption reconnectOption("reconnect", QObject::tr("Reconnect last connection"));
    QCommandLineOption exitOption("exit", QObject::tr("Exit Qv2ray"));
    //
    parser.setApplicationDescription(QObject::tr("Qv2ray - A cross-platform Qt frontend for V2Ray."));
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    //
    parser.addOption(noAPIOption);
    parser.addOption(noPluginsOption);
    parser.addOption(noScaleFactorOption);
    parser.addOption(debugOption);
    parser.addOption(noAutoConnectionOption);
    parser.addOption(disconnectOption);
    parser.addOption(reconnectOption);
    parser.addOption(exitOption);
    //
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();

    if (!parser.parse(filteredArgs))
    {
        *errorMessage = parser.errorText();
        return PRE_INIT_RESULT_CONTINUE;
    }

    if (parser.isSet(versionOption))
    {
        parser.showVersion();
        return PRE_INIT_RESULT_QUIT;
    }

    if (parser.isSet(helpOption))
    {
        parser.showHelp();
        return PRE_INIT_RESULT_QUIT;
    }

    for (const auto &arg : parser.positionalArguments())
    {
        if (arg.startsWith(QV2RAY_URL_SCHEME + "://"))
        {
            Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::QV2RAY_LINK;
            Qv2rayProcessArgument.links << arg;
        }
    }

    if (parser.isSet(exitOption))
    {
        DEBUG("disconnectOption is set.");
        Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::EXIT;
    }

    if (parser.isSet(disconnectOption))
    {
        DEBUG("disconnectOption is set.");
        Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::DISCONNECT;
    }

    if (parser.isSet(reconnectOption))
    {
        DEBUG("reconnectOption is set.");
        Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::RECONNECT;
    }

    if (parser.isSet(noAPIOption))
    {
        DEBUG("noAPIOption is set.");
        StartupOption.noAPI = true;
    }

    if (parser.isSet(debugOption))
    {
        DEBUG("debugOption is set.");
        StartupOption.debugLog = true;
    }

    if (parser.isSet(noScaleFactorOption))
    {
        DEBUG("noScaleFactorOption is set.");
        StartupOption.noScaleFactor = true;
    }

    if (parser.isSet(noAutoConnectionOption))
    {
        DEBUG("noAutoConnectOption is set.");
        StartupOption.noAutoConnection = true;
    }

    if (parser.isSet(noPluginsOption))
    {
        DEBUG("noPluginOption is set.");
        StartupOption.noPlugins = true;
    }
    *errorMessage = "OK";
    return PRE_INIT_RESULT_CONTINUE;
}
