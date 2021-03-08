#include "Qv2rayPlatformApplication.hpp"

#include "core/settings/SettingsBackend.hpp"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QSessionManager>
#endif

#include <QSslSocket>
#define QV_MODULE_NAME "PlatformApplication"

#ifdef QT_DEBUG
const static inline QString QV2RAY_URL_SCHEME = "qv2ray-debug";
#else
const static inline QString QV2RAY_URL_SCHEME = "qv2ray";
#endif

QStringList Qv2rayPlatformApplication::CheckPrerequisites()
{
    QStringList errors;
    if (!QSslSocket::supportsSsl())
    {
        // Check OpenSSL version for auto-update and subscriptions
        const auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString();
        const auto osslCurVersion = QSslSocket::sslLibraryVersionString();
        LOG("Current OpenSSL version: " + osslCurVersion);
        LOG("Required OpenSSL version: " + osslReqVersion);
        errors << "Qv2ray cannot run without OpenSSL.";
        errors << "This is usually caused by using the wrong version of OpenSSL";
        errors << "Required=" + osslReqVersion + "Current=" + osslCurVersion;
    }
    return errors + checkPrerequisitesInternal();
}

bool Qv2rayPlatformApplication::Initialize()
{
    QString errorMessage;
    bool canContinue;
    const auto hasError = parseCommandLine(&errorMessage, &canContinue);
    if (hasError)
    {
        LOG("Command line:" A(errorMessage));
        if (!canContinue)
        {
            LOG("Fatal, Qv2ray cannot continue.");
            return false;
        }
        else
        {
            LOG("Non-fatal error, continue starting up.");
        }
    }

#ifdef Q_OS_WIN
    const auto appPath = QDir::toNativeSeparators(applicationFilePath());
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

    connect(this, &Qv2rayPlatformApplication::aboutToQuit, this, &Qv2rayPlatformApplication::quitInternal);
#ifndef QV2RAY_NO_SINGLEAPPLICATON
    connect(this, &SingleApplication::receivedMessage, this, &Qv2rayPlatformApplication::onMessageReceived, Qt::QueuedConnection);
    if (isSecondary())
    {
        StartupArguments.version = QV2RAY_VERSION_STRING;
        StartupArguments.buildVersion = QV2RAY_VERSION_BUILD;
        StartupArguments.fullArgs = arguments();
        if (StartupArguments.arguments.isEmpty())
            StartupArguments.arguments << Qv2rayStartupArguments::NORMAL;
        bool status = sendMessage(JsonToString(StartupArguments.toJson(), QJsonDocument::Compact).toUtf8());
        if (!status)
            LOG("Cannot send message.");
        SetExitReason(EXIT_SECONDARY_INSTANCE);
        return false;
    }
#endif

#ifdef QV2RAY_GUI
#ifdef Q_OS_LINUX
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    setFallbackSessionManagementEnabled(false);
#endif
    connect(this, &QGuiApplication::commitDataRequest, [] {
        RouteManager->SaveRoutes();
        ConnectionManager->SaveConnectionConfig();
        PluginHost->SavePluginSettings();
        SaveGlobalSettings();
    });
#endif

#ifdef Q_OS_WIN
    SetCurrentDirectory(applicationDirPath().toStdWString().c_str());
    // Set special font in Windows
    QFont font;
    font.setPointSize(9);
    font.setFamily("Microsoft YaHei");
    setFont(font);
#endif
#endif

    // Install a default translater. From the OS/DE
    Qv2rayTranslator = std::make_unique<QvTranslator>();
    Qv2rayTranslator->InstallTranslation(QLocale::system().name());
    const auto allTranslations = Qv2rayTranslator->GetAvailableLanguages();
    const auto osLanguage = QLocale::system().name();
    //
    LocateConfiguration();
    if (!allTranslations.contains(GlobalConfig.uiConfig.language))
    {
        // If we need to reset the language.
        if (allTranslations.contains(osLanguage))
        {
            GlobalConfig.uiConfig.language = osLanguage;
        }
        else if (!allTranslations.isEmpty())
        {
            GlobalConfig.uiConfig.language = allTranslations.first();
        }
    }

    if (!Qv2rayTranslator->InstallTranslation(GlobalConfig.uiConfig.language))
    {
        QvMessageBoxWarn(nullptr, "Translation Failed",
                         "Cannot load translation for " + GlobalConfig.uiConfig.language + NEWLINE + //
                             "English is now used." + NEWLINE + NEWLINE +                            //
                             "Please go to Preferences Window to change language or open an Issue");
        GlobalConfig.uiConfig.language = "en_US";
    }

    return true;
}

Qv2rayExitReason Qv2rayPlatformApplication::RunQv2ray()
{
    PluginHost = new QvPluginHost();
    RouteManager = new RouteHandler();
    ConnectionManager = new QvConfigHandler();
    return runQv2rayInternal();
}

void Qv2rayPlatformApplication::quitInternal()
{
    // Do not change the order.
    ConnectionManager->StopConnection();
    RouteManager->SaveRoutes();
    ConnectionManager->SaveConnectionConfig();
    PluginHost->SavePluginSettings();
    SaveGlobalSettings();
    terminateUIInternal();
    delete ConnectionManager;
    delete RouteManager;
    delete PluginHost;
    ConnectionManager = nullptr;
    RouteManager = nullptr;
    PluginHost = nullptr;
}

bool Qv2rayPlatformApplication::parseCommandLine(QString *errorMessage, bool *canContinue)
{
    *canContinue = true;
    QStringList filteredArgs;
    for (const auto &arg : arguments())
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
    QCommandLineOption debugLogOption("debug", QObject::tr("Enable debug output"));
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
    parser.addOption(debugLogOption);
    parser.addOption(noAutoConnectionOption);
    parser.addOption(disconnectOption);
    parser.addOption(reconnectOption);
    parser.addOption(exitOption);
    //
    const auto helpOption = parser.addHelpOption();
    const auto versionOption = parser.addVersionOption();

    if (!parser.parse(filteredArgs))
    {
        *canContinue = true;
        *errorMessage = parser.errorText();
        return false;
    }

    if (parser.isSet(versionOption))
    {
        parser.showVersion();
        return true;
    }

    if (parser.isSet(helpOption))
    {
        parser.showHelp();
        return true;
    }

    for (const auto &arg : parser.positionalArguments())
    {
        if (arg.startsWith(QV2RAY_URL_SCHEME + "://"))
        {
            StartupArguments.arguments << Qv2rayStartupArguments::QV2RAY_LINK;
            StartupArguments.links << arg;
        }
    }

    if (parser.isSet(exitOption))
    {
        DEBUG("disconnectOption is set.");
        StartupArguments.arguments << Qv2rayStartupArguments::EXIT;
    }

    if (parser.isSet(disconnectOption))
    {
        DEBUG("disconnectOption is set.");
        StartupArguments.arguments << Qv2rayStartupArguments::DISCONNECT;
    }

    if (parser.isSet(reconnectOption))
    {
        DEBUG("reconnectOption is set.");
        StartupArguments.arguments << Qv2rayStartupArguments::RECONNECT;
    }

#define ProcessExtraStartupOptions(option)                                                                                                           \
    DEBUG("Startup Options:" A(parser.isSet(option##Option)));                                                                                       \
    StartupArguments.option = parser.isSet(option##Option);

    ProcessExtraStartupOptions(noAPI);
    ProcessExtraStartupOptions(debugLog);
    ProcessExtraStartupOptions(noAutoConnection);
    ProcessExtraStartupOptions(noPlugins);
    return true;
}
