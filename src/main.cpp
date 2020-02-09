#include <QFileInfo>
#include <QStandardPaths>
#include <QTranslator>
#include <QStyle>
#include <QLocale>
#include <QObject>
#include <QStyleFactory>
#include <QApplication>
#include <singleapplication.h>
#include <csignal>
#include "ui/w_MainWindow.hpp"
#include "common/QvHelpers.hpp"
#include "common/CommandArgs.hpp"
#include "common/QvTranslator.hpp"

#ifdef Q_OS_UNIX
// For unix root user check
#include "unistd.h"
#endif

void signalHandler(int signum)
{
    cout << "Interrupt signal (" << signum << ") received." << endl;

    if (MainWindow::mwInstance && MainWindow::mwInstance->vinstance) {
        cout << "Trying to stop connection..." << endl;
        MainWindow::mwInstance->vinstance->StopConnection();
    }

    qApp->exit(-99);
}


bool verifyConfigAvaliability(QString path, bool checkExistingConfig)
{
    // Does not exist.
    if (!QDir(path).exists()) return false;

    // A temp file used to test file permissions in that folder.
    QFile testFile(path + ".qv2ray_file_write_test_file" + QSTRN(QTime::currentTime().msecsSinceStartOfDay()));
    bool opened = testFile.open(QFile::OpenModeFlag::ReadWrite);

    if (!opened) {
        LOG(SETTINGS, "Directory at: " + path + " cannot be used as a valid config file path.")
        LOG(INIT, "---> Cannot create a new file or openwrite a file.")
        return false;
    } else {
        testFile.write("Qv2ray test file, feel free to remove.");
        testFile.flush();
        testFile.close();
        bool removed = testFile.remove();

        if (!removed) {
            // This is rare, as we can create a file but failed to remove it.
            LOG(SETTINGS, "Directory at: " + path + " cannot be used as a valid config file path.")
            LOG(INIT, "---> Cannot remove a file.")
            return false;
        }
    }

    if (checkExistingConfig) {
        // Check if an existing config is found.
        QFile configFile(path + "Qv2ray.conf");

        // No such config file.
        if (!configFile.exists()) return false;

        bool opened2 = configFile.open(QIODevice::ReadWrite);

        try {
            if (opened2) {
                // Verify if the config can be loaded.
                // Failed to parse if we changed the file structure...
                // Original:
                //  --  auto conf = StructFromJsonString<Qv2rayConfig>(configFile.readAll());
                //
                // Verify JSON file format. (only) because this file version may not be upgraded and may contain unsupported structure.
                auto err = VerifyJsonString(StringFromFile(&configFile));

                if (!err.isEmpty()) {
                    LOG(INIT, "Json parse returns: " + err)
                    return false;
                } else {
                    // If the file format is valid.
                    auto conf = JsonFromString(StringFromFile(&configFile));
                    LOG(SETTINGS, "Path: " + path + " contains a config file, in version " + conf["config_version"].toVariant().toString())
                    configFile.close();
                    return true;
                }
            } else {
                LOG(SETTINGS, "File: " + configFile.fileName()  + " cannot be opened!")
                return false;
            }
        }  catch (...) {
            LOG(SETTINGS, "Exception raised when checking config: " + configFile.fileName())
            //LOG(INIT, e->what())
            QvMessageBoxWarn(nullptr, QObject::tr("Warning"), QObject::tr("Qv2ray cannot load the config file from here:") + NEWLINE + configFile.fileName());
            return false;
        }
    } else return true;
}

