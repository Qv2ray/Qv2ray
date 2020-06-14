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
    if (!Qv2rayApplication::PreInitilize(argc, argv))
        return -1;

    Qv2rayApplication app(argc, argv);

    if (!app.SetupQv2ray())
        return 0;

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
    std::cerr << "WARNING: =================== This is a debug build, many features are not stable enough. ===================" << std::endl;
#endif
    //
    // Qv2ray Initialize, find possible config paths and verify them.
    if (!app.InitilizeConfigurations())
    {
        LOG(MODULE_INIT, "Failed to initialise Qv2ray, exiting.")
        return -1;
    }

    // Load the config for upgrade, but do not parse it to the struct.
    auto conf = JsonFromString(StringFromFile(QV2RAY_CONFIG_FILE));
    const auto configVersion = conf["config_version"].toInt();

    if (configVersion > QV2RAY_CONFIG_VERSION)
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

    if (configVersion < QV2RAY_CONFIG_VERSION)
    {
        // That is, config file needs to be upgraded.
        conf = Qv2ray::UpgradeSettingsVersion(configVersion, QV2RAY_CONFIG_VERSION, conf);
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

        if (!Qv2rayTranslator->InstallTranslation(confObject.uiConfig.language))
        {
            QvMessageBoxWarn(nullptr, "Translation Failed",
                             "Cannot load translation for " + confObject.uiConfig.language + NEWLINE + //
                                 "English is now used." + NEWLINE + NEWLINE +                          //
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

    try
    {
        // Initialise Connection Handler
        PluginHost = new QvPluginHost();
        ConnectionManager = new QvConfigHandler();
        RouteManager = new RouteHandler();
#ifndef Q_OS_WIN
        signal(SIGUSR1, [](int) { ConnectionManager->RestartConnection(); });
        signal(SIGUSR2, [](int) { ConnectionManager->StopConnection(); });
#endif

#ifdef Q_OS_LINUX
        qvApp->setFallbackSessionManagementEnabled(false);
        QObject::connect(qvApp, &QGuiApplication::commitDataRequest, [] {
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

        auto rcode = app.exec();
        delete ConnectionManager;
        delete RouteManager;
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
