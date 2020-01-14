#include <QFileInfo>
#include <QStandardPaths>
#include <QTranslator>
#include <QStyle>
#include <QLocale>
#include <QObject>
#include <QStyleFactory>
#include <QApplication>
#include <singleapplication.h>
#include "w_MainWindow.hpp"
#include "QvCore/QvCommandLineArgs.hpp"

#ifdef Q_OS_UNIX
// For unix root user check
#include "unistd.h"
#endif

bool verifyConfigAvaliability(QString path, bool checkExistingConfig)
{
    // Does not exist.
    if (!QDir(path).exists()) return false;

    // A temp file used to test file permissions in that folder.
    QFile testFile(path + ".qv2ray_file_write_test_file" + QSTRN(QTime::currentTime().msecsSinceStartOfDay()));
    bool opened = testFile.open(QFile::OpenModeFlag::ReadWrite);

    if (!opened) {
        LOG(MODULE_CONFIG, "Directory at: " + path + " cannot be used as a valid config file path.")
        LOG(MODULE_INIT, "---> Cannot create a new file or openwrite a file.")
        return false;
    } else {
        testFile.write("qv2ray test file, feel free to remove.");
        testFile.flush();
        testFile.close();
        bool removed = testFile.remove();

        if (!removed) {
            // This is rare, as we can create a file but failed to remove it.
            LOG(MODULE_CONFIG, "Directory at: " + path + " cannot be used as a valid config file path.")
            LOG(MODULE_INIT, "---> Cannot remove a file.")
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
                    LOG(MODULE_INIT, "Json parse returns: " + err)
                    return false;
                } else {
                    // If the file format is valid.
                    auto conf = JsonFromString(StringFromFile(&configFile));
                    LOG(MODULE_CONFIG, "Path: " + path + " contains a config file, in version " + conf["config_version"].toVariant().toString())
                    configFile.close();
                    return true;
                }
            } else {
                LOG(MODULE_CONFIG, "File: " + configFile.fileName()  + " cannot be opened!")
                return false;
            }
        }  catch (...) {
            LOG(MODULE_CONFIG, "Exception raised when checking config: " + configFile.fileName())
            //LOG(MODULE_INIT, e->what())
            QvMessageBox(nullptr, QObject::tr("Warning"), QObject::tr("Qv2ray cannot load the config file from here:") + NEWLINE + configFile.fileName());
            return false;
        }
    } else return true;
}