bool initialiseQv2ray()
{
    LOG(INIT, "Application exec path: " + QApplication::applicationDirPath())
    const QString currentPathConfig = QApplication::applicationDirPath() + "/config" QV2RAY_CONFIG_DIR_SUFFIX;
    const QString configQv2ray = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
    const QString homeQv2ray = QDir::homePath() + "/.qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
    //
    //
    // Some built-in search paths for Qv2ray to find configs. (load the first one if possible).
    //
    QStringList configFilePaths;
    configFilePaths << currentPathConfig;
#ifdef WITH_FLATHUB_CONFIG_PATH
    // AppConfigLocation uses 'Q'v2ray instead of `q`v2ray. Keep here as backward compatibility.
    configFilePaths << QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + QV2RAY_CONFIG_DIR_SUFFIX;
#endif
    configFilePaths << configQv2ray;
    configFilePaths << homeQv2ray;
    //
    QString configPath = "";
    bool hasExistingConfig = false;

    for (auto path : configFilePaths) {
        // Verify the config path, check if the config file exists and in the correct JSON format.
        // True means we check for config existance as well. --|HERE |
        bool isValidConfigPath = verifyConfigAvaliability(path, true);

        // If we already found a valid config file. just simply load it...
        if (hasExistingConfig) break;

        if (isValidConfigPath) {
            DEBUG(INIT, "Path: " + path + " is valid.")
            configPath = path;
            hasExistingConfig = true;
        } else {
            LOG(INIT, "Path: " + path + " does not contain a valid config file.")
        }
    }

    // If there's no existing config.
    if (hasExistingConfig) {
        // Use the config path found by the checks above
        SetConfigDirPath(configPath);
        LOG(INIT, "Using " + QV2RAY_CONFIG_DIR + " as the config path.")
    } else {
        //
        // Create new config at these dirs, these are default values for each platform.
#ifdef Q_OS_WIN
        configPath = currentPathConfig;
#else
        configPath = configQv2ray;
#endif
        bool mkpathResult = QDir().mkpath(configPath);

        // Check if the dirs are write-able
        if (mkpathResult && verifyConfigAvaliability(configPath, false)) {
            // Found a valid config dir, with write permission, but assume no config is located in it.
            LOG(INIT, "Set " + configPath + " as the config path.")
            SetConfigDirPath(configPath);

            if (QFile::exists(QV2RAY_CONFIG_FILE)) {
                // As we already tried to load config from every possible dir.
                // This condition branch (!hasExistingConfig check) holds the fact that
                // current config dir, should NOT contain any valid file (at least in the same name)
                // It usually means that QV2RAY_CONFIG_FILE here is corrupted, in JSON format.
                // Otherwise Qv2ray would have loaded this config already instead of notifying to
                // create a new config in this folder.
                LOG(INIT, "This should not occur: Qv2ray config exists but failed to load.")
                QvMessageBoxWarn(nullptr, QObject::tr("Failed to initialise Qv2ray"),
                                 QObject::tr("Failed to determine the location of config file.") + NEWLINE +
                                 QObject::tr("Qv2ray will now exit.") + NEWLINE +
                                 QObject::tr("Please report if you think it's a bug."));
                return false;
            }

            Qv2rayConfig conf;
            conf.v2AssetsPath = QString(QV2RAY_DEFAULT_VASSETS_PATH);
            conf.v2CorePath = QString(QV2RAY_DEFAULT_VCORE_PATH);
            conf.logLevel = 3;
            //
            // Save initial config.
            SaveGlobalConfig(conf);
            LOG(INIT, "Created initial config file.")
        } else {
            // None of the path above can be used as a dir for storing config.
            // Even the last folder failed to pass the check.
            LOG(INIT, "FATAL")
            LOG(INIT, " ---> CANNOT find a proper place to store Qv2ray config files.")
            QString searchPath = configFilePaths.join(NEWLINE);
            QvMessageBoxWarn(nullptr, QObject::tr("Cannot Start Qv2ray"),
                             QObject::tr("Cannot find a place to store config files.") + NEWLINE +
                             QObject::tr("Qv2ray has searched these paths below:") +
                             NEWLINE + NEWLINE + searchPath + NEWLINE +
                             QObject::tr("Qv2ray will now exit."));
            return false;
        }
    }

    if (!QDir(QV2RAY_GENERATED_DIR).exists()) {
        // The dir used to generate final config file, for V2ray interaction.
        QDir().mkdir(QV2RAY_GENERATED_DIR);
        LOG(INIT, "Created config generation dir at: " + QV2RAY_GENERATED_DIR)
    }

    return true;
}


int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN
    // Register signal handlers.
    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGKILL, signalHandler);
    signal(SIGTERM, signalHandler);
