#include "Qv2rayWidgetApplication.hpp"

#include "3rdparty/libsemver/version.hpp"
#include "base/Qv2rayBase.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/common/translations/QvTranslator.hpp"
#include "ui/widgets/styles/StyleManager.hpp"
#include "ui/widgets/windows/w_MainWindow.hpp"
#include "utils/QvHelpers.hpp"

#include <QUrl>
#include <QUrlQuery>

#ifdef Q_OS_WIN
    #include <Winbase.h>
#endif

namespace Qv2ray
{
    constexpr auto QV2RAY_CONFIG_PATH_ENV_NAME = "QV2RAY_CONFIG_PATH";

    Qv2rayWidgetApplication::Qv2rayWidgetApplication(int &argc, char *argv[])
#ifdef QV2RAY_NO_SINGLEAPPLICATON
        : QApplication(argc, argv),
#else
        : SingleApplication(argc, argv, true, User | ExcludeAppPath | ExcludeAppVersion),
#endif
          Qv2rayApplicationManagerInterface(argc, argv)
    {
    }

    void Qv2rayWidgetApplication::QuitApplication(int retCode)
    {
        isExiting = true;
        QCoreApplication::exit(retCode);
    }

    Qv2raySetupStatus Qv2rayWidgetApplication::Initialize()
    {
#ifdef Q_OS_WIN
        SetCurrentDirectory(applicationDirPath().toStdWString().c_str());
#endif
        hTray = new QSystemTrayIcon();
        // Install a default translater. From the OS/DE
        Qv2rayTranslator = std::make_unique<QvTranslator>();
        Qv2rayTranslator->InstallTranslation(QLocale::system().name());
        //
        setQuitOnLastWindowClosed(false);

        SingleApplication::connect((QApplication *) this, &SingleApplication::aboutToQuit, this, &Qv2rayWidgetApplication::aboutToQuitSlot);
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        SingleApplication::connect(this, &SingleApplication::receivedMessage, this, &Qv2rayWidgetApplication::onMessageReceived,
                                   Qt::QueuedConnection);
        if (isSecondary())
        {
            if (Qv2rayProcessArgument.arguments.isEmpty())
                Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::NORMAL;
            sendMessage(JsonToString(Qv2rayProcessArgument.toJson(), QJsonDocument::Compact).toUtf8());
            return SINGLE_APPLICATION;
        }
#endif

#ifdef Q_OS_WIN
        // Set special font in Windows
        QFont font;
        font.setPointSize(9);
        font.setFamily("Microsoft YaHei");
        setFont(font);
#endif

#ifdef Q_OS_LINUX
        setFallbackSessionManagementEnabled(false);
        connect(this, &QGuiApplication::commitDataRequest, [] {
            RouteManager->SaveRoutes();
            ConnectionManager->SaveConnectionConfig();
            PluginHost->SavePluginSettings();
            SaveGlobalSettings();
            LOG(MODULE_INIT, "Saving settings triggered by session manager.")
        });
#endif
        return NORMAL;
    }

    void Qv2rayWidgetApplication::aboutToQuitSlot()
    {
        LOG(MODULE_INIT, "Terminating connections and saving data.")
        // Do not change the order.
        ConnectionManager->StopConnection();
        RouteManager->SaveRoutes();
        ConnectionManager->SaveConnectionConfig();
        PluginHost->SavePluginSettings();
        SaveGlobalSettings();
        delete mainWindow;
        delete hTray;
        delete ConnectionManager;
        delete RouteManager;
        delete PluginHost;
        delete StyleManager;
    }

#ifndef QV2RAY_NO_SINGLEAPPLICATON
    void Qv2rayWidgetApplication::onMessageReceived(quint32 clientId, QByteArray _msg)
    {
        // Sometimes SingleApplication will send message with clientId == 0, ignore them.
        if (clientId == instanceId())
            return;
        const auto msg = Qv2rayProcessArguments::fromJson(JsonFromString(_msg));
        LOG(MODULE_INIT, "Client ID: " + QSTRN(clientId) + ", message received, version: " + msg.version)
        DEBUG(MODULE_INIT, _msg)
        //
        const auto currentVersion = semver::version::from_string(QV2RAY_VERSION_STRING);
        const auto newVersionString = msg.version.isEmpty() ? "0.0.0" : msg.version.toStdString();
        const auto newVersion = semver::version::from_string(newVersionString);
        //
        if (newVersion > currentVersion)
        {
            const auto newPath = msg.fullArgs.first();
            QString message;
            message += tr("A new version of Qv2ray is attemping to start:") + NEWLINE;
            message += NEWLINE;
            message += tr("New version information: ") + NEWLINE;
            message += tr("Qv2ray version: %1").arg(msg.version) + NEWLINE;
            message += tr("Qv2ray path: %1").arg(newPath) + NEWLINE;
            message += NEWLINE;
            message += tr("Do you want to exit and launch that new version?");

            const auto result = QvMessageBoxAsk(nullptr, tr("New version detected"), message);
            if (result == Yes)
            {
                Qv2rayProcessArgument._qvNewVersionPath = newPath;
                QuitApplication(QVEXIT_NEW_VERSION);
            }
        }

        for (const auto &argument : msg.arguments)
        {
            switch (argument)
            {
                case Qv2rayProcessArguments::EXIT:
                {
                    QuitApplication();
                    break;
                }
                case Qv2rayProcessArguments::NORMAL:
                {
                    mainWindow->show();
                    mainWindow->raise();
                    mainWindow->activateWindow();
                    break;
                }
                case Qv2rayProcessArguments::RECONNECT:
                {
                    ConnectionManager->RestartConnection();
                    break;
                }
                case Qv2rayProcessArguments::DISCONNECT:
                {
                    ConnectionManager->StopConnection();
                    break;
                }
                case Qv2rayProcessArguments::QV2RAY_LINK:
                {
                    for (const auto &link : msg.links)
                    {
                        const auto url = QUrl::fromUserInput(link);
                        const auto command = url.host();
                        auto subcommands = url.path().split("/");
                        subcommands.removeAll("");
                        QMap<QString, QString> args;
                        for (const auto &kvp : QUrlQuery(url).queryItems())
                        {
                            args.insert(kvp.first, kvp.second);
                        }
                        if (command == "open")
                        {
                            emit mainWindow->ProcessCommand(command, subcommands, args);
                        }
                    }
                    break;
                }
            }
        }
    }
#endif

