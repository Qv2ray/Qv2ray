#include <QFileInfo>
#include <QStandardPaths>
#include <QTranslator>
#include <QStyle>
#include <QLocale>
#include <QStyleFactory>

#include "QvUtils.hpp"
#include "Qv2rayBase.hpp"
#include "QvRunguard.hpp"
#include "w_MainWindow.hpp"

bool verifyConfigAvaliability(QString path, bool checkExistingConfig)
{
    if (!QDir(path).exists()) return false;

    QFile testFile(path + ".qv2ray_file_write_test_file" + QString::number(QTime::currentTime().msecsSinceStartOfDay()));
    bool opened = testFile.open(QFile::OpenModeFlag::ReadWrite);

    if (!opened) {
        LOG(MODULE_CONFIG, "Directory at: " + path.toStdString() + " cannot be used as a valid config file path.")
        LOG(MODULE_INIT, "---> Cannot create a new file or openwrite a file.")
        return false;
    } else {
        testFile.write("qv2ray test file, feel free to remove.");
        testFile.close();
        bool removed = testFile.remove();

        if (!removed) {
            LOG(MODULE_CONFIG, "Directory at: " + path.toStdString() + " cannot be used as a valid config file path.")
            LOG(MODULE_INIT, "---> Cannot remove a file.")
            return false;
        }
    }

    if (checkExistingConfig) {
        QFile configFile(path + "Qv2ray.conf");

        // No such config file.
        if (!configFile.exists()) return false;

        bool opened2 = configFile.open(QIODevice::ReadWrite);

        try {
            if (opened2) {
                // Verify if the config can be loaded.
                // Failed to parse if we changed the file structure...
                //auto conf = StructFromJsonString<Qv2rayConfig>(configFile.readAll());
                auto err = VerifyJsonString(StringFromFile(&configFile));

                if (!err.isEmpty()) {
                    LOG(MODULE_INIT, "Json parse returns: " + err.toStdString())
                    return false;
                } else {
                    auto conf = JsonFromString(StringFromFile(&configFile));
                    LOG(MODULE_CONFIG, "Path: " + path.toStdString() + " contains a config file, in version " + to_string(conf["config_version"].toInt()))
                    configFile.close();
                    return true;
                }
            } else {
                LOG(MODULE_CONFIG, "File: " + configFile.fileName().toStdString()  + " cannot be opened!")
                return false;
            }
        }  catch (...) {
            LOG(MODULE_CONFIG, "Exception raised when checking config: " + configFile.fileName().toStdString())
            //LOG(MODULE_INIT, e->what())
            QvMessageBox(nullptr, QObject::tr("Warning"), QObject::tr("Qv2ray cannot load the config file from here:") + NEWLINE + configFile.fileName());
            return false;
        }
    } else return true;
}