#endif
    //
    // parse the command line before starting as a Qt application
    {
        std::unique_ptr<QCoreApplication> consoleApp(new QCoreApplication(argc, argv));
        //
        // Install a default translater. From the OS/DE
        consoleApp->installTranslator(QvTranslator(QLocale::system().name()).pTranslator.get());
        QvCommandArgParser parser;
        QString errorMessage;

        switch (parser.ParseCommandLine(&errorMessage)) {
            case CommandLineOk:
                break;

            case CommandLineError:
                cout << errorMessage.toStdString() << endl;
                cout << parser.Parser()->helpText().toStdString() << endl;
                return 1;

            case CommandLineVersionRequested:
                LOG("Qv2ray", QV2RAY_VERSION_STRING)
                LOG("QV2RAY_BUILD_INFO", QV2RAY_BUILD_INFO)
                LOG("QV2RAY_BUILD_EXTRA_INFO", QV2RAY_BUILD_EXTRA_INFO)
                return 0;

            case CommandLineHelpRequested:
                cout << parser.Parser()->helpText().toStdString() << endl;
                return 0;
        }
    }
#ifdef Q_OS_UNIX

    // Unix OS root user check.
    // Do not use getuid() here since it's installed as owned by the root, someone may accidently setuid to it.
    if (!StartupOption.forceRunAsRootUser && geteuid() == 0) {
        LOG("ERROR", QObject::tr("You cannot run Qv2ray as root, please use --I-just-wanna-run-with-root if you REALLY want to do so."))
        LOG("ERROR", QObject::tr(" --> USE IT AT YOUR OWN RISK!"))
        return 1;
    }

#endif
    //
    // finished: command line parsing
    LOG("QV2RAY_BUILD_INFO", QV2RAY_BUILD_INFO)
    LOG("QV2RAY_BUILD_EXTRA_INFO", QV2RAY_BUILD_EXTRA_INFO)
    LOG(INIT, "Qv2ray " QV2RAY_VERSION_STRING " running on " + QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture() + NEWLINE)
    //
    // This line must be called before any other ones, since we are using these values to identify instances.
    SingleApplication::setApplicationName("Qv2ray");
    SingleApplication::setApplicationVersion(QV2RAY_VERSION_STRING);
    SingleApplication::setApplicationDisplayName("Qv2ray");
    //
    //
#ifdef QT_DEBUG
    // ----------------------------> For debug build...
    SingleApplication::setApplicationName("Qv2ray - DEBUG");
#endif
    SingleApplication _qApp(argc, argv, false, SingleApplication::Mode::User | SingleApplication::Mode::ExcludeAppPath | SingleApplication::Mode::ExcludeAppVersion);
    _qApp.setQuitOnLastWindowClosed(false);
    // Early initialisation
    //
    // Not duplicated.
    // Install a default translater. From the OS/DE
    auto _lang = QLocale::system().name();
    Qv2rayTranslator = std::move(QvTranslator(_lang).pTranslator);
    //
    // Do not install en-US as it's the default language.
    bool _result_ = _qApp.installTranslator(Qv2rayTranslator.get());
    LOG(UI, "Installing a tranlator from OS: " + _lang + " -- " + (_result_ ? "OK" : "Failed"))
    //
    LOG("LICENCE", NEWLINE "This program comes with ABSOLUTELY NO WARRANTY." NEWLINE
        "This is free software, and you are welcome to redistribute it" NEWLINE
        "under certain conditions." NEWLINE NEWLINE
        "Copyright (c) 2019-2020 Qv2ray Development Group." NEWLINE
        NEWLINE NEWLINE
        "Libraries that have been used in Qv2ray are listed below (Sorted by date added):" NEWLINE
        "Copyright (c) 2020 dridk (@dridk): X2Struct (Apache)" NEWLINE
        "Copyright (c) 2011 SCHUTZ Sacha (@dridk): QJsonModel (MIT)" NEWLINE
        "Copyright (c) 2020 Nikolaos Ftylitakis (@ftylitak): QZXing (Apache2)" NEWLINE
        "Copyright (c) 2016 Singein (@Singein): ScreenShot (MIT)" NEWLINE
        "Copyright (c) 2016 Nikhil Marathe (@nikhilm): QHttpServer (MIT)" NEWLINE
        "Copyright (c) 2020 Itay Grudev (@itay-grudev): SingleApplication (MIT)" NEWLINE
        "Copyright (c) 2020 paceholder (@paceholder): nodeeditor (QNodeEditor modified by lhy0403) (BSD-3-Clause)" NEWLINE
        "Copyright (c) 2019 TheWanderingCoel (@TheWanderingCoel): ShadowClash (launchatlogin) (GPLv3)" NEWLINE
        "Copyright (c) 2020 Ram Pani (@DuckSoft): QvRPCBridge (WTFPL)" NEWLINE
        "Copyright (c) 2019 ShadowSocks (@shadowsocks): libQtShadowsocks (LGPLv3)" NEWLINE
        NEWLINE)
    //
    LOG(INIT, "Qv2ray Start Time: "  + QSTRN(QTime::currentTime().msecsSinceStartOfDay()))
    //