    Qv2rayExitCode Qv2rayWidgetApplication::RunQv2ray()
    {
        {
            StyleManager = new QvStyleManager();
            PluginHost = new QvPluginHost();
            RouteManager = new RouteHandler();
            ConnectionManager = new QvConfigHandler();
            StyleManager->ApplyStyle(GlobalConfig.uiConfig.theme);
        }
        // Show MainWindow
        mainWindow = new MainWindow();
        if (Qv2rayProcessArgument.arguments.contains(Qv2rayProcessArguments::QV2RAY_LINK))
        {
            for (const auto &link : Qv2rayProcessArgument.links)
            {
                const auto url = QUrl::fromUserInput(link);
                const auto command = url.host();
                auto subcommands = url.path().split("/");
                subcommands.removeAll("");
                QMap<QString, QString> args;
                for (const auto &kvp : QUrlQuery(url).queryItems())
                {
                    args.insert(kvp.first, kvp.second);
                }
                if (command == "open")
                {
                    emit mainWindow->ProcessCommand(command, subcommands, args);
                }
            }
        }
#ifdef Q_OS_MACOS
        connect(this, &QApplication::applicationStateChanged, [this](Qt::ApplicationState state) {
            switch (state)
            {
                case Qt::ApplicationActive:
                {
                    mainWindow->show();
                    mainWindow->raise();
                    mainWindow->activateWindow();
                    break;
                }
                case Qt::ApplicationHidden:
                case Qt::ApplicationInactive:
                case Qt::ApplicationSuspended: break;
            }
        });
#endif
        return Qv2rayExitCode(exec());
    }

    bool Qv2rayWidgetApplication::FindAndCreateInitialConfiguration()
    {
        if (initialized)
        {
            LOG(MODULE_INIT, "Qv2ray has already been initialized!")
            return false;
        }
        LOG(MODULE_INIT, "Application exec path: " + applicationDirPath())
        // Non-standard paths needs special handing for "_debug"
        const auto currentPathConfig = applicationDirPath() + "/config" QV2RAY_CONFIG_DIR_SUFFIX;
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
            LOG(MODULE_INIT, "Using config path from env: " + manualConfigPath)
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
                LOG(MODULE_INIT, "FATAL")
                LOG(MODULE_INIT, " ---> CANNOT find a proper place to store Qv2ray config files.")
                QvMessageBoxWarn(nullptr, tr("Cannot Start Qv2ray"),
                                 tr("Cannot find a place to store config files.") + NEWLINE +                                          //
                                     tr("Qv2ray has searched these paths below:") + NEWLINE + NEWLINE +                                //
                                     configFilePaths.join(NEWLINE) + NEWLINE +                                                         //
                                     tr("It usually means you don't have the write permission to all of those locations.") + NEWLINE + //
                                     tr("Qv2ray will now exit."));                                                                     //
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
                QvMessageBoxWarn(nullptr, tr("Failed to initialise Qv2ray"),
                                 tr("Failed to determine the location of config file:") + NEWLINE +                                   //
                                     tr("Qv2ray has found a config file, but it failed to be loaded due to some errors.") + NEWLINE + //
                                     tr("A workaround is to remove the this file and restart Qv2ray:") + NEWLINE +                    //
                                     QV2RAY_CONFIG_FILE + NEWLINE +                                                                   //
                                     tr("Qv2ray will now exit.") + NEWLINE +                                                          //
                                     tr("Please report if you think it's a bug."));                                                   //
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
            // The dir used to generate final config file, for V2Ray interaction.
            QDir().mkdir(QV2RAY_GENERATED_DIR);
            LOG(MODULE_INIT, "Created config generation dir at: " + QV2RAY_GENERATED_DIR)
        }
        return LoadConfiguration();
    }

    bool Qv2rayWidgetApplication::LoadConfiguration()
    {
        // Load the config for upgrade, but do not parse it to the struct.
        auto conf = JsonFromString(StringFromFile(QV2RAY_CONFIG_FILE));
        const auto configVersion = conf["config_version"].toInt();

        if (configVersion > QV2RAY_CONFIG_VERSION)
        {
            // Config version is larger than the current version...
            // This is rare but it may happen....
            QvMessageBoxWarn(nullptr, tr("Qv2ray Cannot Continue"),                                                           //
                             tr("You are running a lower version of Qv2ray compared to the current config file.") + NEWLINE + //
                                 tr("Please check if there's an issue explaining about it.") + NEWLINE +                      //
                                 tr("Or submit a new issue if you think this is an error.") + NEWLINE + NEWLINE +             //
                                 tr("Qv2ray will now exit."));
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
            LOG(MODULE_UI, "Fall back language setting to: " + osLanguage)
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

} // namespace Qv2ray