bool initialiseQv2ray()
{
    // Some built-in search paths for Qv2ray to find configs. Reversed Priority (load the bottom one if possible).
    QStringList configFilePaths;
    configFilePaths << QDir::homePath() + "/.qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
    configFilePaths << QDir::homePath() + "/.config/qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
    configFilePaths << QDir::currentPath() + "/config" QV2RAY_CONFIG_DIR_SUFFIX;
    QString configPath = "";
    //
    bool hasExistingConfig = false;

    for (auto path : configFilePaths) {
        // Verify the config path, check if the config file exists and in the correct JSON format.
        // True means we check for config existance as well. --|     |
        bool isValidConfigPath = verifyConfigAvaliability(path, true);

        if (isValidConfigPath) {
            DEBUG(MODULE_INIT, "Path: " + path + " is valid.")
            configPath = path;
            hasExistingConfig = true;
        } else {
            DEBUG(MODULE_INIT, "Path: " + path + " does not contain a valid config file.")
        }
    }

    // If there's no existing config.
    if (hasExistingConfig) {
        // Use the config path found by the checks above
        SetConfigDirPath(&configPath);
        LOG(MODULE_INIT, "Using " + QV2RAY_CONFIG_DIR + " as the config path.")
    } else {
        // Create new config at these dirs, these are default values for each platform.
#ifdef Q_OS_WIN
        configPath = QDir::currentPath() + "/config" QV2RAY_CONFIG_DIR_SUFFIX;
#elif defined (Q_OS_LINUX)
        configPath = QDir::homePath() + "/.config/qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
#elif defined (__APPLE__)
        configPath = QDir::homePath() + "/.qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
#else
        LOG(MODULE_INIT, "CANNOT CONTINUE because Qv2ray cannot determine the OS type.")
        static_assert(false, "Qv2ray Cannot understand the enviornment");
#endif
        bool mkpathResult = QDir().mkpath(configPath);

        // Check if the dirs are write-able
        if (mkpathResult && verifyConfigAvaliability(configPath, false)) {
            // Found a valid config dir, with write permission, but assume no config is located in it.
            LOG(MODULE_INIT, "Set " + configPath + " as the config path.")
            SetConfigDirPath(&configPath);

            if (QFile::exists(QV2RAY_CONFIG_FILE)) {
                // As we already tried to load config from every possible dir.
                // This condition branch (!hasExistingConfig check) holds the fact that
                // current config dir, should NOT contain any valid file (at least in the same name)
                // It usually means that QV2RAY_CONFIG_FILE here is corrupted, in JSON format.
                // Otherwise Qv2ray would have loaded this config already instead of notifying to
                // create a new config in this folder.
                LOG(MODULE_INIT, "This should not occur: Qv2ray config exists but failed to load.")
                QvMessageBox(nullptr, QObject::tr("Failed to initialise Qv2ray"),
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
            SetGlobalConfig(conf);
            LOG(MODULE_INIT, "Created initial config file.")
        } else {
            // None of the path above can be used as a dir for storing config.
            // Even the last folder failed to pass the check.
            LOG(MODULE_INIT, "FATAL")
            LOG(MODULE_INIT, " ---> CANNOT find a proper place to store Qv2ray config files.")
            QString searchPath = Stringify(configFilePaths, NEWLINE);
            QvMessageBox(nullptr, QObject::tr("Cannot Start Qv2ray"),
                         QObject::tr("Cannot find a place to store config files.") + NEWLINE +
                         QObject::tr("Qv2ray has searched these paths below:") +
                         NEWLINE + NEWLINE + searchPath + NEWLINE +
                         QObject::tr("Qv2ray will now exit."));
            return false;
        }
    }

    if (!QDir(QV2RAY_GENERATED_DIR).exists()) {
        // The dir used to generate final config file, for v2ray interaction.
        QDir().mkdir(QV2RAY_GENERATED_DIR);
        LOG(MODULE_INIT, "Created config generation dir at: " + QV2RAY_GENERATED_DIR)
    }

    return true;
}


int main(int argc, char *argv[])
{
    // parse the command line before starting as a Qt application
    {
        std::unique_ptr<QCoreApplication> consoleApp(new QCoreApplication(argc, argv));
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
                LOG("Qv2ray", QV2RAY_VERSION_STRING);
                return 0;

            case CommandLineHelpRequested:
                cout << parser.Parser()->helpText().toStdString() << endl;
                return 0;
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
    }
    //
    // finished: command line parsing
    LOG(MODULE_INIT, "Qv2ray " QV2RAY_VERSION_STRING " running on " + QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture() + NEWLINE)
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
    //
    SingleApplication _qApp(argc, argv, false, SingleApplication::Mode::User | SingleApplication::Mode::ExcludeAppPath | SingleApplication::Mode::ExcludeAppVersion);
    // Early initialisation
    //
    //
    // Install a default translater. From the OS/DE
    auto _lang = QLocale::system().name().replace("_", "-");
    auto _sysTranslator = getTranslator(_lang);

    if (_lang != "en-US") {
        // Do not install en-US as it's the default language.
        bool _result_ = qApp->installTranslator(_sysTranslator);
        LOG(MODULE_UI, "Installing a tranlator from OS: " + _lang + " -- " + (_result_ ? "OK" : "Failed"))
    }

    LOG("LICENCE", NEWLINE "This program comes with ABSOLUTELY NO WARRANTY." NEWLINE
        "This is free software, and you are welcome to redistribute it" NEWLINE
        "under certain conditions." NEWLINE NEWLINE
        "Copyright (C) 2020 Leroy.H.Y (@lhy0403): Qv2ray Current Developer" NEWLINE
        "Copyright (C) 2019 Hork (@aliyuchang33): Hv2ray Initial Designs & gRPC implementation " NEWLINE
        "Copyright (C) 2019 SOneWinstone (@SoneWinstone): Hv2ray/Qv2ray HTTP Request Helper" NEWLINE
        "Qv2ray ArtWork Done By ArielAxionL (@axionl)" NEWLINE
        "TheBadGateway (@thebadgateway): Qv2ray Russian Translations" NEWLINE
        "Riko (@rikakomoe): Qv2ray patch 8a8c1a/PR115"
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
        NEWLINE)
    //
    LOG(MODULE_INIT, "Qv2ray Start Time: "  + QSTRN(QTime::currentTime().msecsSinceStartOfDay()))
    DEBUG("DEBUG", "WARNING: ============================== This is a debug build, many features are not stable enough. ==============================")
    //
    // Load the language translation list.
    auto langs = GetFileList(QDir(":/translations"));

    if (langs.empty()) {
        LOG(MODULE_INIT, "FAILED to find any translations. THIS IS A BUILD ERROR.")
        QvMessageBox(nullptr, QObject::tr("Cannot load languages"), QObject::tr("Qv2ray will continue running, but you cannot change the UI language."));
    } else {
        for (auto lang : langs) {
            LOG(MODULE_INIT, "Found Translator: " + lang)
        }
    }

    // Qv2ray Initialize, find possible config paths and verify them.
    if (!initialiseQv2ray()) {
        return -1;
    }

    // Load the config for upgrade, but do not parse it to the struct.
    auto conf = JsonFromString(StringFromFile(new QFile(QV2RAY_CONFIG_FILE)));
    auto confVersion = conf["config_version"].toVariant().toString().toInt();

    if (confVersion > QV2RAY_CONFIG_VERSION) {
        // Config version is larger than the current version...
        // This is rare but it may happen....
        QvMessageBox(nullptr, QObject::tr("Qv2ray Cannot Continue"),
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
    qApp->removeTranslator(_sysTranslator);
    LOG(MODULE_INIT, "Removing system translations")

    if (confObject.uiConfig.language.isEmpty()) {
        // Prevent empty.
        LOG(MODULE_UI, "Setting default UI language to en-US")
        confObject.uiConfig.language = "en-US";
    }

    if (qApp->installTranslator(getTranslator(confObject.uiConfig.language))) {
        LOG(MODULE_INIT, "Successfully installed a translator for " + confObject.uiConfig.language)
    } else {
        // Do not translate these.....
        // If a translator fails to load, pop up a message.
        QvMessageBox(
            nullptr, "Translation Failed",
            "Cannot load translation for " + confObject.uiConfig.language + ", English is now used.\r\n\r\n"
            "Please go to Preferences Window to change or Report a Bug at: \r\n"
            "https://github.com/lhy0403/Qv2ray/issues/new");
    }

    // Let's save the config.
    SetGlobalConfig(confObject);
    //
    // Check OpenSSL version for auto-update and subscriptions
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString();
    LOG(MODULE_NETWORK, "Current OpenSSL version: " + osslCurVersion)

    if (!QSslSocket::supportsSsl()) {
        LOG(MODULE_NETWORK, "Required OpenSSL version: " + osslReqVersion)
        LOG(MODULE_NETWORK, "OpenSSL library MISSING, Quitting.")
        QvMessageBox(nullptr, QObject::tr("DependencyMissing"),
                     QObject::tr("Cannot find openssl libs") + "\r\n" +
                     QObject::tr("This could be caused by a missing of `openssl` package in your system. Or an AppImage issue.") + "\r\n" +
                     QObject::tr("If you are using AppImage, please report a bug.") + "\r\n\r\n" +
                     QObject::tr("Please refer to Github Issue #65 to check for solutions.") + "\r\n" +
                     QObject::tr("Github Issue Link: ") + "https://github.com/lhy0403/Qv2ray/issues/65" + "\r\n\r\n" +
                     QObject::tr("Technical Details") + "\r\n" +
                     "OSsl.Rq.V=" + osslReqVersion + "\r\n" +
                     "OSsl.Cr.V=" + osslCurVersion);
        return -3;
    }

#ifdef Q_OS_WIN
    // Set special font in Windows
    QFont font;
    font.setPointSize(9);
    font.setFamily("微软雅黑");
    _qApp.setFont(font);
#endif
#ifdef QV2RAY_USE_BUILTIN_DARKTHEME
    LOG(MODULE_UI, "Using built-in theme.")

    if (confObject.uiConfig.useDarkTheme) {
        LOG(MODULE_UI, " --> Using built-in dark theme.")
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
        LOG(MODULE_INIT " " MODULE_UI, "Setting Qv2ray UI themes: " + confObject.uiConfig.theme)
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
            LOG(MODULE_INIT, "Quit triggered by session manager.");
        });
        auto rcode = _qApp.exec();
        LOG(MODULE_INIT, "Quitting normally")
        return rcode;
#ifndef QT_DEBUG
    }  catch (...) {
        QvMessageBox(nullptr, "ERROR", "There's something wrong happened and Qv2ray will quit now.");
        LOG(MODULE_INIT, "EXCEPTION THROWN: " __FILE__)
        return -99;
    }

#endif
}