#ifdef QT_DEBUG
    cout << "WARNING: ============================== This is a debug build, many features are not stable enough. ==============================" << endl;
#endif
    //
    // Load the language translation list.
    auto langs = GetFileList(QDir(":/translations"));

    if (langs.empty()) {
        LOG(INIT, "FAILED to find any translations. THIS IS A BUILD ERROR.")
        QvMessageBoxWarn(nullptr, QObject::tr("Cannot load languages"), QObject::tr("Qv2ray will continue running, but you cannot change the UI language."));
    } else {
        for (auto lang : langs) {
            LOG(INIT, "Found Translator: " + lang)
        }
    }

    // Qv2ray Initialize, find possible config paths and verify them.
    if (!initialiseQv2ray()) {
        return -1;
    }

    // Load the config for upgrade, but do not parse it to the struct.
    auto conf = JsonFromString(StringFromFile(QV2RAY_CONFIG_FILE));
    auto confVersion = conf["config_version"].toVariant().toString().toInt();

    if (confVersion > QV2RAY_CONFIG_VERSION) {
        // Config version is larger than the current version...
        // This is rare but it may happen....
        QvMessageBoxWarn(nullptr, QObject::tr("Qv2ray Cannot Continue"),
                         QObject::tr("You are running a lower version of Qv2ray compared to the current config file.") + NEWLINE +
                         QObject::tr("Please check if there's an issue explaining about it.") + NEWLINE +
                         QObject::tr("Or submit a new issue if you think this is an error.") + NEWLINE + NEWLINE +
                         QObject::tr("Qv2ray will now exit."));
        return -2;
    }

    if (confVersion < QV2RAY_CONFIG_VERSION) {
        // That is, config file needs to be upgraded.
        conf = Qv2ray::UpgradeConfig(confVersion, QV2RAY_CONFIG_VERSION, conf);
    }

    // Load config object from upgraded config QJsonObject
    auto confObject = StructFromJsonString<Qv2rayConfig>(JsonToString(conf));
    // Remove system translator, for loading custom translations.
    qApp->removeTranslator(Qv2rayTranslator.get());
    LOG(INIT, "Removed system translations")

    if (confObject.uiConfig.language.isEmpty()) {
        // Prevent empty.
        LOG(UI, "Setting default UI language to en-US")
        confObject.uiConfig.language = "en-US";
    }

    Qv2rayTranslator = std::move(QvTranslator(confObject.uiConfig.language).pTranslator);

    if (qApp->installTranslator(Qv2rayTranslator.get())) {
        LOG(INIT, "Successfully installed a translator for " + confObject.uiConfig.language)
    } else {
        // Do not translate these.....
        // If a translator fails to load, pop up a message.
        QvMessageBoxWarn(
            nullptr, "Translation Failed",
            "Cannot load translation for " + confObject.uiConfig.language + ", English is now used." + NEWLINE + NEWLINE +
            "Please go to Preferences Window to change language or open an Issue");
    }

    // Let's save the config.
    SaveGlobalConfig(confObject);
    //
    // Check OpenSSL version for auto-update and subscriptions
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString();
    LOG(NETWORK, "Current OpenSSL version: " + osslCurVersion)

    if (!QSslSocket::supportsSsl()) {
        LOG(NETWORK, "Required OpenSSL version: " + osslReqVersion)
        LOG(NETWORK, "OpenSSL library MISSING, Quitting.")
        QvMessageBoxWarn(nullptr, QObject::tr("Dependency Missing"),
                         QObject::tr("Cannot find openssl libs") + NEWLINE +
                         QObject::tr("This could be caused by a missing of `openssl` package in your system.") + NEWLINE +
                         QObject::tr("If you are using an AppImage from Github Action, please report a bug.") + NEWLINE + NEWLINE +
                         QObject::tr("Technical Details") + NEWLINE +
                         "OSsl.Rq.V=" + osslReqVersion + NEWLINE +
                         "OSsl.Cr.V=" + osslCurVersion);
        return -3;
    }