bool initialiseQv2ray()
{
    QStringList configFilePaths;
    configFilePaths << QDir::homePath() +  "/.qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
    configFilePaths << QDir::homePath() +  "/.config/qv2ray" QV2RAY_CONFIG_DIR_SUFFIX;
    configFilePaths << QDir::currentPath() + "/config" QV2RAY_CONFIG_DIR_SUFFIX;
    QString configPath = "";
    //
    bool hasExistingConfig = false;

    for (auto path : configFilePaths) {
        // No such directory.
        bool avail = verifyConfigAvaliability(path, true);

        if (avail) {
            DEBUG(MODULE_INIT, "Path: " + path.toStdString() + " is valid.")
            configPath = path;
            hasExistingConfig = true;
        } else {
            DEBUG(MODULE_INIT, "Path: " + path.toStdString() + " does not contain a valid config file.")
        }
    }

    // If there's no existing config.
    if (!hasExistingConfig) {
        // Create new config at these dirs.
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

        // Check if the dirs are writeable
        if (!mkpathResult || !verifyConfigAvaliability(configPath, false)) {
            // None of the path above can be used as a dir for storing config.
            LOG(MODULE_INIT, "FATAL")
            LOG(MODULE_INIT, " ---> CANNOT find a proper place to store Qv2ray config files.")
            QString searchPath = Stringify(configFilePaths, NEWLINE);
            QvMessageBox(nullptr, QObject::tr("Cannot Start Qv2ray"),
                         QObject::tr("Cannot find a place to store config files.") + NEWLINE +
                         QObject::tr("Qv2ray has searched these paths below:") +
                         NEWLINE + NEWLINE + searchPath + NEWLINE +
                         QObject::tr("Qv2ray will now exit."));
            return false;
        } else {
            LOG(MODULE_INIT, "Set " + configPath.toStdString() + " as the config path.")
            SetConfigDirPath(&configPath);

            if (QFile::exists(QV2RAY_CONFIG_FILE)) {
                LOG(MODULE_INIT, "This should not occur: Qv2ray config exists but failed to load.")
                QvMessageBox(nullptr, QObject::tr("Failed to initialise Qv2ray"),
                             QObject::tr("Failed to determine the location of config file.") + NEWLINE +
                             QObject::tr("Qv2ray will now exit.") + NEWLINE +
                             QObject::tr("Please report if you think it's a bug."));
                return false;
            }

            Qv2rayConfig conf;
            conf.v2AssetsPath = QV2RAY_DEFAULT_VASSETS_PATH.toStdString();
            conf.v2CorePath = QV2RAY_DEFAULT_VCORE_PATH.toStdString();
            conf.logLevel = 2;
            //
            // Save initial config.
            SetGlobalConfig(conf);
            LOG(MODULE_INIT, "Created initial config file.")
        }
    } else {
        SetConfigDirPath(&configPath);
        LOG(MODULE_INIT, "Using " + QV2RAY_CONFIG_DIR.toStdString() + " as the config path.")
    }

    if (!QDir(QV2RAY_GENERATED_DIR).exists()) {
        QDir().mkdir(QV2RAY_GENERATED_DIR);
        LOG(MODULE_INIT, "Created config generation dir at: " + QV2RAY_GENERATED_DIR.toStdString())
    }

    return true;
}


