#include "Qv2rayApplication.hpp"
#include "common/QvHelpers.hpp"
#include "common/QvTranslator.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/handler/RouteHandler.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/styles/StyleManager.hpp"
#include "ui/windows/w_MainWindow.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QLocale>
#include <QObject>
#include <QStandardPaths>
#include <QTranslator>
#include <csignal>
#include <memory>

void signalHandler(int signum)
{
    std::cout << "Qv2ray: Interrupt signal (" << signum << ") received." << std::endl;
    ExitQv2ray();
    qvApp->exit(-99);
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
    // This line must be called before any other ones, since we are using these
    // values to identify instances.
    Qv2rayApplication::setApplicationVersion(QV2RAY_VERSION_STRING);
    //
#ifdef QT_DEBUG
    Qv2rayApplication::setApplicationName("qv2ray_debug");
    Qv2rayApplication::setApplicationDisplayName("Qv2ray - " + QObject::tr("Debug version"));
#else
    Qv2rayApplication::setApplicationName("qv2ray");
    Qv2rayApplication::setApplicationDisplayName("Qv2ray");
#endif
    //
    // parse the command line before starting as a Qt application
    Qv2rayApplication::PreInitilize(argc, argv);
    Qv2rayApplication app(argc, argv);
    if (app.SetupQv2ray())
    {
        LOG(MODULE_INIT, "Secondary instance detected.")
        return 0;
    }
    app.setQuitOnLastWindowClosed(false);
    //
    // Not duplicated.
    // Install a default translater. From the OS/DE
    Qv2rayTranslator = std::make_unique<QvTranslator>();
    const auto systemLang = QLocale::system().name();
    const auto setLangResult = Qv2rayTranslator->InstallTranslation(systemLang) ? "Succeed" : "Failed";
    LOG(MODULE_UI, "Installing a tranlator from OS: " + systemLang + " -- " + setLangResult)
    //
    LOG("LICENCE", NEWLINE                                                                                               //
        "This program comes with ABSOLUTELY NO WARRANTY." NEWLINE                                                        //
        "This is free software, and you are welcome to redistribute it" NEWLINE                                          //
        "under certain conditions." NEWLINE                                                                              //
            NEWLINE                                                                                                      //
        "Copyright (c) 2019-2020 Qv2ray Development Group." NEWLINE                                                      //
            NEWLINE                                                                                                      //
        "Libraries that have been used in Qv2ray are listed below (Sorted by date added):" NEWLINE                       //
        "Copyright (c) 2020 xyz347 (@xyz347): X2Struct (Apache)" NEWLINE                                                 //
        "Copyright (c) 2011 SCHUTZ Sacha (@dridk): QJsonModel (MIT)" NEWLINE                                             //
        "Copyright (c) 2020 Nikolaos Ftylitakis (@ftylitak): QZXing (Apache2)" NEWLINE                                   //
        "Copyright (c) 2016 Singein (@Singein): ScreenShot (MIT)" NEWLINE                                                //
        "Copyright (c) 2020 Itay Grudev (@itay-grudev): SingleApplication (MIT)" NEWLINE                                 //
        "Copyright (c) 2020 paceholder (@paceholder): nodeeditor (Qv2ray group modified version) (BSD-3-Clause)" NEWLINE //
        "Copyright (c) 2019 TheWanderingCoel (@TheWanderingCoel): ShadowClash (launchatlogin) (GPLv3)" NEWLINE           //
        "Copyright (c) 2020 Ram Pani (@DuckSoft): QvRPCBridge (WTFPL)" NEWLINE                                           //
        "Copyright (c) 2019 ShadowSocks (@shadowsocks): libQtShadowsocks (LGPLv3)" NEWLINE                               //
        "Copyright (c) 2015-2020 qBittorrent (Anton Lashkov) (@qBittorrent): speedplotview (GPLv2)" NEWLINE              //
        "Copyright (c) 2020 Diffusions Nu-book Inc. (@nu-book): zxing-cpp (Apache)" NEWLINE                              //
        "Copyright (c) 2020 feiyangqingyun: QWidgetDemo (Mulan PSL v1)" NEWLINE                                          //
            NEWLINE)                                                                                                     //
    //
#ifdef QT_DEBUG
    std::cout << "WARNING: =================== This is a debug build, many features are not stable enough. ===================" << std::endl;
#endif
    //
    // Qv2ray Initialize, find possible config paths and verify them.
    if (!qvApp->InitilizeConfigurations())
    {
        LOG(MODULE_INIT, "Failed to initialise Qv2ray, exiting.")
        return -1;
    }

    // Load the config for upgrade, but do not parse it to the struct.
    auto conf = JsonFromString(StringFromFile(QV2RAY_CONFIG_FILE));
    auto confVersion = conf["config_version"].toVariant().toString().toInt();

    if (confVersion > QV2RAY_CONFIG_VERSION)
    {
        // Config version is larger than the current version...
        // This is rare but it may happen....
        QvMessageBoxWarn(nullptr, QObject::tr("Qv2ray Cannot Continue"),
                         QObject::tr("You are running a lower version of Qv2ray compared to the current config file.") + NEWLINE +
                             QObject::tr("Please check if there's an issue explaining about it.") + NEWLINE +
                             QObject::tr("Or submit a new issue if you think this is an error.") + NEWLINE + NEWLINE +
                             QObject::tr("Qv2ray will now exit."));
        return -2;
    }

    if (confVersion < QV2RAY_CONFIG_VERSION)
    {
        // That is, config file needs to be upgraded.
        conf = Qv2ray::UpgradeSettingsVersion(confVersion, QV2RAY_CONFIG_VERSION, conf);
    }

    {
        // Load config object from upgraded config QJsonObject
        auto confObject = Qv2rayConfigObject::fromJson(conf);

        if (confObject.uiConfig.language.isEmpty())
        {
            // Prevent empty.
            LOG(MODULE_UI, "Setting default UI language to system locale.")
            confObject.uiConfig.language = QLocale::system().name();
        }

        if (Qv2rayTranslator->InstallTranslation(confObject.uiConfig.language))
        {
            LOG(MODULE_INIT, "Successfully installed a translator for " + confObject.uiConfig.language)
        }
        else
        {
            QvMessageBoxWarn(nullptr, "Translation Failed",
                             "Cannot load translation for " + confObject.uiConfig.language + ", English is now used." + NEWLINE + NEWLINE +
                                 "Please go to Preferences Window to change language or open an Issue");
        }

        // Let's save the config.
        SaveGlobalSettings(confObject);
    }
    //
    // Check OpenSSL version for auto-update and subscriptions
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString();
    LOG(MODULE_NETWORK, "Current OpenSSL version: " + osslCurVersion)

    if (!QSslSocket::supportsSsl())
    {
        LOG(MODULE_NETWORK, "Required OpenSSL version: " + osslReqVersion)
        LOG(MODULE_NETWORK, "OpenSSL library MISSING, Quitting.")
        QvMessageBoxWarn(nullptr, QObject::tr("Dependency Missing"),
                         QObject::tr("Cannot find openssl libs") + NEWLINE +
                             QObject::tr("This could be caused by a missing of `openssl` package in your system.") + NEWLINE +
                             QObject::tr("If you are using an AppImage from Github Action, please report a bug.") + NEWLINE + NEWLINE +
                             QObject::tr("Technical Details") + NEWLINE + "OSsl.Rq.V=" + osslReqVersion + NEWLINE +
                             "OSsl.Cr.V=" + osslCurVersion);
        return -3;
    }

#ifdef Q_OS_WIN
    // Set special font in Windows
    QFont font;
    font.setPointSize(9);
    font.setFamily("Microsoft YaHei");
    app.setFont(font);
#endif
    StyleManager = new QvStyleManager(qvApp);
    StyleManager->ApplyStyle(GlobalConfig.uiConfig.theme);

#if (QV2RAY_USE_BUILTIN_DARKTHEME)
    LOG(MODULE_UI, "Using built-in theme.")

    if (GlobalConfig.uiConfig.useDarkTheme)
    {
        LOG(MODULE_UI, " --> Using built-in dark theme.")
        // From https://forum.qt.io/topic/101391/windows-10-dark-theme/4
        app.setStyle("Fusion");
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
        app.setPalette(darkPalette);
        app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    }
#endif
    try
    {
        // Initialise Connection Handler
        PluginHost = new QvPluginHost();
        ConnectionManager = new QvConfigHandler(qvApp);
        RouteManager = new RouteHandler(qvApp);

#ifdef Q_OS_LINUX
        qvApp->setFallbackSessionManagementEnabled(false);
        QObject::connect(qvApp, &QGuiApplication::commitDataRequest, [] { //
            ConnectionManager->SaveConnectionConfig();
            LOG(MODULE_INIT, "Quit triggered by session manager.")
        });
#endif

        // Show MainWindow
        MainWindow w;
        QObject::connect(qvApp, &SingleApplication::receivedMessage, [&](quint32, QByteArray) {
            // When a second instance is connected, show the mainwindow.
            w.show();
            w.raise();
            w.activateWindow();
        });
#ifndef Q_OS_WIN
        signal(SIGUSR1, [](int) { ConnectionManager->RestartConnection(); });
        signal(SIGUSR2, [](int) { ConnectionManager->StopConnection(); });
#endif
        auto rcode = app.exec();
        delete PluginHost;
        LOG(MODULE_INIT, "Quitting normally")
        return rcode;
    }
    catch (std::exception e)
    {
        QvMessageBoxWarn(nullptr, "ERROR", "There's something wrong happened and Qv2ray will quit now.");
        LOG(MODULE_INIT, "EXCEPTION THROWN: " + QString(e.what()))
        return -99;
    }
}