#ifdef Q_OS_WIN
    // Set special font in Windows
    QFont font;
    font.setPointSize(9);
    font.setFamily("Microsoft YaHei");
    _qApp.setFont(font);
#endif
#ifdef QV2RAY_USE_BUILTIN_DARKTHEME
    LOG(UI, "Using built-in theme.")

    if (confObject.uiConfig.useDarkTheme) {
        LOG(UI, " --> Using built-in dark theme.")
        // From https://forum.qt.io/topic/101391/windows-10-dark-theme/4
        _qApp.setStyle("Fusion");
        QPalette darkPalette;
        QColor darkColor = QColor(45, 45, 45);
        QColor disabledColor = QColor(127, 127, 127);
        // See Qv2rayBase.hpp MACRO --> BLACK(obj)
        QColor defaultTextColor = QColor(210, 210, 210);
        darkPalette.setColor(QPalette::Window, darkColor);
        darkPalette.setColor(QPalette::WindowText, defaultTextColor);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, disabledColor);
        darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
        darkPalette.setColor(QPalette::AlternateBase, darkColor);
        darkPalette.setColor(QPalette::ToolTipBase, defaultTextColor);
        darkPalette.setColor(QPalette::ToolTipText, defaultTextColor);
        darkPalette.setColor(QPalette::Text, defaultTextColor);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor(QPalette::Button, darkColor);
        darkPalette.setColor(QPalette::ButtonText, defaultTextColor);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);
        qApp->setPalette(darkPalette);
        qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    }

#else
    // Set custom themes.
    QStringList themes = QStyleFactory::keys();
    //_qApp.setDesktopFileName("qv2ray.desktop");

    if (themes.contains(confObject.uiConfig.theme)) {
        _qApp.setStyle(confObject.uiConfig.theme);
        LOG(INIT + " " + UI, "Setting Qv2ray UI themes: " + confObject.uiConfig.theme)
    }

#endif
    // Show MainWindow
    MainWindow w;
#ifndef QT_DEBUG

    try {
#endif
        QObject::connect(&_qApp, &SingleApplication::instanceStarted, [&w]() {
            // When a second instance is connected, show the mainwindow.
            w.show();
            w.raise();
            w.activateWindow();
        });
        // Handler for session logout, shutdown, etc.
        // Will not block.
        QGuiApplication::setFallbackSessionManagementEnabled(false);
        QObject::connect(&_qApp, &QGuiApplication::commitDataRequest, []() {
            LOG(INIT, "Quit triggered by session manager.")
        });
#ifndef Q_OS_WIN
        signal(SIGUSR1, [](int) {
            emit MainWindow::mwInstance->Connect();
        });
        signal(SIGUSR2, [](int) {
            emit MainWindow::mwInstance->DisConnect();
        });
#endif
        auto rcode = _qApp.exec();
        LOG(INIT, "Quitting normally")
        return rcode;
#ifndef QT_DEBUG
    }  catch (...) {
        QvMessageBoxWarn(nullptr, "ERROR", "There's something wrong happened and Qv2ray will quit now.");
        LOG(INIT, "EXCEPTION THROWN: " __FILE__)
        return -99;
    }

#endif
}