int main(int argc, char *argv[])
{
    // This line must be called before any other ones.
    QApplication _qApp(argc, argv);
    //
    // Install a default translater. From the OS/DE
    auto _lang = QLocale::system().name().replace("_", "-");

    if (_lang != "en-US") {
        bool _result_ = qApp->installTranslator(getTranslator(_lang));
        LOG(MODULE_UI, "Installing a tranlator from OS: " + _lang.toStdString() + " -- " + (_result_ ? "OK" : "Failed"))
    }

    LOG("LICENCE", NEWLINE "This program comes with ABSOLUTELY NO WARRANTY." NEWLINE
        "This is free software, and you are welcome to redistribute it" NEWLINE
        "under certain conditions." NEWLINE NEWLINE
        "Copyright (C) 2019 Leroy.H.Y (@lhy0403): Qv2ray Current Developer" NEWLINE
        "Copyright (C) 2019 Hork (@aliyuchang33): Hv2ray Initial Designs & gRPC implementation " NEWLINE
        "Copyright (C) 2019 SOneWinstone (@SoneWinstone): Hv2ray/Qv2ray HTTP Request Helper" NEWLINE
        "Qv2ray ArtWork Done By ArielAxionL (@axionl)" NEWLINE
        "TheBadGateway (@thebadgateway): Qv2ray Russian Translations" NEWLINE
        "Riko (@rikakomoe): Qv2ray patch 8a8c1a/PR115"
        NEWLINE NEWLINE
        "Libraries that have been used in Qv2ray are listed below (Sorted by date added):" NEWLINE
        "Copyright (c) 2019 dridk (@dridk): X2Struct (Apache)" NEWLINE
        "Copyright (c) 2011 SCHUTZ Sacha (@dridk): QJsonModel (MIT)" NEWLINE
        "Copyright (c) 2019 Nikolaos Ftylitakis (@ftylitak): QZXing (Apache2)" NEWLINE
        "Copyright (c) 2016 Singein (@Singein): ScreenShot (MIT)" NEWLINE
        "Copyright (c) 2016 Nikhil Marathe (@nikhilm): QHttpServer (MIT)" NEWLINE
        NEWLINE
        "Qv2ray " QV2RAY_VERSION_STRING " running on " +
        (QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture()).toStdString() + NEWLINE)
    //
    LOG(MODULE_INIT, "Qv2ray Start Time: "  + QString::number(QTime::currentTime().msecsSinceStartOfDay()).toStdString())
    DEBUG("DEBUG", "WARNING: ============================== This is a debug build, many features are not stable enough. ==============================")
    //
    // Initialise the language list.
    auto langs = GetFileList(QDir(":/translations"));

    if (langs.empty()) {
        LOG(MODULE_INIT, "FAILED to find any translations. THIS IS A BUILD ERROR.")
        QvMessageBox(nullptr, QObject::tr("Cannot load languages"), QObject::tr("Qv2ray will continue running, but you cannot change the UI language."));
    } else {
        for (auto lang : langs) {
            LOG(MODULE_INIT, "Found Translator: " + lang.toStdString())
        }
    }

    // Qv2ray Initialize
    if (!initialiseQv2ray()) {
        return -1;
    }

#ifdef QT_DEBUG
    RunGuard guard("Qv2ray-Instance-Identifier-DEBUG_VERSION");
#else
    RunGuard guard("Qv2ray-Instance-Identifier");
#endif

    if (!guard.isSingleInstance()) {
        LOG(MODULE_INIT, "Another Instance running, Quit.")
        QvMessageBox(nullptr, "Qv2ray", QObject::tr("Another instance of Qv2ray is already running."));
        return -1;
    }

    auto conf = CONFIGROOT(JsonFromString(StringFromFile(new QFile(QV2RAY_CONFIG_FILE))));
    //
    auto confVersion = conf["config_version"].toVariant().toString();
    auto newVersion = QSTRING(to_string(QV2RAY_CONFIG_VERSION));
    // Some config file upgrades.
    Q_UNLIKELY(confVersion.toInt() > QV2RAY_CONFIG_VERSION);

    if (confVersion.toInt() > QV2RAY_CONFIG_VERSION) {
        // Config version is larger than the current version...
        // This is rare but it may happen....
        QvMessageBox(nullptr, QObject::tr("Qv2ray Cannot Continue"),
                     QObject::tr("You are running a lower version of Qv2ray compared to the current config file.") + NEWLINE +
                     QObject::tr("Please report if you think this is an error.") + NEWLINE +
                     QObject::tr("Qv2ray will now exit."));
        return -3;
    } else if (confVersion != newVersion) {
        conf = Qv2ray::UpgradeConfig(confVersion.toInt(), QV2RAY_CONFIG_VERSION, conf);
    }

    auto confObject = StructFromJsonString<Qv2rayConfig>(JsonToString(conf));
    qApp->removeTranslator(getTranslator(_lang));
    LOG(MODULE_INIT, "Removing system translations")

    if (confObject.uiConfig.language.empty()) {
        LOG(MODULE_UI, "Setting default UI language to en-US")
        confObject.uiConfig.language = "en-US";
    }

    if (qApp->installTranslator(getTranslator(QSTRING(confObject.uiConfig.language)))) {
        LOG(MODULE_INIT, "Loaded Translator " + confObject.uiConfig.language)
    } else {
        // Do not translate these.....
        QvMessageBox(
            nullptr, "Translation Failed",
            "Cannot load translation for " + QSTRING(confObject.uiConfig.language) + ", English is now used.\r\n\r\n"
            "Please go to Prefrences Window to change or Report a Bug at: \r\n"
            "https://github.com/lhy0403/Qv2ray/issues/new");
    }

    SetGlobalConfig(confObject);
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString().toStdString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString().toStdString();
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
                     "OSsl.Rq.V=" + QSTRING(osslReqVersion) + "\r\n" +
                     "OSsl.Cr.V=" + QSTRING(osslCurVersion));
        return -2;
    }

#ifdef Q_OS_WIN
    // Set special font in Windows
    QFont font;
    font.setPointSize(9);
    font.setFamily("微软雅黑");
    _qApp.setFont(font);
#endif
#if QV2RAY_USE_BUILTIN_DARKTHEME
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
    QStringList themes = QStyleFactory::keys();

    if (themes.contains(QSTRING(confObject.uiConfig.theme))) {
        _qApp.setStyle(QSTRING(confObject.uiConfig.theme));
        LOG(MODULE_INIT " " MODULE_UI, "Setting Qv2ray UI themes: " + confObject.uiConfig.theme)
    }

#endif

    try {
        // Show MainWindow
        MainWindow w;
        auto rcode = _qApp.exec();
        LOG(MODULE_INIT, "Quitting normally")
        return rcode;
    }  catch (...) {
        QvMessageBox(nullptr, "ERROR", "There's something wrong happened and Qv2ray will quit now.");
        LOG(MODULE_INIT, "EXCEPTION THROWN: " __FILE__)
        return -9;
    }
}
